#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>

#define BUFFER_SIZE 8192
#define MAX_WORD_LEN 256
#define HASH_SIZE 50000

typedef struct dict_entry {
    char* word;
    int has_capital;  // 1 if first letter is capital
    struct dict_entry* next;
} dict_entry_t;

typedef struct {
    dict_entry_t** buckets;
    int size;
} dict_t;

static dict_t* dictionary = NULL;
static int error_found = 0;

unsigned int hash(const char* str) {
    unsigned int hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;
    return hash % HASH_SIZE;
}

dict_t* dict_create() {
    dict_t* d = malloc(sizeof(dict_t));
    d->size = HASH_SIZE;
    d->buckets = calloc(HASH_SIZE, sizeof(dict_entry_t*));
    return d;
}

void to_lower(char* dest, const char* src) {
    int i = 0;
    while (src[i]) {
        dest[i] = tolower(src[i]);
        i++;
    }
    dest[i] = '\0';
}

void dict_add(dict_t* d, const char* word) {
    char lower[MAX_WORD_LEN];
    to_lower(lower, word);
    
    unsigned int h = hash(lower);
    
    // Check if already exists
    dict_entry_t* curr = d->buckets[h];
    while (curr) {
        if (strcmp(curr->word, lower) == 0) {
            // Update capitalization if needed
            if (isupper(word[0])) {
                curr->has_capital = 1;
            }
            return;
        }
        curr = curr->next;
    }
    
    // Add new entry
    dict_entry_t* entry = malloc(sizeof(dict_entry_t));
    entry->word = strdup(lower);
    entry->has_capital = isupper(word[0]);
    entry->next = d->buckets[h];
    d->buckets[h] = entry;
}

int dict_lookup(dict_t* d, const char* word) {
    char lower[MAX_WORD_LEN];
    to_lower(lower, word);
    
    unsigned int h = hash(lower);
    dict_entry_t* curr = d->buckets[h];
    
    while (curr) {
        if (strcmp(curr->word, lower) == 0) {
            if (curr->has_capital) {
                // Dictionary has capital, so input must have capital first letter
                return isupper(word[0]);
            }
            // Dictionary is lowercase, accept any case
            return 1;
        }
        curr = curr->next;
    }
    return 0;
}

int load_dictionary(const char* filename) {
    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        perror("Error opening dictionary");
        return -1;
    }
    
    dictionary = dict_create();
    
    char buffer[BUFFER_SIZE];
    char word[MAX_WORD_LEN];
    int word_len = 0;
    ssize_t bytes_read;
    
    while ((bytes_read = read(fd, buffer, BUFFER_SIZE)) > 0) {
        for (ssize_t i = 0; i < bytes_read; i++) {
            if (buffer[i] == '\n') {
                if (word_len > 0) {
                    word[word_len] = '\0';
                    dict_add(dictionary, word);
                    word_len = 0;
                }
            } else if (word_len < MAX_WORD_LEN - 1) {
                word[word_len++] = buffer[i];
            }
        }
    }
    
    if (word_len > 0) {
        word[word_len] = '\0';
        dict_add(dictionary, word);
    }
    
    close(fd);
    return 0;
}

int should_skip_word(const char* word) {
    int has_letter = 0;
    
    for (int i = 0; word[i]; i++) {
        if (isalpha(word[i])) {
            has_letter = 1;
            break;
        }
    }
    
    return !has_letter;
}

void normalize_word(char* dest, const char* src) {
    int start = 0;
    int len = strlen(src);
    
    // Skip leading opening punctuation
    while (start < len && (src[start] == '(' || src[start] == '[' || 
           src[start] == '{' || src[start] == '\'' || src[start] == '"')) {
        start++;
    }
    
    // Find end (before trailing non-alphanum)
    int end = len - 1;
    while (end >= start && !isalnum(src[end])) {
        end--;
    }
    
    // Copy normalized word
    int j = 0;
    for (int i = start; i <= end; i++) {
        dest[j++] = src[i];
    }
    dest[j] = '\0';
}

void check_file(const char* filename, int print_filename) {
    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "Error: could not open %s\n", filename);
        error_found = 1;
        return;
    }
    
    char buffer[BUFFER_SIZE];
    char word[MAX_WORD_LEN];
    int word_len = 0;
    int line = 1;
    int col = 1;
    int word_col = 1;
    ssize_t bytes_read;
    
    while ((bytes_read = read(fd, buffer, BUFFER_SIZE)) > 0) {
        for (ssize_t i = 0; i < bytes_read; i++) {
            char c = buffer[i];
            
            if (isspace(c)) {
                if (word_len > 0) {
                    word[word_len] = '\0';
                    
                    if (!should_skip_word(word)) {
                        char normalized[MAX_WORD_LEN];
                        normalize_word(normalized, word);
                        
                        if (normalized[0] && !dict_lookup(dictionary, normalized)) {
                            if (print_filename) {
                                printf("%s:%d:%d %s\n", filename, line, word_col, normalized);
                            } else {
                                printf("%d:%d %s\n", line, word_col, normalized);
                            }
                            error_found = 1;
                        }
                    }
                    
                    word_len = 0;
                }
                
                if (c == '\n') {
                    line++;
                    col = 1;
                } else {
                    col++;
                }
            } else {
                if (word_len == 0) {
                    word_col = col;
                }
                if (word_len < MAX_WORD_LEN - 1) {
                    word[word_len++] = c;
                }
                col++;
            }
        }
    }
    
    // Handle last word
    if (word_len > 0) {
        word[word_len] = '\0';
        if (!should_skip_word(word)) {
            char normalized[MAX_WORD_LEN];
            normalize_word(normalized, word);
            
            if (normalized[0] && !dict_lookup(dictionary, normalized)) {
                if (print_filename) {
                    printf("%s:%d:%d %s\n", filename, line, word_col, normalized);
                } else {
                    printf("%d:%d %s\n", line, word_col, normalized);
                }
                error_found = 1;
            }
        }
    }
    
    close(fd);
}

int ends_with(const char* str, const char* suffix) {
    int str_len = strlen(str);
    int suffix_len = strlen(suffix);
    
    if (suffix_len > str_len) return 0;
    
    return strcmp(str + str_len - suffix_len, suffix) == 0;
}

void process_directory(const char* dirname, const char* suffix) {
    DIR* dir = opendir(dirname);
    if (!dir) {
        fprintf(stderr, "Error: could not open directory %s\n", dirname);
        error_found = 1;
        return;
    }
    
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        // Skip . and .. and hidden files
        if (entry->d_name[0] == '.') continue;
        
        char path[1024];
        snprintf(path, sizeof(path), "%s/%s", dirname, entry->d_name);
        
        struct stat st;
        if (stat(path, &st) < 0) continue;
        
        if (S_ISDIR(st.st_mode)) {
            // Recursively process subdirectory
            process_directory(path, suffix);
        } else if (S_ISREG(st.st_mode)) {
            if (ends_with(entry->d_name, suffix)) {
                check_file(path, 1);
            }
        }
    }
    
    closedir(dir);
}

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s [-s suffix] dictionary [file...]\n", argv[0]);
        return EXIT_FAILURE;
    }
    
    char* suffix = ".txt";
    int arg_idx = 1;
    
    // Check for -s option
    if (strcmp(argv[arg_idx], "-s") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Error: -s requires suffix argument\n");
            return EXIT_FAILURE;
        }
        suffix = argv[arg_idx + 1];
        arg_idx += 2;
    }
    
    if (arg_idx >= argc) {
        fprintf(stderr, "Error: dictionary file required\n");
        return EXIT_FAILURE;
    }
    
    if (load_dictionary(argv[arg_idx]) < 0) {
        return EXIT_FAILURE;
    }
    arg_idx++;
    
    if (arg_idx >= argc) {
        check_file("/dev/stdin", 0);
    } else {
        int file_count = argc - arg_idx;
        
        for (int i = arg_idx; i < argc; i++) {
            struct stat st;
            if (stat(argv[i], &st) < 0) {
                fprintf(stderr, "Error: could not stat %s\n", argv[i]);
                error_found = 1;
                continue;
            }
            
            if (S_ISDIR(st.st_mode)) {
                process_directory(argv[i], suffix);
            } else {
                check_file(argv[i], file_count > 1);
            }
        }
    }
    
    return error_found ? EXIT_FAILURE : EXIT_SUCCESS;
}