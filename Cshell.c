// Development platform: Windows

// Libraries to include
#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdlib.h>

// Global Variables
#define MAX_CHAR 1024
#define MAX_WORDS 30

// this function checks if "exit" is inputted
int is_exit(char *cmd[]) {
    int value;
    if (cmd[0] != NULL) { // check if the input is NULL
        if (strcmp(cmd[0], "exit") == 0) { // if first word is "exit"
            if (cmd[1] == NULL) {   // check if there's any other word after "exit"
                // termination message
                printf("Terminated\n");
                value = 0; // return 0 if there's no words after "exit"
                return value;
            }
            value = 1; // return 1 if there's any arguments after exit
            return value;
        }
        value = 2;
        return value;
    }
    value = 3;
    return value;
}

// this function takes shell input
void input(char str[], char *cmd[]) { 
    cmd[0] = NULL;
    // prints the shell prompt
    printf("$$ Cshell ## ");
    
    // stores the user input line
    fgets(str, MAX_CHAR, stdin);

    // removes the \n from the input line
    char *nl = strchr(str, '\n');
    int index = (int)(nl - str);
    str[index] = '\0';

    // separate the line into tokens using delimiters
    char * cmds;
    cmds = strtok(str, " &"); // delimiters are & and space character
    int i = 0;
    while (cmds != NULL)
    {
        cmd[i] = cmds;
        //printf("%s\n", cmd[i]);
        cmds = strtok(NULL, " &");
        i++;
    }
    cmd[i] = NULL; // sets the last token as NULL in the str array 
    
}

// this function returns 1 if it's a valid timeX syntax
int is_timeX(char *cmd[]) {
    int val;
    if (cmd[0] != NULL) {
        if (strcmp(cmd[0], "timeX") == 0) { // if the first argument is timeX
            // checks if timeX has any other arguments afterwards
            if (cmd[1] == NULL) { 
                val = 1;
                return val;
            }
            else { // it's a valid timeX syntax
                val = 0;
                return val;
            }
        }
    }
    else {
        val = 3;
        return val;
    }
}

// this functions returns 1 if it's a valid pipe operator
int is_pipe(char *cmd[]) {
    if (cmd[0] != NULL) {
        
    }

}

// this function simulates the | (pipe) operator
void pipe_operator(char *cmd[]) {
    int i = 0;
    while (cmd[i] != NULL) {
        if (strcmp(cmd[i], "|") == 0) { // checks if the ith argument is a pipe(|) operator
            if (cmd[i+1] == NULL) {
                // if cmd[i+1] is NULL, then pipe will be UNSUCCESSFUL
            }
            // do a pipe between cmd[i-1] and cmd[i+1]
        }
        i++;
    }
}

// this function handles sigal
void signal_handler(int signum) {
    if (signum == SIGUSR1) {
        // do nothing but acknowledge. yessir
    }
    if (signum == SIGINT) {
        if (fork() == 0) {
            printf("\n$$ Cshell ## ");
            exit(0);
        }
        else {
            wait(NULL);
        }
    }
}

// https://stackoverflow.com/questions/58886885/program-wont-end-after-catching-sigint-before-pressing-enter
// ^ should explain why enter is needed for now

// main function
int main() {
    // initialising an array to store the initial input line
    char str[MAX_CHAR];
    // initialising an array to store pointers to the separated tokens
    char *cmd[MAX_WORDS];
    // this is a copy of cmd without the timeX when timeX will be inputted
    char *cmd2[MAX_WORDS]; 
    // this variable helps keeping track of the exit status
    char *env[] = {"/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin", NULL};
    //int status;
    
    // handles the SIGINT signal
    struct sigaction act;
    act.sa_handler = &signal_handler;
    act.sa_flags = SA_RESTART;
    sigaction(SIGINT, &act, NULL);

    // handles the SIGUSR1 signal
    struct sigaction usr = { 0 };
    usr.sa_flags = SA_RESTART;
    usr.sa_handler = &signal_handler;
    sigaction(SIGUSR1, &usr, NULL);
    
    // an initial input
    input(str, cmd);
    
    // keeps looping following while block until "exit" is entered
    while (is_exit(cmd) != 0 || is_timeX(cmd) == 1) {
        
        //is_timeX(cmd); // checks whether timeX is properly implemented
        // forking a process
        pid_t pid = fork();
        if(pid < 0) {   // if fork fails
            printf("Error creating child process\n");
            exit(-1);
        }
        else if (pid == 0) { // if this is the child process
            // error checking
            int error;
 
            if (is_timeX(cmd) == 0) { // if it's a valid timeX command
                int i = 0;
                int j = 1;
                cmd2[0] = NULL;
                while(cmd[j] != NULL) { // make a copy of the arguments into cmd2 without "timeX"
                    cmd2[i] = cmd[j];
                    i++;
                    j++;
                }
                cmd2[i] = NULL; // set NULL as the last argument afterwards
                // execute the commands after timeX
                error = execvpe(cmd2[0], cmd2, env);
                
            }
            else {
                // executes a process from user input using execvp()
                error = execvpe(cmd[0], cmd, env);
            }
            
            // if the valid program's path is not found
            if (error == -1) {
                if (is_exit(cmd) == 1) { // if exit has arguments afterwards
                    printf("Cshell: \"exit\" with other arguments!!!\n");
                }
                else { // otherwise
                    if (is_timeX(cmd) == 1) {
                        printf("Cshell: \"timeX\" cannot be a standalone command\n");
                    }
                    else {
                        if (is_timeX(cmd) == 0) {
                            printf("Cshell: '%s': No such file or directory\n", cmd[1]);
                        }
                        else {
                            printf("Cshell: '%s': No such file or directory\n", cmd[0]);
                        }
                    }
                }                
            }
            exit(0);
        }
        else {  // this is the parent process
            // send the SIGUSR1 signal to the child process;
            kill(pid, SIGUSR1);

            // this variable helps keeping track of the exit status
            int status;

            // this struct contains resource stats of a process
            struct rusage rusage;
            int ret = wait4(pid, &status, 0, &rusage);
            
            // if it's a valid timeX syntax, then print the stats of the child process
            if (is_timeX(cmd) == 0) {
                printf("(PID)%d  (CMD)%s   (user)%.3f s  (sys)%.3f s\n",
                    pid, cmd[1],
                    rusage.ru_utime.tv_sec + rusage.ru_utime.tv_usec / 1000000.0,
                    rusage.ru_stime.tv_sec + rusage.ru_stime.tv_usec / 1000000.0);
            }
            
            // wait for the child process to finish executing
            wait(&status);
            if (WIFEXITED(status)) {
                int statusCode = WEXITSTATUS(status);

                // if the child process terminates normally
                if (statusCode == 0) {
                    
                } // if the child process termiantes abnormally
                else {
                    //printf("The child process terminates abnormally.\n");
                }
            }
            
        }

        // prompts for input in the while block to keep reprompting until exit is inputted
        input(str, cmd);
    }
        
}
