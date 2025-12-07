#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdarg.h>
#include <errno.h>
#include <time.h>    
#include "network.h" 

#define HOST "localhost"
#define BUF_SIZE 1024 
#define BIG_BUF 16384 

char *TARGET_PORT = "5000";

// --- Helper Functions ---

void sleep_ms(int ms) {
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000;
    nanosleep(&ts, NULL);
}

void send_raw(int fd, const char *str) {
    write(fd, str, strlen(str));
    sleep_ms(100); 
}

void send_ngp(int fd, const char *fmt, ...) {
    char payload[256];
    va_list args;
    va_start(args, fmt);
    vsnprintf(payload, sizeof(payload), fmt, args);
    va_end(args);

    char full_payload[260];
    snprintf(full_payload, sizeof(full_payload), "%s|", payload);

    int len = strlen(full_payload);
    char msg[300];
    snprintf(msg, sizeof(msg), "0|%02d|%s", len, full_payload);

    if (write(fd, msg, strlen(msg)) < 0) {
        // write error
    }
    sleep_ms(50); 
}

int expect(int fd, const char *pattern) {
    char buf[BUF_SIZE];
    int len = read(fd, buf, BUF_SIZE - 1);
    if (len <= 0) return 0;
    buf[len] = '\0';
    if (strstr(buf, pattern)) return 1;
    return 0;
}

// --- Generic Game Logic ---

typedef struct {
    char name[20];
    int role;      // 1 (First/Winner) or 2 (Second/Loser)
    int is_winner; // 1 if expected to win
} PlayerArgs;

void *auto_player(void *arg) {
    PlayerArgs *pa = (PlayerArgs*)arg;
    int sock = connect_inet(HOST, TARGET_PORT);
    if (sock < 0) {
        fprintf(stderr, "[%s] Connection failed\n", pa->name);
        return NULL;
    }

    send_ngp(sock, "OPEN|%s", pa->name);

    char buf[BIG_BUF];
    int valid_len = 0;

    while (1) {
        int r = read(sock, buf + valid_len, BIG_BUF - valid_len - 1);
        if (r <= 0) break;
        valid_len += r;
        buf[valid_len] = '\0';

        if (strstr(buf, "OVER")) {
            if (pa->is_winner && strstr(buf, "|1|")) 
                printf("[%s] SUCCESS: Won as expected.\n", pa->name);
            else if (!pa->is_winner && strstr(buf, "|1|"))
                printf("[%s] SUCCESS: Lost as expected.\n", pa->name);
            else
                printf("[%s] FAIL: Unexpected result. Buf: %s\n", pa->name, buf);
            break; 
        }

        if (strstr(buf, "FAIL")) {
            printf("[%s] ERROR: Got FAIL: %s\n", pa->name, buf);
            break;
        }

        char *cursor = buf;
        int action_taken = 0;

        while ((cursor = strstr(cursor, "PLAY"))) {
            if (cursor + 6 > buf + valid_len) break; 
            int turn = cursor[5] - '0';
            
            // If it is my turn, make a move
            if (turn == pa->role) {
                // Logic for Player 1 (Winner Path)
                if (pa->role == 1) {
                    if (strstr(cursor, "1 3 5 7 9")) {
                        printf("[%s] Moving 4,9\n", pa->name);
                        send_ngp(sock, "MOVE|4|9");
                        action_taken = 1;
                    }
                    else if (strstr(cursor, "1 3 5 0 0")) { 
                        printf("[%s] Moving 2,5\n", pa->name);
                        send_ngp(sock, "MOVE|2|5");
                        action_taken = 1;
                    }
                    else if (strstr(cursor, "1 0 0 0 0")) {
                         printf("[%s] Moving 0,1\n", pa->name);
                         send_ngp(sock, "MOVE|0|1");
                         action_taken = 1;
                    }
                }
                // Logic for Player 2 (Loser Path)
                else if (pa->role == 2) {
                    if (strstr(cursor, "1 3 5 7 0")) {
                        printf("[%s] Moving 3,7\n", pa->name);
                        send_ngp(sock, "MOVE|3|7");
                        action_taken = 1;
                    }
                    else if (strstr(cursor, "1 3 0 0 0")) {
                        printf("[%s] Moving 1,3\n", pa->name);
                        send_ngp(sock, "MOVE|1|3");
                        action_taken = 1;
                    }
                }
            }
            cursor++;
        }
        if (action_taken) { valid_len = 0; buf[0] = '\0'; }
    }
    close(sock);
    return NULL;
}

// --- Test 1: Happy Path ---

void run_happy() {
    printf("--- Running Happy Path (Port %s) ---\n", TARGET_PORT);
    pthread_t t1, t2;
    PlayerArgs p1 = {"Alice", 1, 1};
    PlayerArgs p2 = {"Bob", 2, 0};

    pthread_create(&t1, NULL, auto_player, &p1);
    sleep_ms(200);
    pthread_create(&t2, NULL, auto_player, &p2);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    printf("--- Happy Path Complete ---\n");
}

// --- Test 2: Concurrency ---

void run_concurrent() {
    printf("--- Running Concurrency Test (Port %s) ---\n", TARGET_PORT);
    pthread_t t1, t2, t3, t4;
    
    PlayerArgs p1 = {"Alice", 1, 1};
    PlayerArgs p2 = {"Bob", 2, 0};
    
    PlayerArgs p3 = {"Charlie", 1, 1};
    PlayerArgs p4 = {"Dave", 2, 0};

    printf("Starting Game 1 (Alice vs Bob)...\n");
    pthread_create(&t1, NULL, auto_player, &p1);
    sleep_ms(100);
    pthread_create(&t2, NULL, auto_player, &p2);

    printf("Starting Game 2 (Charlie vs Dave) immediately...\n");
    // If server is iterative, these will block or fail.
    // If concurrent, they will play simultaneously.
    pthread_create(&t3, NULL, auto_player, &p3);
    sleep_ms(100);
    pthread_create(&t4, NULL, auto_player, &p4);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);
    pthread_join(t4, NULL);
    
    printf("--- Concurrency Test Complete ---\n");
}

// --- Test 3: Error Cases ---

void run_case(const char *desc, void (*func)(int), const char *expect_code) {
    printf("Testing %s... ", desc);
    int s = connect_inet(HOST, TARGET_PORT);
    if (s < 0) { printf("Connect failed\n"); return; }
    func(s);
    if (expect(s, expect_code)) printf("PASS (%s)\n", expect_code);
    else printf("FAIL\n");
    close(s);
}

void case_a(int s) { send_raw(s, "9|99|GARBAGE|"); }
void case_b(int s) { 
    char long_name[80];
    memset(long_name, 'A', 75);
    long_name[75] = '\0';
    send_ngp(s, "OPEN|%s", long_name); 
}
void case_d(int s) {
    send_ngp(s, "OPEN|Tester");
    sleep_ms(100);
    send_ngp(s, "OPEN|Tester");
}
void case_e(int s) {
    send_ngp(s, "OPEN|Tester");
    sleep_ms(100);
    send_ngp(s, "MOVE|0|1");
}

void run_errors() {
    printf("--- Running Error Tests (Port %s) ---\n", TARGET_PORT);
    run_case("Case A (Framing)", case_a, "FAIL|10");
    run_case("Case B (LongName)", case_b, "FAIL|21");
    run_case("Case D (DoubleOpen)", case_d, "FAIL|23");
    run_case("Case E (EarlyMove)", case_e, "FAIL|24");
}

// --- Test 4: Extra Credit Impatience Case ---

void *impatient_p1(void *arg) {
    int s = connect_inet(HOST, TARGET_PORT);
    send_ngp(s, "OPEN|P1");
    
    char buf[BUF_SIZE];
    while(read(s, buf, sizeof(buf)) > 0) {
        if (strstr(buf, "FAIL|31")) {
            printf("[P1] PASS: Received Impatient Error.\n");
            return NULL;
        }
    }
    return NULL;
}

void run_extra() {
    printf("--- Running Impatience Test (Port %s) ---\n", TARGET_PORT);
    
    printf("Connecting P1...\n");
    int sock1 = connect_inet(HOST, TARGET_PORT);
    if (sock1 < 0) return;
    send_ngp(sock1, "OPEN|Me");
    
    printf("Connecting P2...\n");
    int sock2 = connect_inet(HOST, TARGET_PORT);
    if (sock2 < 0) { close(sock1); return; }
    send_ngp(sock2, "OPEN|Bot");
    
    sleep_ms(500); // Wait for match
    
    // Drain buffer
    char buf[1024];
    read(sock1, buf, sizeof(buf)); 
    read(sock2, buf, sizeof(buf));

    // P1 makes valid move
    send_ngp(sock1, "MOVE|0|1"); 
    sleep_ms(100);
    
    // It is now P2's turn. P1 sends move again (IMPATIENT)
    printf("P1 sending move out of turn...\n");
    send_ngp(sock1, "MOVE|1|1");
    
    if (expect(sock1, "FAIL|31")) {
        printf("PASS: Server sent FAIL 31 Impatient.\n");
    } else {
        printf("FAIL: Server did not send FAIL 31.\n");
    }
    
    close(sock1);
    close(sock2);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s [happy|concurrent|errors|extra] <optional_port>\n", argv[0]);
        return 1;
    }
    if (argc >= 3) TARGET_PORT = argv[2];

    if (strcmp(argv[1], "happy") == 0) run_happy();
    else if (strcmp(argv[1], "concurrent") == 0) run_concurrent();
    else if (strcmp(argv[1], "errors") == 0) run_errors();
    else if (strcmp(argv[1], "extra") == 0) run_extra();
    else printf("Unknown mode.\n");

    return 0;
}