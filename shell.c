// Custom Shell
// Author: Muktadir Hassan

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>
#include <time.h>

#define MAX_COMMAND_LENGTH 100
#define MAX_HISTORY_SIZE 10

// ANSI color codes
#define RED "\x1B[31m"
#define GREEN "\x1B[32m"
#define BLUE "\x1B[34m"
#define YELLOW "\x1B[33m"
#define MAGENTA "\x1B[35m"
#define CYAN "\x1B[36m"
#define WHITE "\x1B[37m"
#define RESET "\x1B[0m"
#define BOLD "\x1B[1m"
#define UNDERLINE "\x1B[4m"
#define BLINK "\x1B[5m"
#define REVERSE "\x1B[7m"
#define HIDDEN "\x1B[8m"
#define CLEAR "\033[H\033[J"
#define HOME "\033[H"
#define UP "\033[1A"
#define DOWN "\033[1B"
#define RIGHT "\033[1C"
#define LEFT "\033[1D"

// Global variable to track if Ctrl+C was pressed
volatile sig_atomic_t ctrlCPressed = 0;

// Signal handler for SIGINT (Ctrl+C)
void sigintHandler(int signum)
{
    ctrlCPressed = 1;
}

// print prompt
void print_prompt()
{
    printf(BOLD GREEN "myshell@shell" RESET ":~" BOLD BLUE "$ " RESET);
}

// parse command into arguments
void parse_command(char *command, char **args)
{
    // remove newline character
    command[strlen(command) - 1] = '\0';

    // split command into arguments
    char *token = strtok(command, " ");
    int i = 0;
    while (token != NULL)
    {
        args[i++] = token;
        token = strtok(NULL, " ");
    }
    args[i] = NULL;
}

// first argument is the command
// rest are options such as -l, -a, -r
// execute command and measure time taken
void execute_command(char **args)
{
    pid_t pid = fork();
    // execute command and measure time taken
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    if (pid == 0)
    {
        execvp(args[0], args);

        // exit child process
        perror("execvp() error");
        exit(EXIT_FAILURE);
    }
    else if (pid > 0)
    {
        int status;
        waitpid(pid, &status, 0);
        // calculate time taken in milliseconds
        clock_gettime(CLOCK_MONOTONIC, &end);
        double time_taken = (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_nsec - start.tv_nsec) / 1000000.0;

        // print time taken with color
        printf(BOLD YELLOW "Time taken: %f ms\n" RESET, time_taken);

        // check if command executed successfully
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
        {
            printf(BOLD GREEN "Command executed successfully\n" RESET);
        }
        else
        {
            printf(BOLD RED "Command execution failed\n" RESET);
        }
    }
    else
    {
        // fork failed
        perror("fork() error");
        exit(EXIT_FAILURE);
    }
}

// add command to history
void add_to_history(char *command, char **history, int *history_count)
{
    if (*history_count < MAX_HISTORY_SIZE)
    {
        history[*history_count] = strdup(command);
        (*history_count)++;
    }
    else
    {
        free(history[0]);
        for (int i = 0; i < MAX_HISTORY_SIZE - 1; i++)
        {
            history[i] = history[i + 1];
        }
        history[MAX_HISTORY_SIZE - 1] = strdup(command);
    }
}

// print command history
void print_history(char **history, int history_count)
{
    printf(BOLD "Command History:\n" RESET);
    for (int i = 0; i < history_count; i++)
    {
        printf("%d. %s\n", i + 1, history[i]);
    }
}

int main(void)
{
    printf("Welcome to Muktadir's Shell\n");

    char *history[MAX_HISTORY_SIZE];
    int history_count = 0;
    int current_history_index = 0;

    while (true)
    {
        // print prompt
        print_prompt();

        // register signal handler for SIGINT (Ctrl+C)
        signal(SIGINT, sigintHandler);

        // read and store command
        char command[MAX_COMMAND_LENGTH];
        fgets(command, MAX_COMMAND_LENGTH, stdin);

        // Exit if "exit" is entered
        if (strcmp(command, "exit\n") == 0)
        {
            break;
        }

        // Ignore if command is empty
        if (strcmp(command, "\n") == 0)
        {
            continue;
        }

        // Add command to history
        add_to_history(command, history, &history_count);

        // parse command into arguments
        char *args[MAX_COMMAND_LENGTH];
        parse_command(command, args);

        // ...

        // Reset Ctrl+C flag
        ctrlCPressed = 0;

        // execute command
        execute_command(args);

        // Check if Ctrl+C was pressed
        if (ctrlCPressed)
        {
            printf(BOLD RED "Process terminated by Ctrl+C\n" RESET);
        }
    }

    // Free history memory
    for (int i = 0; i < history_count; i++)
    {
        free(history[i]);
    }

    return 0;
}
