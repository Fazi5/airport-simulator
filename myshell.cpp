#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>
#include <fcntl.h>
#include <signal.h>

using namespace std;

/**
 * @brief Signal handler for the shell.
 * Ensures the shell doesn't terminate on Ctrl+C (SIGINT).
 * Instead, it just prints a newline to keep the prompt clean.
 */
void sigint_handler(int sig) {
    cout << "\n[Shell] Interrupt received. Type 'quit' to exit safely." << endl;
    cout << "myshell> " << flush;
}

/**
 * @brief Helper function to tokenize a string based on a delimiter.
 * Handles multiple spaces, tabs, and trims each token.
 * 
 * @param input The raw input string from the user.
 * @param delimiter The character to split on (space or pipe).
 * @return vector<string> A list of cleaned tokens.
 */
vector<string> tokenize(string input, char delimiter) {
    vector<string> tokens;
    string token;
    istringstream tokenStream(input);
    while (getline(tokenStream, token, delimiter)) {
        // Trim leading and trailing spaces
        size_t first = token.find_first_not_of(" ");
        if (first != string::npos) {
            size_t last = token.find_last_not_of(" ");
            tokens.push_back(token.substr(first, last - first + 1));
        }
    }
    return tokens;
}

/**
 * @brief Executes a single command.
 * @param args The command and its arguments.
 */
void execute_single_command(vector<string> args) {
    // Convert vector<string> to char* array for execvp
    vector<char*> c_args;
    for (const auto& arg : args) {
        c_args.push_back(const_cast<char*>(arg.c_str()));
    }
    c_args.push_back(nullptr);

    /**
     * fork() system call:
     * Creates a new process (child) by duplicating the calling process (parent).
     * Returns 0 to the child and child's PID to the parent.
     */
    pid_t pid = fork();

    if (pid == 0) {
        // Child process
        /**
         * execvp() system call:
         * Replaces the current process image with a new process image.
         * It searches for the executable in the PATH environment variable.
         */
        if (execvp(c_args[0], c_args.data()) == -1) {
            perror("myshell: execution error");
            exit(EXIT_FAILURE);
        }
    } else if (pid < 0) {
        // Error forking
        perror("myshell: fork error");
    } else {
        // Parent process
        /**
         * wait() system call:
         * Suspends execution of the parent process until one of its children terminates.
         */
        int status;
        waitpid(pid, &status, 0);
    }
}

/**
 * @brief Handles multi-level pipelined commands.
 * Examples: ls | wc, ls | sort | wc
 */
void execute_piped_commands(vector<string> commands) {
    int num_commands = commands.size();
    int pipes[num_commands - 1][2];

    // Create all necessary pipes
    for (int i = 0; i < num_commands - 1; i++) {
        /**
         * pipe() system call:
         * Creates a unidirectional data channel.
         * pipes[i][0] is the read end, pipes[i][1] is the write end.
         */
        if (pipe(pipes[i]) == -1) {
            perror("myshell: pipe error");
            return;
        }
    }

    vector<pid_t> pids;
    for (int i = 0; i < num_commands; i++) {
        pid_t pid = fork();

        if (pid == 0) {
            // Child process

            // If not the first command, redirect stdin to previous pipe's read end
            if (i > 0) {
                /**
                 * dup2() system call:
                 * Duplicates a file descriptor, making the new descriptor point to the same file.
                 * Redirects stdin (0) to pipes[i-1][0].
                 */
                dup2(pipes[i - 1][0], STDIN_FILENO);
            }

            // If not the last command, redirect stdout to current pipe's write end
            if (i < num_commands - 1) {
                /**
                 * Redirects stdout (1) to pipes[i][1].
                 */
                dup2(pipes[i][1], STDOUT_FILENO);
            }

            // Close all pipe file descriptors in the child
            for (int j = 0; j < num_commands - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            // Execute the command
            vector<string> args = tokenize(commands[i], ' ');
            vector<char*> c_args;
            for (const auto& arg : args) {
                c_args.push_back(const_cast<char*>(arg.c_str()));
            }
            c_args.push_back(nullptr);

            if (execvp(c_args[0], c_args.data()) == -1) {
                perror("myshell: exec error");
                exit(EXIT_FAILURE);
            }
        } else if (pid < 0) {
            perror("myshell: fork error");
            return;
        }
        pids.push_back(pid);
    }

    // Parent closes all pipe file descriptors
    for (int i = 0; i < num_commands - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    // Parent waits for all child processes to finish
    for (pid_t pid : pids) {
        int status;
        waitpid(pid, &status, 0);
    }
}

/**
 * @brief Main entry point for the Custom Shell.
 * Initializes signal handlers and maintains the interactive command loop.
 * 
 * @return int Exit status.
 */
int main() {
    // Set up signal handling for Ctrl+C
    signal(SIGINT, sigint_handler);

    string input;
    const string prompt = "myshell> ";

    cout << "====================================================" << endl;
    cout << "   CUSTOM POSIX SHELL - ADVANCED IPC DEMO          " << endl;
    cout << "====================================================" << endl;
    cout << "Features: Pipelining, Process Isolation, Signal Handling" << endl;

    while (true) {
        cout << prompt;
        if (!getline(cin, input)) {
            cout << endl; // Handle EOF (Ctrl+D)
            break;
        }

        if (input == "quit") {
            cout << "Exiting myshell..." << endl;
            break;
        }

        if (input.empty()) continue;

        // Check for pipes
        if (input.find('|') != string::npos) {
            vector<string> piped_cmds = tokenize(input, '|');
            execute_piped_commands(piped_cmds);
        } else {
            vector<string> args = tokenize(input, ' ');
            if (args.empty()) continue;
            execute_single_command(args);
        }
    }

    return 0;
}
