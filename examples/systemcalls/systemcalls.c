#include "systemcalls.h"
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

/**
 * @param cmd the command to execute with system()
 * @return true if the command in @param cmd was executed
 *   successfully using the system() call, false if an error occurred,
 *   either in invocation of the system() call, or if a non-zero return
 *   value was returned by the command issued in @param cmd.
*/
bool do_system(const char *cmd)
{

/*
 * TODO  add your code here
 *  Call the system() function with the command set in the cmd
 *   and return a boolean true if the system() call completed with success
 *   or false() if it returned a failure
*/

    int ret = system(cmd); // execute the command using system()
    if (ret == 0) {
        printf("Command completed successfully\n");
        return true;

    } else {
        printf("Command failed\n");
        return false;

    }
    
}

/**
* @param count -The numbers of variables passed to the function. The variables are command to execute.
*   followed by arguments to pass to the command
*   Since exec() does not perform path expansion, the command to execute needs
*   to be an absolute path.
* @param ... - A list of 1 or more arguments after the @param count argument.
*   The first is always the full path to the command to execute with execv()
*   The remaining arguments are a list of arguments to pass to the command in execv()
* @return true if the command @param ... with arguments @param arguments were executed successfully
*   using the execv() call, false if an error occurred, either in invocation of the
*   fork, waitpid, or execv() command, or if a non-zero return value was returned
*   by the command issued in @param arguments with the specified arguments.
*/

bool do_exec(int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
    // this line is to avoid a compile warning before your implementation is complete
    // and may be removed
    command[count] = command[count];

/*
 * TODO:
 *   Execute a system command by calling fork, execv(),
 *   and wait instead of system (see LSP page 161).
 *   Use the command[0] as the full path to the command to execute
 *   (first argument to execv), and use the remaining arguments
 *   as second argument to the execv() command.
 *
*/
   pid_t pid = fork(); // create a new process
    if (pid == -1) {
      perror("fork"); // error log
      exit(EXIT_FAILURE);
    } 
    else if (pid == 0) {
       // child process
      int result = execv(command[0], command); // execv
      if (result == -1) {
        perror("execv"); // error log
        exit(EXIT_FAILURE);
      }
    }

    va_end(args);  
     
    // parent process
    int status;
    wait(&status); // wait for child process to complete
    if (WIFEXITED(status)) {
        // child process completed with exit status
        int exit_status = WEXITSTATUS(status);
        return (exit_status == 0); // return true if exit status is 0, false otherwise
    } else if (WIFSIGNALED(status)) {
        // child process terminated by signal
        fprintf(stderr, "Child process terminated by signal %d\n", WTERMSIG(status));
        return false;
    } else {
       // unknown error
        fprintf(stderr, "Unknown error occurred\n");
        return false;
    }
    


}

/**
* @param outputfile - The full path to the file to write with command output.
*   This file will be closed at completion of the function call.
* All other parameters, see do_exec above
*/
bool do_exec_redirect(const char *outputfile, int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
    // this line is to avoid a compile warning before your implementation is complete
    // and may be removed
    command[count] = command[count];


/*
 * TODO
 *   Call execv, but first using https://stackoverflow.com/a/13784315/1446624 as a refernce,
 *   redirect standard out to a file specified by outputfile.
 *   The rest of the behaviour is same as do_exec()
 *
*/
    int pid = fork();
    if (pid == 0) { // child process
        int fd = open(outputfile, O_CREAT | O_TRUNC | O_WRONLY, 0666);
        dup2(fd, STDOUT_FILENO); // redirect stdout to file
        close(fd);
        execv(command[0], command);
        exit(EXIT_FAILURE);
    } else if (pid > 0) { // parent process
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            return true;
        } else {
            return false;
        }
    } else { // error
        return false;
    }
    va_end(args);
}
