/**
 * Assignment 2: Simple UNIX Shell
 * @file pcbtable.h
 * @author Tim Nguyen, Josh Shor
 * @brief This is the main function of a simple UNIX Shell. You may add additional functions in this file for your implementation
 * @version 0.1
 */
// You must complete the all parts marked as "TODO". Delete "TODO" after you are done.
// Remember to add sufficient and clear comments to your code
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <fcntl.h>
#include <cstring>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/wait.h>

#include <string.h>
#include <sys/stat.h>


using namespace std;

#define MAX_LINE 80 // The maximum length command

int totalWords(char command[]);

bool isLess, isGreater , isAnd = false; //Verify less than, greater ehan and & symbol. 
int num_args = 0;
char hist[MAX_LINE],hist2[MAX_LINE]; //History used when typing  !!.


/**
 * Set flags base on param
 * 
 */ 
void setFlags(char *params){
	 if(strcmp(params,">") == 0) { //Verify if param is >
		isGreater = true; 
	}else if(strcmp(params,"&") == 0){//Verify if param is &
		 isAnd = true; 
	}else if(strcmp(params,"<") == 0) {//Verify if param is <
	 	 isLess = true; 
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
  char *params = strtok(command, " "); //parses the command with " " to params array. 
  while (params != NULL) {
    setFlags(params);

    args[num_args] = params; //Add thes params into the args array
    params = strtok(NULL, " "); //reset the params 
    num_args++;
  }
  args[num_args] = params; //puts the last params into the args array. 
  return num_args;
}


/*
* reset the flags
*/
void resetFlags(){
  isAnd = false; //resets bool
  isLess = false; //resets bool
  isGreater = false; //resets bool
}
/**
 * @brief process the fork.
 *
 * @param pid
 * @param args
 */
void processFork(pid_t pid, char *args[]) {
	 
if (pid > 0) { //check Parent
  if (isAnd == true) { //Verify& in the command
  } else {
    wait(NULL); //wait 
  }
} else if (pid == 0) { //child
  if ((execvp(args[0], args) < 0)){ //Verify if is invalid
    printf("Command not found\n"); //print not found
    exit(1);
  } else {
    execvp(args[0], args); //if  not invalid, run the command
    }
} else { //error
    printf("error\n" ); //print fork error
}
strcpy(hist, hist2); //copies history.
resetFlags();
}

/**
 * Process if < Than
 */ 
void processLess(char *args[]){
	args[num_args-2] = NULL; //set the < to NULL so the command can be run
    int inside = open(args[num_args-1], O_RDONLY); 
    dup2(inside, STDIN_FILENO);
    close(inside);
}
/**
 * Proces if > than
 */ 
void porcessGreater(char *args[]){
  args[num_args-2] = NULL;
  int outside = open(args[num_args-1], O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
  dup2(outside, STDOUT_FILENO);
  close(outside);
}
/**
 * Processs if & set to null
 */ 
void processAnd(char *args[]){
	args[num_args-1] = NULL;
}

/**
 * Processs History
 */ 
void processHistory(char *args[]){
  if (strcmp(hist,"") == 0) { //checks if the hist is empty, prints "No command history"
    printf( "No command history found\n");
  } else {
    printf("%s\n", hist); // print out the history 
    num_args = parse_command(hist, args); //calls the parse_command function with the history as the arguments. 
  }
}



/**
 * @brief The main function of a simple UNIX Shell. You may add additional functions in this file for your implementation
 * @param argc The number of arguments
 * @param argv The array of arguments
 * @return The exit status of the program
 */
int main(int argc, char *argv[])
{
  char command[MAX_LINE];       // the command that was entered
  char *args[MAX_LINE / 2 + 1]; // parsed out command line arguments
  int should_run = 1;           /* flag to determine when to exit program */
  // Add additional variables for the implementation.   


  while (should_run)  {
    printf("osh>");
    fflush(stdout);
    pid_t pid;        
    // Read the input command
    fgets(command, MAX_LINE, stdin); //reads input line and puts it into command. 

    command[strcspn(command, "\n")] = 0; //Remove the newline character

    // Parse the input command
    if (strcmp(command,"exit") == 0){ //check if input is exit
      should_run = 0; //ends the loop of the program
    } 

    strcpy(hist2, command); //Adds the input command into the history 

    if (strcmp(command,"!!") == 0) { //checks if input is to show history !!
        processHistory(args);
    } else {
      num_args = parse_command(command, args); //if input is not for history then just call the parse_command.
    }

    if (isGreater == true){ // Verify > in the command then 
      porcessGreater(args);
    }else  if (isLess == true){ // Verify < in the command then 
      processLess(args);
    }else if (isAnd == true) { //Verify isAnd then set the argument as Null
      processAnd(args);
    }
    pid = fork(); 
    processFork(pid, args);
  }
  /*    // create the pipe 
   if (pipe(fd) == -1) {
     fprintf(stderr,"Pipe failed");
     return 1;
   }
 
   // fork a child process 
   pid = fork();
 
   if (pid < 0) { // error occurred 
     fprintf(stderr, "Fork Failed");
     return 1;
   }
 
   if (pid > 0) { // parent process 
     // close the unused end of the pipe 
     close(fd[READ_END]);
 
     // write to the pipe 
     write(fd[WRITE_END], write_msg, strlen(write_msg)+1);
 
     // close the write end of the pipe 
     close(fd[WRITE_END]);
   }
   else { // child process 
     // close the unused end of the pipe 
     close(fd[WRITE_END]);
 
     // read from the pipe 
     read(fd[READ_END], read_msg, BUFFER_SIZE);
     printf("read %s",read_msg);
 
     // close the read end of the pipe 
     close(fd[READ_END]);
   }*/
  return 0;
}
