# Custom Shell in C++

A robust shell-like program that emulates the functionality of a command-line interface, demonstrating core OS concepts like process creation, process control, and inter-process communication (IPC) via pipelining.

## Features
- **Standalone Commands**: Run commands like `ls`, `date`, `pwd`.
- **Commands with Arguments**: Run commands like `ls -l /tmp`, `grep "pattern" file.txt`.
- **Multi-level Pipelining**: Supports single and multi-level pipes (e.g., `ls -l | sort | wc -l`).
- **Persistent Interface**: Displays a `myshell> ` prompt until the user types `quit`.
- **Graceful Termination**: Ensures all resources are released and children are waited upon before exiting.

## System Calls Used
- `fork()`: Creates child processes for command execution.
- `execvp()`: Replaces child process memory with the target executable.
- `waitpid()`: Synchronizes parent and child processes.
- `pipe()`: Establishes unidirectional IPC channels for data transfer.
- `dup2()`: Redirects standard input/output streams for pipelining.

## How to Compile and Run

> [!IMPORTANT]
> This program requires a **POSIX environment** (Linux, macOS, or WSL on Windows).

1.  Open your terminal.
2.  Navigate to the project directory.
3.  Compile the source code:
    ```bash
    g++ -o myshell myshell.cpp
    ```
4.  Run the shell:
    ```bash
    ./myshell
    ```

## Project Structure
- `myshell.cpp`: The main source code with detailed internal documentation.
- `report.md`: A technical report on `dup()` and `dup2()` with diagrams.
- `task.md`: Development task list (internal).
- `implementation_plan.md`: Detailed plan used during development (internal).
