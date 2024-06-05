# 宏过滤器
macro_filter 是一个命令行工具，用于根据指定的宏条件处理源文件。它可以过滤包含在宏指令中的代码段，并在考虑指定配置的情况下复制文件或目录。
 
## 特性
- 根据宏条件过滤和处理源文件 (#if, #else if, #elif, #else, #endif)。
- 复制整个文件和目录。
- 支持自定义文件类型和排除目录。

## 配置
创建一个配置文件（例如，macro_filter_config.ini），以指定要处理的文件类型和要排除的目录。配置文件应具有以下格式：

check_file_format: .c
check_file_format: .h
exclude_dir: folder_b
exclude_dir: folder_a_a
exclude_dir: /path/to/exclude

## 使用方法
运行 macro_filter 工具，请使用以下命令：

./macro_filter <dest_dir> <src_dir> <macro_name> <macro_branch> [<config_file>]
<dest_dir>：保存处理后文件的目标目录。
<src_dir>：包含要处理文件的源目录。
<macro_name>：要评估的宏名称（例如，BOARD）。
<macro_branch>：要评估的宏分支（例如，BOARD_A）。
<config_file>：配置文件的路径。非必须，没有设置时，默认检测文件格式为.c和.h。

## 示例

./macro_filter ./example/proj_src example/proj_dst BOARD BOARD_A ./example/proj_src/macro_filter_config.ini

## 限制
- 仅支持单一条件：该工具仅支持单一宏条件，如 #if BOARD == BOARD_A。它不支持复合条件（例如，#if BOARD == BOARD_A && FEATURE_ENABLED）。
- 不支持括号：不支持宏名称或值带有括号的条件（例如，#if (BOARD) == (BOARD_A)）。
- 宏名称和分支格式：该工具假定条件评估的格式为 <macro_name> == <macro_branch>，不支持变体或复杂表达式。

## 错误处理
该工具能处理常见错误，如文件未找到、权限被拒绝和无效输入。请确保提供的路径是正确且可访问的。

## 贡献
欢迎贡献！请 fork 本仓库，并提交 pull requests 以进行任何增强或修复 bug。