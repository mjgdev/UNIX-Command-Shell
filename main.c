/* 
 * File:   main.c
 * Author: mjagdev
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

#define  MAX_LINE_LENGTH 100 
#define MAX_ARGUMENTS 10

int main(int argc, char** argv) {

//the variables here 
char inputLine[MAX_LINE_LENGTH];
char *arguments[MAX_ARGUMENTS];
int character;
int index;
int checking;
char *outputFile;
char *inputFile;
char *commandLine;

//While loop since it should be never ending until exit is typed
while (1) {

    // asks and takes the user input
    printf("Your command> ");
    fflush(stdout);

    index = 0;

    // as long as its not newline or EOF
    while ((character = getchar()) != '\n' && character != EOF){
        if (index < MAX_LINE_LENGTH - 1)
            inputLine[index++] = character;
    }

    inputLine[index] = '\0';
    
    //it can be ended using ctrl d
    if(character == EOF){
        printf("\n");
        break;
    }

    // if user just presses enter
    if(index == 0) continue;
    
     if (strcmp(inputLine, "exit") == 0) {
        break;
    }
    
    // check if the command contains a pipe
    if (strchr(inputLine, '|') != NULL) {

        // split around pipe into left and right commands
        char *leftCommand = strtok(inputLine, "|");
        char *rightCommand = strtok(NULL, "|");

        if (rightCommand == NULL) {
            printf("Pipe needed\n");
            continue;
        }

        // parse left command
        char *leftArgs[MAX_ARGUMENTS];
        int leftCount = 0;

        char *word = strtok(leftCommand, " ");

        while (word != NULL && leftCount < MAX_ARGUMENTS - 1) {
            leftArgs[leftCount++] = word;
            word = strtok(NULL, " ");
        }

        leftArgs[leftCount] = NULL;

        // parse right command
        char *rightArgs[MAX_ARGUMENTS];
        int rightCount = 0;

        word = strtok(rightCommand, " ");

        while (word != NULL && rightCount < MAX_ARGUMENTS - 1) {
            rightArgs[rightCount++] = word;
            word = strtok(NULL, " ");
        }

        rightArgs[rightCount] = NULL;

        // make the pipe
        int pipeEnds[2];
        pipe(pipeEnds);

        // first child = left command
        pid_t pid1 = fork();

        if (pid1 == 0) {

            // send left command output into the pipe
            dup2(pipeEnds[1], STDOUT_FILENO);

            close(pipeEnds[0]);
            close(pipeEnds[1]);

            execvp(leftArgs[0], leftArgs);

            fprintf(stderr, "Left command failed\n");
            exit(1);
        }

        // second child = right command
        pid_t pid2 = fork();

        if (pid2 == 0) {

            // get input for right command from the pipe
            dup2(pipeEnds[0], STDIN_FILENO);

            close(pipeEnds[1]);
            close(pipeEnds[0]);

            execvp(rightArgs[0], rightArgs);

            fprintf(stderr, "Right command failed\n");
            exit(1);
        }

        // parent closes both ends of the pipe
        close(pipeEnds[0]);
        close(pipeEnds[1]);

        waitpid(pid1, NULL, 0);
        waitpid(pid2, NULL, 0);

        // pipe command is finished, go back to command prompt
        continue;
    }

    //Splitting around the >
    outputFile = NULL;
    inputFile = NULL;
    commandLine = inputLine;

    if (strchr(inputLine, '>') != NULL) {

        commandLine = strtok(inputLine, ">");
        outputFile = strtok(NULL, " ");

    } else if (strchr(inputLine, '<') != NULL) {

        commandLine = strtok(inputLine, "<");
        inputFile = strtok(NULL, " ");
    }

    //checking is false meaning there is no &
    checking = 0;

    //if the inputLine has a & then & makes the parent not wait
    if(index > 0 && inputLine[index -1] == '&'){
        //then the checking is true
        checking = 1;
        inputLine[index -1] = '\0';
    }
    
    //splitting it into words by the space
    int argCount = 0;

    //this part takes the string and breaks it up based on where the " " is
    char *word = strtok(commandLine, " ");

    while (word != NULL && argCount < MAX_ARGUMENTS - 1) {
        arguments[argCount++] = word;
        word = strtok(NULL, " ");
    }

    arguments[argCount] = NULL;

    // if nothing valid typed
    if(arguments[0] == NULL) continue;
    
    //implementing the cd
    if (strcmp(arguments[0], "cd") == 0) {
    if (arguments[1] == NULL) {
        chdir(getenv("HOME"));
    } else {
        chdir(arguments[1]);
    }
    continue;
}
    // Fork makes a copy of the program and lets both the copies run at the same time
    pid_t pid = fork();

    if (pid == 0) {

        if (outputFile != NULL) {
            int file = open(outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            dup2(file, STDOUT_FILENO);
            close(file);
        }

        if (inputFile != NULL) {
            int file = open(inputFile, O_RDONLY);
            dup2(file, STDIN_FILENO);
            close(file);
        }

        // running the code in the child. this part is what makes ls etc commands work
        //the line below searches in the path and gets the path information like ls, the ./dist etc
        execvp(arguments[0], arguments);
        fprintf(stderr, "Command failed\n");
        exit(1);
    }

    if (!checking) {
        // fixing the sleep function
        waitpid(pid, NULL, 0);

    }
}

return (EXIT_SUCCESS);

}

