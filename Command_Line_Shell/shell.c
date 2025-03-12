#include <stdio.h>     // For standard input/output functions

#include <stdlib.h>    // For system functions like exit()

#include <string.h>    // For string manipulation functions like strtok()

#include <unistd.h>    // For system calls like fork(), execvp(), and getpid()

#include <sys/wait.h>  // For wait() to make the parent wait for child process completion



// Define constants for command length and the number of arguments

#define MAX_COMMAND_LENGTH 1024  // Maximum length of a single command

#define MAX_ARGS 100             // Maximum number of arguments a command can have



// Function to read a command from the user

void read_command(char *command) {

    printf("myShell> ");  // Display shell prompt

    if (fgets(command, MAX_COMMAND_LENGTH, stdin) == NULL) {

        perror("Error reading command");  // Handle input error

        exit(1);  // Exit with failure status

    }

    // Remove the trailing newline character added by fgets

    command[strcspn(command, "\n")] = '\0';

}



// Function to parse the command into arguments

void parse_command(char *command, char **args) {

    int i = 0;

    // Use strtok to split the command string by spaces

    args[i] = strtok(command, " ");

    while (args[i] != NULL) {

        i++;

        args[i] = strtok(NULL, " ");

    }

}



// Function to execute the parsed command

void execute_command(char **args) {

    if (args[0] == NULL) return;  // No command entered, just return



    // Handle built-in "exit" command

    if (strcmp(args[0], "exit") == 0) {

        exit(0);  // Terminate the shell

    }



    // Create a new process using fork()

    pid_t pid = fork();

    

    if (pid < 0) {

        // Fork failed

        perror("Fork failed");

    } else if (pid == 0) {

        // Child process: execute the command

        if (execvp(args[0], args) == -1) {

            perror("Command execution failed");  // Handle invalid commands

        }

        exit(EXIT_FAILURE);  // Terminate child process if execvp fails

    } else {

        // Parent process: wait for child to complete

        wait(NULL);

    }

}



int main() {

    // Buffers for command and arguments

    char command[MAX_COMMAND_LENGTH];  

    char *args[MAX_ARGS];



    // Infinite loop to keep the shell running

    while (1) {

        read_command(command);        // Read user input

        parse_command(command, args); // Parse the input into command and arguments

        execute_command(args);        // Execute the command

    }



    return 0;  // Program never actually reaches this point

}