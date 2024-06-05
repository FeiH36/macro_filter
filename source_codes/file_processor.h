#ifndef FILE_PROCESSOR_H_
#define FILE_PROCESSOR_H_

#include <stddef.h>

typedef struct {
    char **file_types;
    int num_file_types;
    char **excluded_dirs;
    int num_excluded_dirs;
} Config;

// Evaluates if a condition in a line matches the macro definition.
int evaluate_condition(const char *line, const char *macro_name, const char *macro_branch);

// Processes a code file to keep only the relevant parts based on the macro definition.
void process_code(const char *input_file, const char *output_file, const char *macro_name, const char *macro_branch);

// Creates the specified directory and its parent directories if they do not exist.
void create_directory(const char *path);

// Copies a file from the source to the destination.
void copy_file(const char *src_file, const char *dst_file);

// Checks if a given file type is supported based on the configuration.
int is_file_type_supported(const char *filename, const Config *config);

// Checks if a directory is excluded based on the configuration.
int is_directory_excluded(const char *dirname, const Config *config);

// Reads the configuration file to get the file types to be processed and directories to be excluded.
void read_config_file(const char *config_file, Config *config);

// Processes a directory recursively to handle all files and subdirectories based on the configuration.
void process_directory(const char *input_dir, const char *output_dir, const char *macro_name, const char *macro_branch, const Config *config);

// Frees the memory allocated for the configuration.
void free_config(Config *config);

#endif // FILE_PROCESSOR_H
