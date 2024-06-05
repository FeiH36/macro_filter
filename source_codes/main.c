#include "file_processor.h"
#include <stdio.h>
#include <stdlib.h>

#define VERSION "1.0"   

int main(int argc, char *argv[]) {
    printf("Macro filter tool.\n");
    printf("Version: %s\n",VERSION);
    printf("Starting...\n");
    if (argc < 5) {
        fprintf(stderr, "Usage: %s <input_dir> <output_dir> <macro_name> <macro_branch> [config_file]\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *input_dir = argv[1];
    const char *output_dir = argv[2];
    const char *macro_name = argv[3];
    const char *macro_branch = argv[4];
    const char *config_file = (argc >= 6) ? argv[5] : NULL;

    Config config;
    if (config_file) {
        read_config_file(config_file, &config);
    } else {
        config.file_types = malloc(2 * sizeof(char *));
        config.file_types[0] = strdup(".c");
        config.file_types[1] = strdup(".h");
        config.num_file_types = 2;
        config.excluded_dirs = NULL;
        config.num_excluded_dirs = 0;
    }

    process_directory(input_dir, output_dir, macro_name, macro_branch, &config);

    free_config(&config);
    printf("Done!\n");
    return EXIT_SUCCESS;
}
