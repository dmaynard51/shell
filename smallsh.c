/*
Name: Daniel Maynard
Assigment: Smallsh
Date: 5/21/19
Description: This program will take a userinput first, and then take take each
string separated by a space as an command/argument. It will then go through
each string, and flag them depending on if its an exit, cd, status,comment call.
For each command, it will fork and execlp to make sure its a new process
for each new command. If the command is a special command (status, cd, exit)
it wont fork and execlp. Also the program should check for CTRL_Shift_Z, if 
it is called, then we enter foreground mode, and background calls will be 
ignored. Finally if exit is called, all background tasks should be killed with
a signal kill of 12.

*/

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <dirent.h>
#include <signal.h>


int childExitMethod = -5;
char input[2048];
int exitCall = 0;
int ten = 10;
int temp = 0;
char *pch;
//holds arguments
char *args[512];
int i;
int argNumber = 0;
//echo
int echoCall = 0;
//length 2048
char echoString[2048];
//status
int statusCall = 0;
int test;
//linux command
int linuxCommand;

char commandName[2048];

//background task
int backgroundTask;
//notes
int notes;
//comments
char noteString[2048];

//change directory
int cd;
char cdMove[2048];
char mainDir[2048];

//standard out
int stdOut;
char stdOutFile[2048];

//standard in
int stdIn;
char stdInFile[2048];

//hold unfinished processes
pid_t unfinishedProcesses[512];
int pidIncrementor = 0;



//background task
int bg;
//start at 0
int fg = 0;
int sleeping;
char sleepArg[2048];


void forknewprocess()
{
    pid_t spawnpid = -5;

    

    
    spawnpid = fork();
    switch (spawnpid)
    {
        //error
        case -1:
            perror("Hull Breach!");
            exit(1);
            break;
        //child    
        case 0:
            //if stdout is called only
            if (stdOut == 1 && stdIn == 0)
            {
                //designate the file to be open and written to
                int targetFD = open(stdOutFile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (targetFD == -1) 
                {
                    perror("open()"); exit(1); 
                }
                
                //redirect output
                int result = dup2(targetFD, 1);
                
                if (result == -1) 
                {
                    perror("dup2"); exit(2); 
                }
                              
                
                test = execlp(args[0], args[0], NULL);
                exit(1);
            }
            
            
            //if stdin is called only
            if (stdIn == 1 && stdOut == 0)
            {
                int sourceFD, result;
   
                sourceFD = open(stdInFile, O_RDONLY);
                if (sourceFD == -1) 
                { 
                    printf("cannot open %s for input\n", stdInFile);
                    fflush(stdout);
                    exit(1); 
                    
                }


                result = dup2(sourceFD, 0);
                if (result == -1) 
                { 
                    perror("source dup2()"); exit(2); 
                    
                }
                    
                
                test = execlp(args[0], args[0], NULL);
                fflush(stdout);
                exit(1);
            }
            
            
            //if stdin and stdout are called we redirect data
            if (stdIn == 1 && stdOut == 1)
            {
                //call dup2 for stdin
                int sourceFD, targetFD, result;
   
                sourceFD = open(stdInFile, O_RDONLY);
                if (sourceFD == -1) 
                { 
                    printf("cannot open %s for input\n", stdInFile);
                    exit(1); 
                    
                }

                //redirect stdin using dup2
                result = dup2(sourceFD, 0);
                if (result == -1) 
                { 
                    perror("source dup2()"); exit(2); 
                    
                }
                
                
                //designate the file to be open and written to
                targetFD = open(stdOutFile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                
                if (targetFD == -1) 
                {
                    printf("cannot open %s for input\n", stdOutFile);
                    exit(1); 
                }
                
                //redirect stdout with dup2
                int result2 = dup2(targetFD, 1);
                
                if (result2 == -1) 
                {
                    perror("dup2"); exit(2); 
                }
                
                
                    
                
                test = execlp(args[0], args[0], NULL);
                exit(1);
            }            
             
            //redirect stdin and stdout to /dev/null if its a background task
            //reference: https://stackoverflow.com/questions/14846768/in-c-how-do-i-redirect-stdout-fileno-to-dev-null-using-dup2-and-then-redirect
            if (bg == 1 && fg == 0)
            {
              
                int devNull = open("/dev/null", O_WRONLY);
                
                int result2 = dup2(devNull, 1);
                int result = dup2(devNull, 0);
            }

            //if bg & sleeping
            if (bg == 1 && sleeping == 1 && fg == 0)
            {
                test = execlp(args[0], args[0], sleepArg, NULL);
                printf("error!");
                exit(1);
            }
            
            if (bg == 1 && sleeping == 1 && fg == 1)
            {
                test = execlp(args[0], args[0], sleepArg, NULL);
                printf("error!");
                fflush(stdout);
                exit(1);
            }              
            

            if (argNumber > 1  && bg == 0 && echoCall == 0)
            {
                //first arg is the command, 2nd is the argument
                if (argNumber == 2)
                {
                    
                    test = execlp(args[0], args[0], args[1], NULL);
                    printf("error!");
                    fflush(stdout);
                    exit(1);
                }
                //if there are 2 arguments
                else if (argNumber == 3)
                {
                    test = execlp(args[0], args[0], args[1], args[2], NULL);
                    printf("error!");
                    fflush(stdout);
                    exit(1);
                }

            }
            
            
          
            
            
            if (echoCall == 1)
            {
                execlp(args[0], args[0], echoString, NULL);
                printf("error!");
                fflush(stdout);
                exit(1);
            }
            
            
            
            else
            {
                //call 1 arguement or a failed
                test = execlp(args[0], args[0], NULL);
                printf("%s: no such file or directory\n", args[0]);
                fflush(stdout);
                exit(1);
            }

            break;
        //parent    
        default:
            //make sure child process is done
            //if entered in the foreground then we wait for completion first
            if ((bg == 0 && fg == 0) || (bg == 1 && fg == 1) || 
            ((fg == 1 && bg == 0)))
            {
                pid_t childProcess = waitpid(spawnpid, &childExitMethod, 0);
                
                


                
                
                //reset spawnpid for new process
                spawnpid = -5;
                break;
            }
            //if entered in the background then add to our unfinished process
            else if (bg == 1 && fg == 0)
            {

                unfinishedProcesses[pidIncrementor] = spawnpid;
                pidIncrementor++;
                printf("background pid is %d\n", spawnpid);
                fflush(stdout);
                break;
            }
            
            
    }
}

//http://www.cplusplus.com/reference/cstring/strtok/
void readData(){
    
    //allocate memory for arguments
    i = 0;
    for (i = 0; i < 512; i++)
    {
        args[i] = malloc(50 * sizeof(*args[i]));
        memset(args[i],'\0', 50 * sizeof(*args[i]));
    }    
    
    //reset global variables
    echoCall = 0;
    statusCall = 0;
    linuxCommand = 0;
    backgroundTask = 0;
    cd = 0;
    notes = 0;
    stdOut = 0;
    stdIn = 0;
    bg = 0;

    sleeping = 0;
    //clear echo string
    memset(echoString,'\0', sizeof(echoString));
    memset(cdMove,'\0', sizeof(cdMove));
    memset(stdOutFile,'\0', sizeof(stdOutFile));
    memset(commandName,'\0', sizeof(commandName));    
    memset(stdInFile,'\0', sizeof(stdInFile));    
    memset(sleepArg,'\0', sizeof(sleepArg));     

    
    //clear input buffer
    memset(input,'\0', sizeof(input));
    //read user input
    printf(": ");    
    fflush(stdout);
    //get user input
    fgets(input, sizeof input, stdin); 

    
    //replace any $$ with a getpid
    //reference: https://stackoverflow.com/questions/15465755/replacing-a-word-in-a-text-by-another-given-word
    int pidTest = 0;
    char pidVar[100];
    //clear space for pidVariable
    memset(pidVar, '\0', sizeof(pidVar));
    //generate PID
    sprintf(pidVar, "%d", getpid());
    
    char tempInput[2048];
    memset(tempInput,'\0', sizeof(tempInput));  
    
    //look for $$ in the input
    char *pos = strstr(input, "$$");
  
    
    // if found $$ found replace it with pidVariable
    if (pos != NULL) {
        // The new string
        size_t newlen = strlen(input)-1 - strlen("$$") + strlen(pidVar);
        char new_sentence[newlen + 1];
        //printf("%i", newlen);
       
    
        // Copy the part of the old sentence *before* the replacement
        memcpy(new_sentence, input, pos - input);

        // Copy the replacement
        memcpy(new_sentence + (pos - input), pidVar, strlen(pidVar));
        
        
    
        // Copy the rest
        strcpy(new_sentence + (pos - input) + strlen(pidVar), pos + strlen("$$"));
        //clear input
        memset(input,'\0', sizeof(input));  
        //replace input with new string
        strcpy(input,new_sentence);
    }
   
    
    
    
    //check first character is a note '#'
    if (input[0] == '#')
    {
        notes = 1;
    }
    
    //split up user input by space
    pch = strtok(input, " \n");
    i = 0;
    
    //check args, with delimitor space and \n
    while (pch != NULL)
    {
        strcpy(args[i], pch);
        
        pch = strtok(NULL, " \n");   
        i++;

    }
    

    //holds our argument number
    argNumber = i;
   

    
    //check if the first arg is an exit
    //reference: https://stackoverflow.com/questions/6501522/how-to-kill-a-child-process-by-the-parent-process
    if (strcmp(args[0], "exit") == 0)
    {
        exitCall = 1;
        
        for (i = 0; i < pidIncrementor; i++)
        {
            kill(unfinishedProcesses[i],15);
        }
        
    }
    
    //reset i
    i = 0;
    //check for exit call
    for (i = 0; i < argNumber; i++)
    {
        
        
        //check if echo is called
        if (strcmp(args[i], "echo") == 0)
        {
            echoCall = 1;
            linuxCommand = 1;
            //linuxCommand = 1;
            //get the string after echo
            
            int p = 1;
            const char singleSpace[1] = " ";
            
            while (strcmp(args[i+p], "") != 0)
            {
                strcat(echoString, args[i+p]);
                //printf("%s", args[i+p]);
                //add a space between arguments
                strcat(echoString, " ");
                //printf("%s",echoString);
                
                p++;
            }
            //copy the string to be printed into stdOutFile
            strcpy(stdOutFile, echoString);
            
        }
        
        //check for status call
        if (strcmp(args[i], "status") == 0)
        {
            statusCall = 1;
        }
        
        //check if background is entered
        if (strcmp(args[i], "&") == 0)
        {
            backgroundTask = 1;
        }
        
        //check if a comment is entered
        if (strcmp(args[i], "#") == 0)
        {
            notes = 1;
            linuxCommand = 0;
            //get the string after #
            int p = 1;
            const char singleSpace[2] = " ";
            
            while (args[i+p] != 0)
            {
                strcat(noteString, args[i+p]);
                //add a space between arguments
                strcat(noteString, singleSpace);
                p++;
            }
            
            
        }
        
        //if a cd is found
        if (strcmp(args[i], "cd") == 0)
        {
            cd = 1;
            
            //if there is another argument
            if (strcmp(args[i + 1], "") != 0)
            {
                
                strcpy(cdMove, args[i+1]);
            }
            
            //if nothing in next string
            //move to the home directory
            else if (strcmp(args[i + 1], "") == 0)
            {
                //Reference:https://www.stev.org/post/cgethomedirlocationinlinux
                //get home directory
                memset(mainDir,'\0', sizeof(cdMove));
                strcpy(mainDir, getenv("HOME"));
                printf("home dir: %s", mainDir);
            }
            
            
        }
        
        //check if stdOut is called
        if (strcmp(args[i], ">") == 0)
        {
            stdOut = 1;
            //if a file is indicated after the ">"
            if (strcmp(args[i+1], "") != 0)
            {
                //get the file to create and write into
                strcpy(stdOutFile, args[i+1]);
            }
            
            
            
        }
        
        //check if stdIn is called
        if (strcmp(args[i], "<") == 0)
        {
            stdIn = 1;
            //if a file is indicated after the "<"
            if (strcmp(args[i+1], "") != 0)
            {
                //get the file to create and write into
                strcpy(stdInFile, args[i+1]);
            }
            
            
            
        }
        
        //check if background is called
        if (strcmp(args[i], "&") == 0)
        {
            //make sure nothing is after &
            if (strcmp(args[i+1], "") == 0)
            {
                bg = 1;
                
                //linuxCommand = 1;
            }
        }
        
        //check if a sleep call
        if (strcmp(args[i], "sleep") == 0)
        {
            if (strcmp(args[i+1], "") != 0)
            {
                sleeping = 1;
                strcpy(sleepArg, args[i+1]);
                
            }
            
            
            
            
        }        
        
        
        
        //make sure its not an echo, status, exit, note, cd
        //if its none of these, then we are going to fork and execute
        if (statusCall == 0 && echoCall == 0 && exitCall == 0 && notes == 0 && cd == 0) 
        {
            linuxCommand = 1;
        }


    //if exit call
    if (exitCall == 1)
    {
        printf("%s\n", args[0]);
    }
    
    //if status call we check for exit value and signal value
    if (statusCall == 1)
    {


        //get the exit status
        if (WIFEXITED(childExitMethod))
        {
            
            int exitStatus = WEXITSTATUS(childExitMethod);
            printf("exit value was %d\n", exitStatus);
            fflush(stdout);
        }

        //get the signal status
         else if (WIFSIGNALED(childExitMethod))
        {
            int termSignal = WTERMSIG(childExitMethod);
            printf("terminated by signal %d\n", termSignal);
            fflush(stdout);
        }
        
    }
    
    //if cd is called by itself we change to the main directory
    if (cd == 1)
    {
        if (argNumber == 1)
        {
            chdir(mainDir);
        }
        //if there is more than one argument then move to the 2nd argument
        else if (argNumber > 1)
        {
        //move directories
        chdir(cdMove);
        }
        
    }
        
        
    }

}





void checkSignal()
{
    //clear up allocated argument array
    i = 0;
    for (i = 0; i < 512; i++)
    {
        free(args[i]);
    }
    
    
    
    //check for background tasks to be complete
    //need to check exit and signal also status
    i = 0;

    for (i = 0; i < pidIncrementor; i++)
    {
        //if childPID comes back with 0 then its not done.
        pid_t childPID = waitpid(unfinishedProcesses[i], &childExitMethod, WNOHANG);
        
        //remove PID from unfinished processes and decrease PID incrementory case user hits CTRL Z, or an exit is types
        
        if (childPID != 0)
        {
            //get the exit status
            if (WIFEXITED(childExitMethod))
            {

                
                
                int exitStatus = WEXITSTATUS(childExitMethod);
                printf("exit value was %d\n", exitStatus);
                
            }
            
            //get the signal status
             if (WIFSIGNALED(childExitMethod))
            {            
                    int exitStatus = WTERMSIG(childExitMethod);
                    printf("terminated by signal %d\n", exitStatus);
                    
            }
                
                //if its finished remove from unfinished process array
                printf("background pid %d is done\n", unfinishedProcesses[i]);
                unfinishedProcesses[i] = -5;
                pidIncrementor--; 

        }
    }
    
    
    
    
}




//checks if user hits CTRL-shift-C
//changes to foreground mode
void catchSIGINT(int signo)
{
    //if foreground mode is not on, set it to one
    if (fg == 0)
    {
        char* message = "Entering foreground mode!\n";
        write(STDOUT_FILENO, message, 26);
        fg = 1;
    }
    else if (fg == 1)
    {
        char* message = "Exiting foreground mode!\n";
        write(STDOUT_FILENO, message, 25);
        fg = 0;
        
    }

}




int main() 

{
    struct sigaction SIGINT_action = {0}, SIGUSR2_action = {0}, ignore_action = {0};
    //when CTRL-shift Z is hit we call catchSIGINT function - which enters foreground mode    
    SIGINT_action.sa_handler = catchSIGINT;
    SIGINT_action.sa_flags = 0;
    //ignore signal
    ignore_action.sa_handler = SIG_IGN;
    
    //terminal stop when CTRL_shift Z is hit
    sigaction(SIGTSTP, &SIGINT_action, NULL);
    
    //ignore CTRL C default action
    sigaction(SIGINT, &ignore_action, NULL);

    
    
    
    //while exit is not type we loop program to take in user input
    while (exitCall == 0)
    {
        //reads data, checking what type of call it is
        readData();
        
        //if a linux command we fork and execute
        if (linuxCommand == 1)
        {
            forknewprocess();
        }
        
        checkSignal();
    }
    


    
    
    return 0;
}