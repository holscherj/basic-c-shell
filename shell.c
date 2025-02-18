#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define TOK_DELIM " \t\r\n"
#define RED "\033[0;31m"
#define RESET "\e[0m"

char *read_line();
char **split_line(char *);
int dash_exit(char **);
int dash_execute(char **);

/**
 * Program main entry point
 */
int main() {
    loop();
    return 0;
}

/**
 * The driving function of the terminal loop
 * Accepts input from the user in order to execute some command
 */
void loop() {
    char *line;
    char **args;
    int status = 1;

    do {
        // get user input
        printf("> ");
        line = read_line();
        args = split_lines(line);
        status = dash_execute(args);

        // free memory used by pointers
        free(line);
        free(args);
    } while (status);
}

/**
 * Reading the user's command
 * Dynamically allocated memory for the user's command to ensure
 * that we have enough space for any command entered
 */
char *read_line() {
    // allocate space for the buffer
    int buffsize = 1024;
    int position = 0;
    char *buffer = malloc(sizeof(char) *buffsize);
    int c;

    // ensure the buffer is allocated successfully
    if(!buffer) {
        fprintf(stderr, "%sdash: Allocation error%s\n", RED, RESET);
        exit(EXIT_FAILURE);
    }

    // loop over the characters entered by the user and append to the buffer
    while (1) {
        c = getchar();
        // terminate buffer if end of file or newline is entered
        if(c == EOF || c == "\n") {
            buffer[position] = '\0';
            return buffer;
        } else {
            buffer[position] = c;
        }
        position++;

        // resize the buffer if needed
        if(position >= buffsize) {
            buffsize += 1024;
            buffer = realloc(buffer, buffsize);

            // ensure buffer is successfully reallocated
            if(!buffer) {
                fprintf(stderr, "%sdash: Allocation error%s\n", RED, RESET);
                exit(EXIT_FAILURE);
            }
        }
    }
}

/**
 * Tokenizing the user input
 * This makes it easier to execute the user's command
 */
char **split_line(char *line) {
    // allocate memory for token buffer
    int buffsize = 1024, position = 0;
    char **tokens = malloc(sizeof(char) *buffsize);
    char *token;

    // ensure the buffer is successfully allocated
    if(!tokens) {
        fprintf(stderr, "%sdash: Allocation error%s\n", RED, RESET);
        exit(EXIT_FAILURE);
    }
    
    // process tokens
    token = strtok(line, TOK_DELIM);
    while(token != NULL) {
        tokens[position] = token;
        position++;

        // resize the buffer if needed
        if(position >= buffsize) {
            buffsize += 1024;
            tokens = realloc(tokens, buffsize * sizeof(char *));

            // ensure buffer is successfully reallocated
            if(!tokens) {
                fprintf(stderr, "%sdash: Allocation error%s\n", RED, RESET);
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, TOK_DELIM);
    }

    tokens[position] = NULL;
    return tokens;
}

/**
 * Simple function for exiting the shell
 */
int dash_exit(char **args) {
    return 0;
}

/**
 * A function to execute the user's commands
 */
int dash_execute(char **args) {
    pid_t cpid;
    int status;

    if(strcmp(args[0], "exit") == 0) {
        return dash_exit(args);
    }

    cpid = fork();

    if(cpid == 0) {
        if(execvp(args[0], args) < 0) {
            printf("dash: command not found: %s\n", args[0]);
            exit(EXIT_FAILURE);
        }
    } else if(cpid < 0) {
        printf(RED "Error forking" RESET "\n");
    } else {
        waitpid(cpid, &status, WUNTRACED);
    }
    return 1;
}