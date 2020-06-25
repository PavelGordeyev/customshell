/********************************************************************* 
** CS344 - Operating Systems
** Program name: Program 3 - smallsh
** Author: Pavel Gordeyev
** Date: 5/18/20
** Description:  Shell implementation. The shell will run command line
**				 instructions and return the results similar to other 
**				 shells, but without many of their fancier features.
**				 It will work similarly to the bash shell. It
**				 supports the redirection of standard input and 
**				 standard output and it will support both foreground 
**				 and background processes.
*********************************************************************/
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include "dynArrPID.h"
#include "dynArrChar.h"

void copyArgs(struct DynArrChar*,char*[],int,pid_t,char*);
char * parseInput(struct DynArrChar*,char*);
void adjustInput(char*);
int isBackground(char*);
int getRedirectPos(char*[],int,char*);
void checkBG(struct DynArrPID*);
void catchSIGTSTP(int);

// Global status for entering/exiting foreground mode
int fgSTATUS = 0;


int main(){

	// Initialize input & buffer
	char * cInput;

	size_t bufferSize = 2048;
	cInput = malloc(bufferSize * sizeof(char));

	// Create dynamic array to hold process id's
	struct DynArrPID * processArr;
	processArr = newDynArrPID(100);

	int i = 0;

	pid_t spawnPid = -5,
		  parentPid = getpid();
	int childExitMethod = -5;

	// Signal handling for SIGINT
	struct sigaction SIGINT_action = {{0}};
	SIGINT_action.sa_handler = SIG_IGN; 
	sigfillset(&SIGINT_action.sa_mask); 
	SIGINT_action.sa_flags = 0;

	// Signal handling for SIGTSTP
	struct sigaction SIGTSTP_action = {{0}};
	SIGTSTP_action.sa_handler = catchSIGTSTP; 
	sigfillset(&SIGTSTP_action.sa_mask); 
	SIGTSTP_action.sa_flags = 0;

	// Publish the signal handling actions
	sigaction(SIGINT, &SIGINT_action, NULL);
	sigaction(SIGTSTP, &SIGTSTP_action, NULL);

	int numChars;

	while(1){

		// Check to see if any background processes completed
		checkBG(processArr);
		
		// Read in command
		while(1){

			// Colon prompt
			printf(": ");

			// Get input
			numChars = getline(&cInput,&bufferSize,stdin);

			// Validate input was received
			if(numChars == -1)
				clearerr(stdin);
			else
				break;
		}

		// Correct for newline character
		adjustInput(cInput);

		// Determine if the process was requested to be run in the background
		int backgroundBool = isBackground(cInput);

		// Get values from input
		struct DynArrChar * argsArr;
		argsArr = newDynArrChar(32);
		char * cmd = parseInput(argsArr,cInput);

		int numElements;

		// Set the elements based on whether a command was input
		if(cmd == NULL){
			numElements = 1;
		}else{
			numElements = sizeDynArrChar(argsArr) + 2;
		}

		// Copy to an arguments array
		char * args[numElements];
		copyArgs(argsArr,args,numElements,parentPid,cmd);

		// Get redirect positions
		char * inpChar = "<";
		char * outChar = ">";

		int inputRedirect = getRedirectPos(args,numElements,inpChar);
		int outputRedirect = getRedirectPos(args,numElements,outChar);

		// Checks to see if background was prompted for and if it can be run
		// Remove '&' from args
		if(fgSTATUS == 0 && numElements > 2 && backgroundBool){
			args[numElements - 2] = NULL;
		}

		// Remove '&' from args when in foreground-only mode; reset background boolean
		if(fgSTATUS == 1 && numElements > 2 && backgroundBool){
			args[numElements - 2] = NULL;
			backgroundBool = 0;
		}

		// Initialize err message
		char err[255];
		char * message;
		message = malloc(255 * sizeof(char));

		// Check for blank line or if it begins with #
		if(cmd != NULL && cmd[0] != '#'){ // Go to next line

			if(strcmp(args[0],"exit") == 0){

				if(sizeDynArrPID(processArr) > 0){ // Kill all remaining processes, if any
					
					for(int i = 0; i < sizeDynArrPID(processArr); i++){
						
						kill(processArr->data[i], SIGTERM);
						waitpid(processArr->data[i], &childExitMethod, 0);

						if(WIFEXITED(childExitMethod) != 0){
							// printf("Exit status was %d\n", WEXITSTATUS(childExitMethod));
						}else{
							// printf("Process %d was terminated by signal %d\n",processArr->data[i],WTERMSIG(childExitMethod));
						}
					}
				}

				// Free memory
				free(cInput);
				free(message);
				deleteDynArrChar(argsArr);
				deleteDynArrPID(processArr);

				exit(0);

			}else if(strcmp(args[0],"cd") == 0){

				char * message = "no such filepath or directory\n";
				
				// Change to home directory without any arguments
				if(sizeDynArrChar(argsArr) == 0){
					if(chdir(getenv("HOME")) != 0){
						write(STDOUT_FILENO,message,30);
					}
				}else if(sizeDynArrChar(argsArr) == 1){ // Change to specified directory
					if(args[1][0] == '/'){ // Absolute path
						if(chdir(args[1]) != 0){
							write(STDOUT_FILENO,message,30);
						}
					}else{ // Relative path
						char * cwd;
						char * path;
						char newDir[255];

						cwd = getcwd(NULL, 0);

						// Copy relative path to create string 
						path = malloc(255 * sizeof(char));
						strcpy(path,args[1]);

						if(cwd != NULL){
							sprintf(newDir,"%s/%s",cwd,path);

							if(chdir(newDir) != 0){
								write(STDOUT_FILENO,message,30);
							}
						}else{
							perror("error getting current directory...\n");
						}

						free(path);
					}

				}else{
					printf("Invalid use of cd!\n");
				}

			}else if(strcmp(args[0],"status") == 0){

				if(WIFEXITED(childExitMethod) != 0){
					printf("exit value %d\n",WEXITSTATUS(childExitMethod));
					fflush(stdout);
				}else{
					printf("terminated by signal %d\n",WTERMSIG(childExitMethod));
					fflush(stdout);
				}

			}else{ // Spawn a process from an existing command

				// Ignore SIGTSTP within the child process
				SIGTSTP_action.sa_handler = SIG_IGN;
				sigaction(SIGTSTP, &SIGTSTP_action, NULL);
				
				spawnPid = fork();

				switch(spawnPid){
					case -1: {
						perror("Hull Breach!\n");
						exit(1);
						break;
					}

					case 0:{

						// Set default action for a SIGINT
						SIGINT_action.sa_handler = SIG_DFL; 						
						sigaction(SIGINT, &SIGINT_action, NULL);

						int sourceFD, targetFD, errorFD, result;

						if(backgroundBool){ // Redirect STDERR,STDIN,STDOUT

							// STDIN Redirect
							sourceFD = open("/dev/null", O_RDONLY);
							result = dup2(sourceFD,0);

							// STDOUT Redirect
							targetFD = open("/dev/null", O_WRONLY | O_TRUNC, 0644);
							result = dup2(targetFD,1);

							// STERR Redirect
							errorFD = open("/dev/null", O_RDWR, 0644);
							result = dup2(errorFD,2);
						}

						// Arguments were passed in
						if(sizeDynArrChar(argsArr) > 0){

							if(inputRedirect != -1 && outputRedirect != -1){ // Input & output redirection

								// Input redirection
								sourceFD = open(args[inputRedirect + 1], O_RDONLY);

								if(sourceFD == -1){

									if(backgroundBool){ // background process redirect to /dev/null

										sourceFD = open("/dev/null", O_RDONLY);

									}else{

										char * err = "cannot open file for input\n";
										
										write(STDOUT_FILENO,err,27);

										exit(1);
									}
								}

								result = dup2(sourceFD,0);
								if(result == -1){
									perror("target dup2()");
									exit(2);
								}

								// Output redirection
								targetFD = open(args[outputRedirect + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
								if(targetFD == -1){
									perror("target is open\n");
									exit(1);
								}

								result = dup2(targetFD,1);
								if(result == -1){
									perror("target dup2()");
									exit(2);
								}

								execlp(args[0],args[0],NULL);
								perror("Child: exec failure!\n");
								exit(1);

							}else if(outputRedirect != -1){ // Output redirection
									
								targetFD = open(args[outputRedirect + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
								if(targetFD == -1){
									perror("target is open\n");
									exit(1);
								}

								result = dup2(targetFD,1);
								if(result == -1){
									perror("target dup2()");
									exit(2);
								}

								execlp(args[0],args[0],NULL);
								perror("Child: exec failure!\n");
								exit(1);

							}else if(inputRedirect != -1){ // Input redirection
									
								sourceFD = open(args[inputRedirect + 1], O_RDONLY);

								if(sourceFD == -1){

									if(backgroundBool){ // background process redirect to /dev/null

										sourceFD = open("/dev/null", O_RDONLY);

									}else{

										char * err = "cannot open file for input\n";
										
										write(STDOUT_FILENO,err,27);

										exit(1);
									}
								}

								result = dup2(sourceFD,0);
								if(result == -1){
									perror("target dup2()");
									exit(2);
								}

								execlp(args[0],args[0],NULL);
								perror("Child: exec failure!\n");
								exit(1);

							}else{
								if(execvp(args[0],args) < 0){
									perror("Exec failed!");
									exit(1);
								}
							}
						}else{ // No arguments
							execlp(args[0],args[0],NULL);

							sprintf(err,"%s: no such file or directory\n",args[0]);
							strcpy(message,err);
							write(STDOUT_FILENO,err,strlen(message));

							exit(2);
						}

						break;
					}

					default:{

						if(backgroundBool){

							// Notify of background process
							char bg[255];
							char * bgMessage;
							bgMessage = malloc(255 * sizeof(char));

							sprintf(bg,"background pid is %d\n",spawnPid);
							strcpy(bgMessage,bg);

							write(STDOUT_FILENO,bgMessage,strlen(bgMessage));
							free(bgMessage);

							waitpid(spawnPid,&childExitMethod,WNOHANG);

							addDynArrPID(processArr,spawnPid);

						}else{

							// Re-establish SIGTSTP action for the shell
							SIGTSTP_action.sa_handler = catchSIGTSTP;
							sigaction(SIGTSTP, &SIGTSTP_action, NULL);

							waitpid(spawnPid,&childExitMethod,0);

							// Check to see if any background processes completed
							checkBG(processArr);

							if(WIFEXITED(childExitMethod) != 0){
								// No output
							}else{
								printf("terminated by signal %d\n",WTERMSIG(childExitMethod));
								fflush(stdout);
							}
						}
						break;
					}
				}
			}
		}

		deleteDynArrChar(argsArr);
		free(message);
		i++;
	}

	return 0;

}

/********************************************************************* 
**        Name: checkBG
**    Synopsis: Checks if any background processes have completed.
** Description: Checks if any background processes have completed
**				and prints out the respective status messages. If
**				the process was exited or terminated, it is removed
**				from the array of background processes.
*********************************************************************/
void checkBG(struct DynArrPID * arr){

	pid_t childPID;
	int childStatus = -5;

	for(int i = 0; i < sizeDynArrPID(arr); i++){

		childPID = waitpid(arr->data[i],&childStatus,WNOHANG);

		if(childPID > 0){

			if(WIFEXITED(childStatus) != 0){
				
				printf("background pid %d is done: exit value %d\n",childPID,WEXITSTATUS(childStatus));
				fflush(stdout);
			}else{
				printf("background pid %d is done: terminated by signal %d\n",childPID,WTERMSIG(childStatus));
				fflush(stdout);
			}

			// Remove process from array since it has exited or was terminated
			removeAtDynArrPID(arr,i);

		}else if(childPID == -1){

			// Remove process from array since it has exited or was terminated
			removeAtDynArrPID(arr,i);
		}
	}	
}

/********************************************************************* 
**        Name: catchSIGTSTP
**    Synopsis: Prints messages when SIGTSTP signal is sent.
** Description: Signal handler for the SIGTSTP signal that prints out
**				messages depending if the current mode is
**				foreground-only or not.
*********************************************************************/
void catchSIGTSTP(int signo){

	char * fgOnlyMessage = "\nEntering foreground-only mode (& is now ignored)\n";
	char * exitMessage = "\nExiting foreground-only mode\n";

	if(fgSTATUS){ // Currently in foreground-only mode; will exit
		fgSTATUS = 0;
		write(STDOUT_FILENO, exitMessage, 30);
	}else{ // Enter foreground-only mode
		fgSTATUS = 1;
		write(STDOUT_FILENO, fgOnlyMessage, 50);
	}
	
}

/********************************************************************* 
**        Name: getRedirectPost
**    Synopsis: Returns the index of the redirect symbol
** Description: Finds the redirect symbol, "<" or ">", in the given
**				character array of arguments and returns its index.
*********************************************************************/
int getRedirectPos(char * args[], int numElements, char * c){

	// Loop through all arguments to find the specified redirection
	for(int i = 0;i < numElements - 1;i++){
		if(strcmp(args[i],c) == 0)
			return i;
	}
	
	// No redirection symbol found
	return -1;
}

/********************************************************************* 
**        Name: copyArgs
**    Synopsis: Returns a character array of input arguments
** Description: Copies values from the dynamic array into a character
**				array to be used by the execvp function in main.
*********************************************************************/
void copyArgs(struct DynArrChar * arr, char * args[], int numElements, pid_t parentPid, char * cmd){

	const char s[3] = "$$";
	char * token;
	char strPid[2048];
	char * arg;

	
	if(cmd != NULL){

		// Set command input to first element
		args[0] = cmd;

		// Loop through all argument elements
		for(int i = 0; i < numElements - 2; i++){

			arg = malloc((strlen(arr->data[i]) + 1) * sizeof(char));

			strcpy(arg,arr->data[i]);

			// Check for $$
			token = strtok(arg, s);

			if(token != NULL && strcmp(arr->data[i],token) != 0){
				sprintf(strPid,"%s%d",token,parentPid);

				// Get text after the $$
				token = strtok(NULL,"$");
				if(token != NULL){
					sprintf(strPid,"%s%s",strPid,token);
				}

				args[i + 1] = strPid;

			}else if(strcmp(arr->data[i],"$$") == 0){ // argument is $$
				sprintf(strPid,"%d",parentPid);
				args[i + 1] = strPid;
			}else{
				args[i + 1] = arr->data[i];
			}

			free(arg);
		}
	}

	// Last argument needs to be NULL
	args[numElements - 1] = NULL;
}

/********************************************************************* 
**        Name: parseInput
**    Synopsis: Returns command input and associated arguments, if any.
** Description: Reads the first part of the line entered as the 
**				command and the rest as the arguments.  Protects 
**				against an empty input
*********************************************************************/
char * parseInput(struct DynArrChar * arr,char * input){

	const char space[2] = " ";
	char * token;
	char * cmd;

	// Get the initial command
	cmd = strtok(input,space);
	
	// Validate for not NULL
	if(cmd){

		if(strcmp(cmd,"echo") == 0){
			token = strtok(NULL,"\0");
			if(token != NULL)
				addDynArrChar(arr,token);
		}else{
			// Get the arguments
			token = strtok(NULL,space);

			while(token != NULL){
				addDynArrChar(arr,token);
				token = strtok(NULL,space);
			}
		}
	}

	return cmd;
}

/********************************************************************* 
**        Name: adjustInput
**    Synopsis: Strips new line character from input
** Description: Strips new line character, "\n", from input and adds
**				end of line character, "\0".
*********************************************************************/
void adjustInput(char * input){
	input[strlen(input) - 1] = '\0';
}

/********************************************************************* 
**        Name: isBackground
**    Synopsis: Returns if a input command is needed to run in the
**				background.
** Description: Checks if the last character entered is a '&'
**				to determine if the input command is a background
**				process.  Returns 1 if it is a background process,
**				0 otherwise.
*********************************************************************/
int isBackground(char * input){

	if(strlen(input) > 0){
		if(input[strlen(input) - 1] == '&')
			return 1;
	}

	// Not a background process
	return 0;
}