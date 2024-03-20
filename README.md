# LeanShell

LeanShell is a lightweight, minimalistic command-line interface (CLI) application written in C++ that provides a basic shell environment for executing built-in commands and external commands available in the system's PATH. It is inspired by Stephen Brennan's lsh (https://brennan.io/2015/01/16/write-a-shell-in-c/).

## Features

- **Built-in Commands**
 - `cd`: Change the current working directory.
 - `ls`: List files and directories in the specified path.
 - `touch`: Create new empty files.
 - `cp`: Copy a file from source to destination.
 - `mv`: Move a file or directory from source to destination.
 - `hs`: Show command history.
 - `clrhs`: Clear command history.
 - `exit`: Exit the shell.
 - `help`: Display a help message with command descriptions.
 - `echo`: Print the arguments to the console.
 - `cat`: Print the contents of files to the console.
 - `df`: Show available disk space.
 - `clear`: Clear the console screen.

- **External Commands**: Ability to run any built-in Unix commands that exist in the system's PATH.
- **Command History**: Maintains a history of executed commands, which can be displayed or cleared.
- **Colored Output**: The `ls` command displays directories in a different color for better visibility.

## Usage

1. Clone the repository or download the source code.
2. Compile the source code using a C++ compiler. For example, with GCC:

```
g++ main.cpp -o leanshell
```

3. Run the compiled executable:

```
./leanshell
```
4. The shell prompt (`>`) will appear, indicating that you can enter commands.
5. Type a command and press Enter to execute it. For example:

```
> ls
> cd /path/to/directory
> touch file.txt
> cp file.txt file_copy.txt
> mv file_copy.txt new_file.txt
> hs
> help
> exit
```

6. To exit the shell, use the `exit` command or press `Ctrl+C`.

## Future Improvements

- Support for redirection (`>`, `<`) and piping (`|`) operators.
- Handling of multiple spaces, quotes, and other special characters in input.
- Tab completion for commands and file paths.
- Persistent command history across sessions.
- Improved error handling and error messages.
- Support for environment variables and shell scripting.
