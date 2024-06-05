#include "file_processor.h"
#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>

#define MAX_LINE_LENGTH 1024
#define MAX_STACK_SIZE 256


int evaluate_macro(const char *line, const char *macro_name) {
    assert(macro_name);

    char *line_copy = strdup(line);  // Create a copy of line because strtok modifies the original string
    if (line_copy == NULL) {
        perror("strdup failed");
        exit(EXIT_FAILURE);
    }

    char *token_line = strtok(line_copy, " \t");  // Split by space and tab
    if (token_line == NULL || strcmp(token_line, macro_name) != 0) {
        free(line_copy);  // Free the allocated memory
        return 0;
    }
    free(line_copy);  // Free the allocated memory
    return 1;
}

int evaluate_condition(const char *line, const char *macro_name, const char *macro_branch) {
    assert(macro_name);
    assert(macro_branch);

    char *line_copy = strdup(line);  // Create a copy of line because strtok modifies the original string
    if (line_copy == NULL) {
        perror("strdup failed");
        exit(EXIT_FAILURE);
    }

    char *token_line = strtok(line_copy, " \t");  // Split by space and tab
    if (token_line == NULL || strcmp(token_line, macro_name) != 0) {
        free(line_copy);  // Free the allocated memory
        return 0;
    }

    token_line = strtok(NULL, " \t");
    if (token_line == NULL || strcmp(token_line, "==") != 0) {
        free(line_copy);  // Free the allocated memory
        return 0;
    }

    token_line = strtok(NULL, " \t\n");
    if (token_line == NULL || strcmp(token_line, macro_branch) != 0) {
        free(line_copy);  // Free the allocated memory
        return 0;
    }

    free(line_copy);  // Free the allocated memory
    return 1;
}

void process_code(const char *input_file, const char *output_file, const char *macro_name, const char *macro_branch) {
    FILE *infile = fopen(input_file, "r");
    if (!infile) {        
        perror("File open error");
        exit(EXIT_FAILURE);
    }

    FILE *outfile = fopen(output_file, "w");
    if (!outfile) {  
        perror("File open error");
        fclose(infile);
        exit(EXIT_FAILURE);
    }

    char line[MAX_LINE_LENGTH];
    bool is_macro_end = false;
    bool is_macro_line = false;
    bool is_valid_range_4othermacro = true;
    bool keep_lines = true;
    bool keep_stack[MAX_STACK_SIZE];    // the macro condition has been satisfied in current case.
    bool is_the_macro_stack[MAX_STACK_SIZE];
    int stack_top = 0;

    keep_stack[0] = true;  // first default not skip
    is_the_macro_stack[0] = false;  // first default not in the macro
    while (fgets(line, sizeof(line), infile)) {
        char *line_tmp = strdup(line);
        if (line_tmp == NULL) {
            perror("strdup failed");
            fclose(infile);
            fclose(outfile);
            exit(EXIT_FAILURE);
        }

        char *first_word = strtok(line_tmp, " \t");
        if (first_word == NULL) {
            free(line_tmp);
            continue;
        }

        if (strncmp(first_word, "#if", 3) == 0) {
            is_macro_line = true;
            char *second_word = strtok(NULL, " \t");
            if (evaluate_condition(line + 3, macro_name, macro_branch)) { // if condition is true
                is_valid_range_4othermacro = true;
                keep_stack[++stack_top] = true;
                keep_lines = true;
                is_the_macro_stack[stack_top] = true;
            } else {
                if (evaluate_macro(second_word, macro_name)) {
                    is_valid_range_4othermacro = false;
                    keep_stack[++stack_top] = false;
                    keep_lines = false;
                    is_the_macro_stack[stack_top] = true;
                } else {  // has other #if case
                    if (is_valid_range_4othermacro) {
                        keep_lines = true;
                    }
                    keep_stack[++stack_top] = true;
                    is_the_macro_stack[stack_top] = false;
                }
            }
        } else if ((strncmp(first_word, "#else if", 8) == 0) || (strncmp(first_word, "#elif", 5) == 0)) {
            is_macro_line = true;
            if (is_the_macro_stack[stack_top] == true) {
                if (stack_top >= 0) {
                    uint8_t token_size = (strncmp(first_word, "#else if", 8) == 0) ? 8 : 5;
                    keep_lines = !keep_stack[stack_top] && evaluate_condition(line + token_size, macro_name, macro_branch);
                    if (keep_lines) {
                        is_valid_range_4othermacro = true;
                        keep_stack[stack_top] = true;
                    } else {
                        is_valid_range_4othermacro = false;
                    }
                }
            }
        } else if (strncmp(first_word, "#else", 5) == 0) {
            is_macro_line = true;
            if (is_the_macro_stack[stack_top] == true) {
                if (stack_top >= 0) {
                    keep_lines = !keep_stack[stack_top];
                    if (keep_lines) {
                        is_valid_range_4othermacro = true;
                        keep_stack[stack_top] = true;
                    } else {
                        is_valid_range_4othermacro = false;
                    }
                }
            }
        } else if (strncmp(first_word, "#endif", 6) == 0) {
            is_macro_line = true;
            if (stack_top >= 0) {
                is_macro_end = true;
            }
        } else {
            is_macro_line = false;
        }

        if (keep_lines) {
            if (is_macro_line) {
                if (is_the_macro_stack[stack_top] == false) {
                    fputs(line, outfile);
                }
            } else {
                fputs(line, outfile);
            }
        }

        free(line_tmp);

        if (is_macro_end) {
            keep_lines = keep_stack[--stack_top];
            is_macro_end = false;
        }
    }

    fclose(infile);
    fclose(outfile);
}


void create_directory(const char *path) {
    char tmp[256];
    snprintf(tmp, sizeof(tmp), "%s", path);
    char *p;
    for (p = tmp + 1; *p; p++) {
        if (*p == '/') {
            *p = '\0';
            // if (mkdir(tmp, 0755) != 0 && errno != EEXIST) {
            if (mkdir(tmp) != 0 && errno != EEXIST) {
                perror("mkdir error");
                exit(EXIT_FAILURE);
            }
            *p = '/';
        }
    }
    // if (mkdir(tmp, 0755) != 0 && errno != EEXIST) {
    if (mkdir(tmp) != 0 && errno != EEXIST) {
        perror("mkdir error");
        exit(EXIT_FAILURE);
    }
}

void copy_file(const char *src_file, const char *dst_file) {
    FILE *src = fopen(src_file, "r");
    FILE *dst = fopen(dst_file, "w");    
    if (!src) {        
        perror("File open error");
        exit(EXIT_FAILURE);
    }
    if (!dst) {  
        perror("File open error");
        exit(EXIT_FAILURE);
    }

    char buffer[1024];
    size_t n;
    while ((n = fread(buffer, 1, sizeof(buffer), src)) > 0) {
        fwrite(buffer, 1, n, dst);
    }

    fclose(src);
    fclose(dst);
}

int is_file_type_supported(const char *filename, const Config *config) {
    const char *ext = strrchr(filename, '.');
    int i;
    if (!ext) return 0;
    for (i = 0; i < config->num_file_types; ++i) {
        if (strcmp(ext, config->file_types[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

int is_directory_excluded(const char *dirname, const Config *config) {
    int i;
    for (i = 0; i < config->num_excluded_dirs; ++i) {
        if (strstr(dirname, config->excluded_dirs[i]) != NULL) {
            return 1;
        }
    }
    return 0;
}

void read_config_file(const char *config_file, Config *config) {
    FILE *file = fopen(config_file, "r");
    if (!file) {
        perror("Config file open error");
        exit(EXIT_FAILURE);
    }

    char line[256];
    config->file_types = NULL;
    config->num_file_types = 0;
    config->excluded_dirs = NULL;
    config->num_excluded_dirs = 0;

    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "check_file_format:", 18) == 0) {
            config->file_types = realloc(config->file_types, sizeof(char*) * (config->num_file_types + 1));
            config->file_types[config->num_file_types] = strdup(line + 18);
            config->file_types[config->num_file_types][strcspn(config->file_types[config->num_file_types], "\n")] = 0;
            config->num_file_types++;
        } else if (strncmp(line, "exclude_dir:", 12) == 0) {
            config->excluded_dirs = realloc(config->excluded_dirs, sizeof(char*) * (config->num_excluded_dirs + 1));
            config->excluded_dirs[config->num_excluded_dirs] = strdup(line + 12);
            config->excluded_dirs[config->num_excluded_dirs][strcspn(config->excluded_dirs[config->num_excluded_dirs], "\n")] = 0;
            config->num_excluded_dirs++;
        }
    }

    fclose(file);
}

void process_directory(const char *input_dir, const char *output_dir, const char *macro_name, const char *macro_branch, const Config *config) {
    DIR *dir = opendir(input_dir);
    if (!dir) {
        perror("opendir error");
        exit(EXIT_FAILURE);
    }

    struct dirent *entry;
    while ((entry = readdir(dir))) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char input_path[1024];
        char output_path[1024];
        snprintf(input_path, sizeof(input_path), "%s/%s", input_dir, entry->d_name);
        snprintf(output_path, sizeof(output_path), "%s/%s", output_dir, entry->d_name);

        struct stat statbuf;
        if (stat(input_path, &statbuf) == -1) {
            perror("stat error");
            exit(EXIT_FAILURE);
        }
        if (S_ISDIR(statbuf.st_mode)) { //is a directory.
            printf("folder: %s\n", input_path);
            create_directory(output_path);
            process_directory(input_path, output_path, macro_name, macro_branch, config);
        } else if (S_ISREG(statbuf.st_mode)) {    //is a regular file.
            printf("file: %s\n", input_path);
            if (is_directory_excluded(input_path, config)) {
                printf("is excluded\n");
                copy_file(input_path, output_path);
            }else{
                printf("is not excluded\n");
                if(is_file_type_supported(entry->d_name, config)) {
                    printf("is supported\n");
                    process_code(input_path, output_path, macro_name, macro_branch);
                } else {
                    printf("is not supported\n");
                    copy_file(input_path, output_path);
                }
            }
        }else{  //is neither a directory nor a regular file.
            printf("neither a directory nor a regular file: %s\n", input_path);
        }
    }

    closedir(dir);
}

void free_config(Config *config) {
    int i;
    for (i = 0; i < config->num_file_types; ++i) {
        free(config->file_types[i]);
    }
    free(config->file_types);

    for (i = 0; i < config->num_excluded_dirs; ++i) {
        free(config->excluded_dirs[i]);
    }
    free(config->excluded_dirs);
}
