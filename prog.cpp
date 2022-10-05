/**
 * Assignment 2: Simple UNIX Shell
 * @file pcbtable.h
 * @author ??? (TODO: your name)
 * @brief This is the main function of a simple UNIX Shell. You may add additional functions in this file for your implementation
 * @version 0.1
 */
// You must complete the all parts marked as "TODO". Delete "TODO" after you are done.
// Remember to add sufficient and clear comments to your code

	#include <stdio.h>
	#include <unistd.h>
	#include <stdlib.h>
	#include <string.h>
	#include <sys/wait.h>
	#include <fcntl.h>

	#define MAX_LINE 80 /* The maximum length command */




	/**
	 * This function clears the Arguments 
	 * stored in the command array
	*/
	void clearArguments(char *args[], int num_args) {
	int i = 0;
	  while (args[i] != NULL && (i < num_args)) {//while the array is not empty
	  	free(args[i]);
	  	i++;
	  	if (i == MAX_LINE) {
	  		break;
	  	}
	  }
	}

	/**
	 * @brief parse out the command and arguments from the input command separated by spaces
	 *
	 * @param command
	 * @param args
	 * @return int
	 */
	int parse_command(char command[], char *args[]) {
		int num_args = 0;
	  	int argc; /* argument count */
		char delimiter[] = " ";
		//char input[MAX_LINE];
	  argc = read(STDIN_FILENO, command, MAX_LINE);//reads user input
	  //argc = sizeof(command); //TODO how to get size of command.
	  //printf("*******TESTING\n");
	  
	  //printf("********argc = %d\n",argc);
	  
	  if (command[argc - 1] == '\n') command[argc - 1] = '\0';
	  if (strcmp(command, "!!") == 0) {//check for history command
	  	if (num_args == 0) {
	  		printf("No commands in history.\n");
	  	}
	  	return num_args;
	  }
	  if (strcmp(command, "exit") == 0) {//exits the program on command
	  	exit(1);
	  }

	  clearArguments(args, num_args);//clears args array
	  num_args = 0;

	  char *params = strtok(command, delimiter);
	  while (params != NULL) {//while there are still values to be read
	    if (params[0] == '&') {//this triggers if the user adds the & to a command
	    	//*amp = 1;
	    	params = strtok(NULL, delimiter);
	    	continue;
	    }
	    num_args += 1;
	    args[num_args - 1] = strdup(params);
	    params = strtok(NULL, delimiter);
	  }
	  args[num_args] = NULL;
	  return num_args;
	}


	//this is the main function that take care of forking and piping
int main(int argc, char *argv[]){
	  char command[MAX_LINE];       // the command that was entered
	  char *args[MAX_LINE / 2 + 1]; /* command line arguments */
	  int should_run = 1; /* flag to determine when to exit program */
		pid_t pid;
		int amp = 0;
		int usePipe = 0;
		int redirect, file, pidPipe;
		int fd[2];
	  while (should_run) {//loops through the program until exit is called
	  	printf("osh>");
        fflush(stdout);
        // Read the input command
        //fgets(command, MAX_LINE, stdin);  //TODO figure out how to get this working 
        //printf("adsfadsfa fadsf>");
        // Parse the input command
	  	int num_args = parse_command(command, args);
	  	pid = fork();
	    if (pid == 0) {//fork successful
	      if (num_args == 0) {//there are no commands
	      	continue;
	      } else {//there are commands
	      	redirect = 0;
		for (int i = 1; i <= num_args - 1; i++) {//iterates through the array
		  if (strcmp(args[i], "<") == 0) {//check for input redirect
		    file = open(args[i + 1], O_RDONLY);//opens file to read
		    if (file == -1 || args[i + 1] == NULL) {
		    	printf("invalid command\n");
		    	return 1;
		    }
		    dup2(file, STDIN_FILENO);
		    args[i] = NULL;
		    args[i + 1] = NULL;
		    redirect = 1;
		    break;
		  } else if (strcmp(args[i], ">") == 0) {//check for output redirect
		    file = open(args[i + 1], O_WRONLY | O_CREAT, 0644);//opens/creates file to write
		    if (file == -1 || args[i + 1] == NULL) {
		    	printf("invalid command\n");
		    	return 1;
		    }
		    dup2(file, STDOUT_FILENO);
		    args[i] = NULL;
		    args[i + 1] = NULL;
		    redirect = 2;
		    break;
		  } else if (strcmp(args[i], "|") == 0) {//checks for piping
		  	usePipe = 1;
		  	if (pipe(fd) == -1) {
		  		fprintf(stderr, "pipe failed");
		  		return 1;
		  	}
		  	char *args1[i + 1];
		  	char *args2[num_args - i - 1 + 1];
		    for (int j = 0; j < i; j++) {//stores first args command in args1
		    	args1[j] = args[j];
		    }
		    args1[i] = NULL;
		    for (int j = 0; j < num_args - i - 1; j++) {//stores second args command in args2
		    	args2[j] = args[j + i + 1];
		    }
		    args2[num_args - i - 1] = NULL;
		    pidPipe = fork();// creates a new fork
		    if (pidPipe > 0) {//waits for first pipe to finish
		    	wait(NULL);
		    	close(fd[1]);
		    	dup2(fd[0], STDIN_FILENO);
		    	close(fd[0]);
		      if (execvp(args2[0], args2) == -1) {//executes command
		      	printf("invalid command\n");
		      	return 1;
		      }
		    } else if (pidPipe == 0) {//first pipe
		    	close(fd[0]);
		    	dup2(fd[1], STDOUT_FILENO);
		    	close(fd[1]);
		      if (execvp(args1[0], args1) == -1) {//executes command
		      	printf("invalid command\n");
		      	return 1;
		      }
		      return 1;
		    }
		    close(fd[0]);
		    close(fd[1]);
		    break;
		  }
		}
		if (usePipe == 0) {//no piping
		  if (execvp(args[0], args) == -1) {//executes command
		  	printf("invalid command\n");
		  	return 1;
		  }
		}
		if (redirect == 1) {//closes files
			close(STDIN_FILENO);
		} else if (redirect == 2) {
			close(STDOUT_FILENO);
		}
		close(file);
	}
	return 1;
	    } else if (pid > 0) {//only if first fork piped
	    	if (amp == 0) {
	    		wait(NULL);
	    	}
	    } else {// fork failed
	    	printf("forking error");
	    }
	  }
	  return 0;
	}
