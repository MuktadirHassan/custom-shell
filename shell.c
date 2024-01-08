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
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

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

// Function declarations
void listFiles();
void current_directory();

// Global variable to track if Ctrl+C was pressed
volatile sig_atomic_t ctrlCPressed = 0;

// Signal handler for SIGINT (Ctrl+C)
void sigintHandler(int signum)
{
    ctrlCPressed = 1;
}

void clear_screen()
{
    const char *CLEAR_SCREEN_ANSI = "\e[1;1H\e[2J";
    write(STDOUT_FILENO, CLEAR_SCREEN_ANSI, 12);
}

// print prompt
void print_prompt()
{
    static int first_time = 1;
    char cwd[1024];
    if (first_time)
    {
        // clear screen
        clear_screen();
        first_time = 0;
        printf(CYAN BOLD UNDERLINE "Welcome to Muktadir's Shell\n" RESET);
        printf(BOLD "Type \"exit\" to exit the shell\n" RESET);
        printf(BOLD "Type \"clear\" to clear the screen\n" RESET);
        printf(BOLD "Type \"ls\" to list files in the current directory\n" RESET);
        printf(BOLD "Type \"pwd\" to print the current directory\n" RESET);
        printf(BOLD "Type \"cd <directory>\" to change the current directory\n" RESET);
        printf(BOLD "Type \"<command> &\" to run the command in the background\n" RESET);
        printf(BOLD "Type \"<command> < <input_file>\" to redirect input from a file\n" RESET);
        printf(BOLD "Type \"<command> > <output_file>\" to redirect output to a file\n" RESET);
    }

    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        perror("getcwd() error");
        return;
    }

    printf("\033[1;32m");
    printf("muktadir");
    printf("\033[0m");
    printf("👌");
    printf(CYAN "%s", cwd);
    printf("\033[0m");
    printf("$ ");
    fflush(stdout);
}

// signal handler for SIGCHLD
void sigchldHandler(int signum)
{
    int status;
    while (waitpid(-1, &status, WNOHANG) > 0)
        ;
}

// parse command into arguments
void parse_command(char *command, char **args, bool *background)
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

    // check if command should be run in the background
    if (i > 0 && strcmp(args[i - 1], "&") == 0)
    {
        *background = true;
        args[i - 1] = NULL; // remove & from arguments
    }
}

// first argument is the command
// rest are options such as -l, -a, -r
// execute command and measure time taken
void execute_command(char **args, bool background)
{
    printf("\n\n");
    printf(BOLD CYAN "Command " RESET);
    for (int i = 0; args[i] != NULL; i++)
    {
        printf("%s", args[i]);
    }
    printf("\n\n");

    if (strcmp(args[0], "cd") == 0)
    {
        if (args[1] == NULL)
        {
            printf("cd: expected argument to \"cd\"\n");
        }
        else
        {
            if (chdir(args[1]) != 0)
            {
                perror("chdir() error");
            }
        }
        return;
    }

    // check for redirection
    int in = 0, out = 0; // file descriptors for input and output files
    char *input_file = NULL, *output_file = NULL;
    for (int i = 0; args[i] != NULL; i++)
    {
        if (strcmp(args[i], "<") == 0)
        {
            input_file = args[i + 1];
            args[i] = NULL;
        }
        else if (strcmp(args[i], ">") == 0)
        {
            output_file = args[i + 1];
            args[i] = NULL;
        }
    }

    // fork child process
    pid_t pid = fork();

    // execute command and measure time taken
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    if (pid == 0)
    {

        // handle input redirection
        if (input_file != NULL)
        {
            in = open(input_file, O_RDONLY);
            if (in == -1)
            {
                perror("open() error");
                exit(EXIT_FAILURE);
            }
            dup2(in, STDIN_FILENO);
            close(in);
        }

        // handle output redirection
        if (output_file != NULL)
        {
            out = open(output_file, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
            if (out == -1)
            {
                perror("open() error");
                exit(EXIT_FAILURE);
            }
            dup2(out, STDOUT_FILENO);
            close(out);
        }

        // execute command
        execvp(args[0], args);

        // exit child process
        perror("execvp() error");
        exit(EXIT_FAILURE);
    }
    else if (pid > 0)
    {

        // check if command should be run in the background
        if (background)
        {
            printf("Process running in background\n");
            return;
        }

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

int main(void)
{

    // register signal handler for SIGINT (Ctrl+C)
    signal(SIGINT, sigintHandler);
    // register signal handler for SIGCHLD
    signal(SIGCHLD, sigchldHandler);

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
            printf("Exiting shell...\n" RESET);
            break;
        }

        if (strcmp(command, "clear\n") == 0)
        {
            clear_screen();
            continue;
        }

        if (strcmp(command, "\n") == 0)
        {
            continue;
        }

        if (strcmp(command, "ls\n") == 0)
        {
            listFiles();
            continue;
        }

        if (strcmp(command, "pwd\n") == 0)
        {
            current_directory();
            continue;
        }

        // parse command into arguments
        char *args[MAX_COMMAND_LENGTH];
        bool background = false;
        parse_command(command, args, &background);

        // Reset Ctrl+C flag
        ctrlCPressed = 0;

        // execute command
        execute_command(args, background);

        // Check if Ctrl+C was pressed
        if (ctrlCPressed)
        {
            printf(BOLD RED "Process terminated by Ctrl+C\n" RESET);
        }
    }

    return 0;
}

// implementation of ls command
void listFiles()
{
    // open current directory
    DIR *dir = opendir(".");

    // read directory
    struct dirent *entry;
    printf("%-10s %-10s %-10s %-10s %-20s %-20s %-20s\n", "Permissions", "User", "Group", "Size", "Modified", "Accessed", "File Name");
    while ((entry = readdir(dir)) != NULL)
    {
        // skip hidden files
        if (entry->d_name[0] == '.')
        {
            continue;
        }

        struct stat file_stat;
        char access_time[20], modification_time[20], change_time[20];

        // get file details
        stat(entry->d_name, &file_stat);
        strftime(access_time, sizeof(access_time), "%b %d %Y %H:%M:%S", localtime(&file_stat.st_atime));
        strftime(modification_time, sizeof(modification_time), "%b %d %Y %H:%M:%S", localtime(&file_stat.st_mtime));
        strftime(change_time, sizeof(change_time), "%b %d %Y %H:%M:%S", localtime(&file_stat.st_ctime));

        // get file permissions
        char permissions[11];
        permissions[0] = (S_ISDIR(file_stat.st_mode)) ? 'd' : '-';
        permissions[1] = (file_stat.st_mode & S_IRUSR) ? 'r' : '-';
        permissions[2] = (file_stat.st_mode & S_IWUSR) ? 'w' : '-';
        permissions[3] = (file_stat.st_mode & S_IXUSR) ? 'x' : '-';
        permissions[4] = (file_stat.st_mode & S_IRGRP) ? 'r' : '-';
        permissions[5] = (file_stat.st_mode & S_IWGRP) ? 'w' : '-';
        permissions[6] = (file_stat.st_mode & S_IXGRP) ? 'x' : '-';
        permissions[7] = (file_stat.st_mode & S_IROTH) ? 'r' : '-';
        permissions[8] = (file_stat.st_mode & S_IWOTH) ? 'w' : '-';
        permissions[9] = (file_stat.st_mode & S_IXOTH) ? 'x' : '-';
        permissions[10] = '\0';

        // get file extension
        char *extension = strrchr(entry->d_name, '.');
        char *color_code = "";
        if (extension != NULL)
        {
            if (strcmp(extension, ".c") == 0 || strcmp(extension, ".cpp") == 0 || strcmp(extension, ".java") == 0 || strcmp(extension, ".py") == 0 || strcmp(extension, ".js") == 0 || strcmp(extension, ".php") == 0 || strcmp(extension, ".html") == 0 || strcmp(extension, ".css") == 0 || strcmp(extension, ".sh") == 0)
            {
                color_code = "\033[0;32m"; // green
            }
            else if (strcmp(extension, ".h") == 0 || strcmp(extension, ".txt") == 0 || strcmp(extension, ".md") == 0 || strcmp(extension, ".json") == 0 || strcmp(extension, ".audio") == 0 || strcmp(extension, ".video") == 0)
            {
                color_code = "\033[0;34m"; // blue
            }
            // Add more file extensions and color codes as needed
        }

        // print file details in tabular format
        printf("%-10s %-10d %-10d %-10ld %-20s %-20s %s%s%s\n", permissions, file_stat.st_uid, file_stat.st_gid, file_stat.st_size, modification_time, access_time, color_code, entry->d_name, "\033[0m");
    }

    // close directory
    closedir(dir);
}

void current_directory()
{
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        perror("getcwd() error");
        return;
    }
    printf("Current Directory: %s\n", cwd);
}
