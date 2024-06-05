# Macro filter

`macro_filter` is a command-line tool designed to process source files based on specified macro conditions. It can filter code sections enclosed within macro directives and copy files or directories while considering the specified configuration.

## Features

- Filter and process source files based on macro conditions (`#if`, `#else if`, `#elif`, `#else`, `#endif`).
- Copy entire files and directories.
- Support for custom file types and excluded directories.

## Configuration

Create a configuration file (e.g., `config.txt`) to specify the file types to be processed and directories to be excluded. The configuration file should have the following format:

check_file_format: .c
check_file_format: .h
exclude_dir:folder_b
exclude_dir:folder_a_a
exclude_dir: /path/to/exclude

## Usage

To run the `macro_filter` tool, use the following command:

./macro_filter <dest_dir> <src_dir> <macro_name> <macro_branch> [<config_file>]
- <dest_dir>: Destination directory where the processed files will be saved.
- <src_dir>: Source directory containing the files to be processed.
- <macro_name>: Macro name to be evaluated (e.g., BOARD).
- <macro_branch>: Macro branch to be evaluated (e.g., BOARD_A).
- <config_file>: Path to the configuration file.

## Example
./macro_filter ./example/proj_src example/proj_dst BOARD BOARD_A ./example/proj_src/macro_filter_config.ini

## Limitations
- Single Condition Only: The tool only supports single macro conditions such as #if BOARD == BOARD_A. It does not support compound conditions (e.g., #if BOARD == BOARD_A && FEATURE_ENABLED).
- No Parentheses Support: Conditions with parentheses around the macro name or value (e.g., #if (BOARD) == (BOARD_A)) are not supported.
- Macro Name and Branch Format: The tool assumes the format <macro_name> == <macro_branch> for condition evaluation and does not support variations or complex expressions.


## Error Handling
The tool handles common errors such as file not found, permission denied, and invalid input gracefully. Ensure that the paths provided are correct and accessible.

## Contributing
Contributions are welcome! Please fork the repository and submit pull requests for any enhancements or bug fixes.

## License
This project is licensed under the MIT License - see the LICENSE file for details.
