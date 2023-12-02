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

void print_prompt()
{
    printf(BOLD GREEN "muktadir@shell" RESET ":~" BOLD BLUE "$ " RESET);
}

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
void execute_command(char **args)
{

    pid_t pid = fork();
    if (pid == 0)
    {
        execvp(args[0], args);
        printf("Command not found\n");
        exit(0);
    }
    else
    {
        wait(NULL);
    }
}

/**
 * Main
 * @brief The main function of the shell.
 *
 * This function is the entry point of the shell program. It prints a welcome
 * message and then enters an infinite loop where it
 *  1. Prints a prompt
 *  2. Reads a command
 *  3. Parses the command into arguments
 *  4. Executes the command
 *  5. Prints the exit status of the command
 *  6. Repeats
 *  7. Exits when the command "exit" is entered or Ctrl-D is pressed
 *  8. Ctrl+c should terminate the current process, but not the shell
 *  9. "Enter" should do nothing if the command is empty
 *
 *
 */

// Signal handler for SIGINT (Ctrl+C)
// void sigintHandler(int signum)
// {
//     ctrlCPressed = 1;
// }

// void add_to_history(char *command)
// {
//     // open file in append mode
//     FILE *fp = fopen("history", "a");

//     // write command to file
//     fprintf(fp, "%s", command);

//     // close file
//     fclose(fp);
// }

int main(void)
{
    printf("Welcome to Muktadir's Shell\n");

    while (true)
    {
        // print prompt
        print_prompt();

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

        // parse command into arguments
        char *args[MAX_COMMAND_LENGTH];
        parse_command(command, args);

        // ...

        // execute command and measure time taken
        clock_t start = clock();
        execute_command(args);
        clock_t end = clock();

        // calculate time taken in milliseconds
        double time_taken = ((double)(end - start) / CLOCKS_PER_SEC) * 1000;

        // print time taken
    }

    return 0;
}
