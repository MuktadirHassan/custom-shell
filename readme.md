# A custom shell (linux based)
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


 ```mermaid
 graph TD
    A[Start Shell] --> B{Read Input}
    B -->|Command Entered| C[Parse Input]
    C --> D{Is it a Built-in Command?}
    D -->|Yes| E[Execute Built-in Command]
    D -->|No| F[Execute System Command]
    E --> G[Output Result]
    F --> G
    G --> H{Any Errors?}
    H -->|Yes| I[Display Error]
    H -->|No| B
    I --> B
 ```