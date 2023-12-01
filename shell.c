// Custom Shell
// Author: Muktadir Hassan

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

void print_prompt()
{
    printf("msh> ");
}

void read_command(char *command)
{
    fgets(command, 100, stdin);
}

/**
 * @brief Parses a command string into an array of arguments.
 *
 * This function takes a command string and splits it into individual arguments
 * based on the space delimiter. The arguments are stored in the provided array.
 * The last element of the array is set to NULL to indicate the end of arguments.
 *
 * @param command The command string to be parsed.
 * @param args The array to store the parsed arguments.
 */
void parse_command(char *command, char **args)
{
    char *token = strtok(command, " ");
    int i = 0;
    while (token != NULL)
    {
        args[i++] = token;
        token = strtok(NULL, " ");
    }
    args[i] = NULL;
}

/**
 * Main
 */
int main(void)
{
    printf("Welcome to Muktadir's Shell\n");
    return 0;
}
