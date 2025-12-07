#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <ctype.h>
#include <poll.h>
#include <errno.h>
#include <sys/socket.h>
#include <stdarg.h>
#include <signal.h>
#include "network.h" 

#define MAX_MSG_LEN 104
#define NAME_LIMIT 72

typedef struct {
    int fd;
    char name[NAME_LIMIT + 1];
    int id; 
} Player;

typedef struct name_node {
    char name[NAME_LIMIT + 1];
    struct name_node *next;
} NameNode;

typedef struct {
    Player *p1;
    Player *p2;
    int piles[5];
    int turn; 
} GameState;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
Player *waiting_player = NULL;
int waiting_player_matched = 0; 
NameNode *active_names = NULL;

int register_name(const char *name) {
    NameNode *curr = active_names;
    while (curr) {
        if (strcmp(curr->name, name) == 0) return -1; 
        curr = curr->next;
    }
    NameNode *new_node = malloc(sizeof(NameNode));
    strncpy(new_node->name, name, NAME_LIMIT);
    new_node->name[NAME_LIMIT] = '\0';
    new_node->next = active_names;
    active_names = new_node;
    return 0;
}

void remove_name(const char *name) {
    NameNode **curr = &active_names;
    while (*curr) {
        if (strcmp((*curr)->name, name) == 0) {
            NameNode *temp = *curr;
            *curr = (*curr)->next;
            free(temp);
            return;
        }
        curr = &((*curr)->next);
    }
}

int send_msg(int fd, const char *fmt, ...) {
    char payload[MAX_MSG_LEN];
    va_list args;
    
    va_start(args, fmt);
    vsnprintf(payload, sizeof(payload), fmt, args);
    va_end(args);

    int len = strlen(payload); 
    if (len + 1 > 99) return -1; 

    char msg[MAX_MSG_LEN + 10];
    int total_len = snprintf(msg, sizeof(msg), "0|%02d|%s|", len + 1, payload);
    
    if (write(fd, msg, total_len) < 0) return -1;
    
    printf("[Log] Sent to %d: %s\n", fd, msg);
    return 0;
}

int send_error(int fd, int code, const char *msg) {
    return send_msg(fd, "FAIL|%d %s", code, msg);
}

int read_n(int fd, char *buf, int count) {
    int total = 0;
    while (total < count) {
        int r = read(fd, buf + total, count - total);
        if (r <= 0) return -1;
        total += r;
    }
    buf[total] = '\0';
    return 0;
}

int receive_msg(int fd, char **out_type, char **out_body) {
    char header[6]; 
    if (read_n(fd, header, 5) < 0) return -1;

    if (header[0] != '0' || header[1] != '|' || header[4] != '|') return 10; 

    char len_str[3] = {header[2], header[3], '\0'};
    if (!isdigit(len_str[0]) || !isdigit(len_str[1])) return 10;
    
    int len = atoi(len_str);
    if (len < 1 || len > MAX_MSG_LEN) return 10;

    char *buf = malloc(len + 1);
    if (read_n(fd, buf, len) < 0) { free(buf); return -1; }

    if (buf[len-1] != '|') { free(buf); return 10; }
    buf[len-1] = '\0'; 

    char *pipe = strchr(buf, '|');
    if (!pipe) { free(buf); return 10; }
    
    *pipe = '\0';
    *out_type = strdup(buf);
    *out_body = strdup(pipe + 1); 
    
    free(buf);
    printf("[Log] Recv from %d: %s|%s\n", fd, *out_type, *out_body);
    return 0;
}

// --- Game Logic ---

void *game_thread(void *arg) {
    GameState *game = (GameState *)arg;
    Player *p1 = game->p1;
    Player *p2 = game->p2;
    
    send_msg(p1->fd, "NAME|%d|%s", 1, p2->name);
    send_msg(p2->fd, "NAME|%d|%s", 2, p1->name);

    game->piles[0]=1; game->piles[1]=3; game->piles[2]=5; game->piles[3]=7; game->piles[4]=9;
    game->turn = 1;

    struct pollfd pfds[2];
    pfds[0].fd = p1->fd; pfds[0].events = POLLIN;
    pfds[1].fd = p2->fd; pfds[1].events = POLLIN;

    int winner = 0;
    char *reason = "";

    // Send Initial PLAY
    char board_str[64];
    snprintf(board_str, 64, "%d %d %d %d %d", 
        game->piles[0], game->piles[1], game->piles[2], game->piles[3], game->piles[4]);
    
    send_msg(p1->fd, "PLAY|%d|%s", game->turn, board_str);
    send_msg(p2->fd, "PLAY|%d|%s", game->turn, board_str);

    while (1) {
        int ret = poll(pfds, 2, -1);
        if (ret < 0) break;

        int active_idx = (game->turn == 1) ? 0 : 1;
        int idle_idx = (game->turn == 1) ? 1 : 0;
        Player *active_p = (game->turn == 1) ? p1 : p2;
        Player *idle_p = (game->turn == 1) ? p2 : p1;

        // 1. Check Idle Player (Impatient/Forfeit)
        if (pfds[idle_idx].revents & (POLLIN | POLLHUP)) {
            char *type = NULL, *body = NULL;
            int rc = receive_msg(idle_p->fd, &type, &body);
            if (rc == -1) {
                winner = game->turn; 
                reason = "Forfeit";
                break;
            }
            if (rc > 0) { send_error(idle_p->fd, rc, "Invalid"); close(idle_p->fd); break; }
            
            if (strcmp(type, "MOVE") == 0) {
                send_error(idle_p->fd, 31, "Impatient");
                // Resend Board to idle player to remind them
                send_msg(idle_p->fd, "PLAY|%d|%s", game->turn, board_str);
                free(type); free(body);
            } else {
                free(type); free(body);
            }
        }

        // 2. Check Active Player
        if (pfds[active_idx].revents & (POLLIN | POLLHUP)) {
            char *type = NULL, *body = NULL;
            int rc = receive_msg(active_p->fd, &type, &body);
            
            if (rc == -1) {
                winner = (game->turn == 1) ? 2 : 1;
                reason = "Forfeit";
                break;
            }
            if (rc > 0) { send_error(active_p->fd, rc, "Invalid"); close(active_p->fd); break; }

            if (strcmp(type, "MOVE") != 0) {
                free(type); free(body);
                continue; 
            }

            char *arg2 = strchr(body, '|');
            if (!arg2) { send_error(active_p->fd, 10, "Invalid"); close(active_p->fd); break; }
            *arg2 = '\0';
            arg2++;

            int pile_idx = atoi(body);
            int amount = atoi(arg2);

            free(type); free(body);

            if (pile_idx < 0 || pile_idx > 4) {
                send_error(active_p->fd, 32, "Pile Index");
                // Resend PLAY to let them try again
                send_msg(active_p->fd, "PLAY|%d|%s", game->turn, board_str);
                continue; 
            }
            if (amount < 1 || amount > game->piles[pile_idx]) {
                send_error(active_p->fd, 33, "Quantity");
                // Resend PLAY to let them try again
                send_msg(active_p->fd, "PLAY|%d|%s", game->turn, board_str);
                continue; 
            }

            game->piles[pile_idx] -= amount;

            int stones = 0;
            for(int i=0; i<5; i++) stones += game->piles[i];
            if (stones == 0) {
                winner = game->turn;
                reason = "";
                break;
            }

            game->turn = (game->turn == 1) ? 2 : 1;

            snprintf(board_str, 64, "%d %d %d %d %d", 
                game->piles[0], game->piles[1], game->piles[2], game->piles[3], game->piles[4]);
            
            send_msg(p1->fd, "PLAY|%d|%s", game->turn, board_str);
            send_msg(p2->fd, "PLAY|%d|%s", game->turn, board_str);
        }
    }

    snprintf(board_str, 64, "%d %d %d %d %d", 
        game->piles[0], game->piles[1], game->piles[2], game->piles[3], game->piles[4]);

    if (winner != 0) {
        if (strlen(reason) > 0) {
            send_msg(p1->fd, "OVER|%d|%s|%s", winner, board_str, reason);
            send_msg(p2->fd, "OVER|%d|%s|%s", winner, board_str, reason);
        } else {
            send_msg(p1->fd, "OVER|%d|%s|", winner, board_str);
            send_msg(p2->fd, "OVER|%d|%s|", winner, board_str);
        }
    }

    close(p1->fd);
    close(p2->fd);
    
    pthread_mutex_lock(&lock);
    remove_name(p1->name);
    remove_name(p2->name);
    pthread_mutex_unlock(&lock);

    free(p1);
    free(p2);
    free(game);
    return NULL;
}

// --- Client Handshake Thread ---

void *handle_client(void *arg) {
    int fd = *(int*)arg;
    free(arg);

    char *type = NULL, *body = NULL;
    
    int rc = receive_msg(fd, &type, &body);
    if (rc != 0) {
        if (rc > 0) send_error(fd, rc, "Invalid");
        close(fd); return NULL;
    }

    if (strcmp(type, "OPEN") != 0) {
        send_error(fd, 10, "Expected OPEN"); 
        free(type); free(body); close(fd); return NULL;
    }

    char *name = body;
    if (strlen(name) > NAME_LIMIT) {
        send_error(fd, 21, "Long Name");
        free(type); free(body); close(fd); return NULL;
    }

    pthread_mutex_lock(&lock);
    if (register_name(name) != 0) {
        pthread_mutex_unlock(&lock);
        send_error(fd, 22, "Already Playing");
        free(type); free(body); close(fd); return NULL;
    }
    pthread_mutex_unlock(&lock);

    Player *me = malloc(sizeof(Player));
    me->fd = fd;
    strcpy(me->name, name);
    free(type); free(body);

    send_msg(fd, "WAIT");

    pthread_mutex_lock(&lock);
    if (waiting_player == NULL) {
        waiting_player = me;
        waiting_player_matched = 0;
        
        struct pollfd pfd = { .fd = fd, .events = POLLIN };
        
        while (waiting_player == me) {
            pthread_mutex_unlock(&lock);
            
            int ret = poll(&pfd, 1, 100); 
            
            pthread_mutex_lock(&lock);
            if (waiting_player != me) break; 

            if (ret > 0) {
                waiting_player = NULL; 
                remove_name(me->name);
                pthread_mutex_unlock(&lock);
                
                char *type = NULL, *body = NULL;
                int rc = receive_msg(fd, &type, &body);
                
                if (rc == -1) {
                } else if (rc > 0) {
                    send_error(fd, rc, "Invalid");
                } else {
                    if (strcmp(type, "OPEN") == 0) {
                        send_error(fd, 23, "Already Open");
                    } else {
                        send_error(fd, 24, "Not Playing");
                    }
                    free(type); free(body);
                }

                close(fd); free(me);
                return NULL;
            }
        }
        pthread_mutex_unlock(&lock);
    } else {
        Player *opponent = waiting_player;
        waiting_player = NULL;
        
        GameState *game = malloc(sizeof(GameState));
        game->p1 = opponent;
        game->p2 = me;
        
        pthread_t tid;
        pthread_create(&tid, NULL, game_thread, game);
        pthread_detach(tid);
        
        pthread_mutex_unlock(&lock);
    }

    return NULL;
}

int main(int argc, char **argv) {
    signal(SIGPIPE, SIG_IGN); 

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    int listener = open_listener(argv[1], 8);
    if (listener < 0) exit(1);

    printf("[Log] Server listening on port %s\n", argv[1]);

    while (1) {
        struct sockaddr_storage addr;
        socklen_t len = sizeof(addr);
        int client_fd = accept(listener, (struct sockaddr *)&addr, &len);
        
        if (client_fd < 0) {
            perror("accept");
            continue;
        }

        int *arg = malloc(sizeof(int));
        *arg = client_fd;
        
        pthread_t tid;
        if (pthread_create(&tid, NULL, handle_client, arg) != 0) {
            perror("pthread_create");
            close(client_fd);
            free(arg);
        } else {
            pthread_detach(tid);
        }
    }

    return 0;
}