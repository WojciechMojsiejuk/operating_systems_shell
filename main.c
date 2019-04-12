#define _GNU_SOURCE
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <pwd.h>
#include "queue.h"


volatile sig_atomic_t running = 1;
struct Queue q;

void handler(int signum)
{
	if(signum == SIGQUIT)
	{
		running = 0;
	}
	exit(signum);
}

void printCommandPrompt()
{
  //buffor do przechowywania ścieżki w której obecnie jest użytkownik
  char cwd[256];
  //wyświetlenie ścieżki użytkownika
  if (getcwd(cwd, sizeof(cwd)) == NULL)
      perror("getcwd() error");
  else
      printf("%s#", cwd);
}

//TODO: Finish this function
//Execvp(with fork), redirects output to other execvp
void execWithRedirect(char** bufor, char** bufor2, int backgroundProcess)
{
	int fds[2];
	pid_t pid;
	/* Create a pipe. File descriptors for the two ends of the pipe are placed in fds. */
	int pipeResult = pipe (fds);
	if(pipeResult == -1)
	{
		printf("Pipe failed\n");
		return;
	}
	/* Fork a child process. */
	int i;
	for(i=0;i<2;i++)
	{
		pid = fork();
		/* This is the child process. Close our copy of the write end of the file descriptor. */
		if (pid == (pid_t) 0)
		{
			close (fds[1]);
			/* Connect the read end of the pipe to standard input. */
			dup2 (fds[0], STDIN_FILENO);
			/* Replace the child process with our program. */
			int execvpResult;
			if(i==0)
				execvpResult = execvp (bufor[0], bufor);
			else
				execvpResult = execvp(bufor2[0], bufor2);
			if(execvpResult == -1)
			{
				perror("execvp failed");
				return;
			}
		}
		//fork error handling
		else if(pid < 0)
		{
			printf("Fork failed");
			return;
		}
		/* This is the parent process. */
		else
		{
			//FILE* stream;
			/* Close our copy of the read end of the file descriptor. */
			close (fds[0]);
			// /* Convert the write file descriptor to a FILE object, and write to it. */
			close (fds[1]);
			/* Wait for the child process to finish (unless there was a & character)*/
			if(!backgroundProcess)
				waitpid (pid, NULL, 0);
		}
	}
	return;
}

//TODO: Clear this function and its calls
/*
void tempExecRedirect()
{
	char bufor[2][] = {"ls", "ls"};
	int fds[2];
	pid_t pid;
	// Create a pipe. File descriptors for the two ends of the pipe are placed in fds.
	int pipeResult = pipe (fds);
	if(pipeResult == -1)
	{
		printf("Pipe failed\n");
		return;
	}
	//Fork a child process.
	int i;
	for(i=0;i<2;i++)
	{
		pid = fork();
		// This is the child process. Close our copy of the write end of the file descriptor.
		if (pid == (pid_t) 0)
		{
			close (fds[1]);
			// Connect the read end of the pipe to standard input.
			dup2 (fds[0], STDIN_FILENO);
			// Replace the child process with our program.
			int execvpResult;
			if(i==0)
				execvpResult = execvp (bufor[0], bufor);
			else
				execvpResult = execvp(bufor2[0], bufor2);
			if(execvpResult == -1)
			{
				perror("execvp failed");
				return;
			}
		}
		//fork error handling
		else if(pid < 0)
		{
			printf("Fork failed");
			return;
		}
		// This is the parent process.
		else
		{
			//FILE* stream;
			// Close our copy of the read end of the file descriptor.
			close (fds[0]);
			// Convert the write file descriptor to a FILE object, and write to it.
			close (fds[1]);
			// Wait for the child process to finish (unless there was a & character)
			if(!backgroundProcess)
				waitpid (pid, NULL, 0);
		}
	}
	return;
}
*/

//Execvp (with fork), prints output to stdout
void execToStdout(char** bufor, int backgroundProcess)
{
	pid_t pid;
	/* Fork a child process. */
	pid = fork();
	//This is the child process.
	if (pid == (pid_t) 0)
	{
		/* Replace the child process with our program. */
		int execvpResult = execvp (bufor[0], bufor);
		if(execvpResult == -1)
		{
			perror("execvp failed");
			return;
		}
	}
	//fork error handling
	else if(pid < 0)
	{
		printf("Fork failed");
		return;
	}
	/* This is the parent process. */
	else
	{
		if(!backgroundProcess)
			waitpid (pid, NULL, 0);
	}
	return;
}

//Execvp (with fork), prints output to file
void execToFile(char** bufor, char* fileName, int backgroundProcess)
{
	int fds[2];
	pid_t pid;
	/* Create a pipe. File descriptors for the two ends of the pipe are placed in fds. */
	int pipeResult = pipe (fds);
	if(pipeResult == -1)
	{
		printf("Pipe failed\n");
		return;
	}
	/* Fork a child process. */
	pid = fork();
	/* This is the child process. Close our copy of the write end of the file descriptor. */
	if (pid == (pid_t) 0)
	{
		int fd = open(fileName, O_WRONLY | O_CREAT, 0777);
		if(fd == -1)
		{
			perror(fileName);
			return;
		}
		dup2(fd, 1);
		close (fds[1]);
		/* Connect the read end of the pipe to standard input. */
		dup2 (fds[0], STDIN_FILENO);
		/* Replace the child process with our program. */
		int execvpResult = execvp (bufor[0], bufor);
		if(execvpResult == -1)
		{
			perror("execvp failed");
			return;
		}
}
	//fork error handling
	else if(pid < 0)
	{
		printf("Fork failed");
		return;
	}
	/* This is the parent process. */
	else
	{
		//FILE* stream;
		/* Close our copy of the read end of the file descriptor. */
		close (fds[0]);
		// /* Convert the write file descriptor to a FILE object, and write to it. */
		// stream = fdopen (fds[1], "w");
		// fprintf (stream, "This is a test.\n");
		// fprintf (stream, "Hello, world.\n");
		// fprintf (stream, "My dog has fleas.\n");
		// fprintf (stream, "This program is great.\n");
		// fprintf (stream, "One fish, two fish.\n");
		// fflush (stream);
		close (fds[1]);
		/* Wait for the child process to finish (unless there was a & character)*/
		if(!backgroundProcess)
			waitpid (pid, NULL, 0);
    }
	return;
}

//Reads line from stdin
//Note: Function returns dynamically allocated line - it needs to be freed
//Return value: returns NULL on failure
char* readLineFromCommandPrompt()
{
  ssize_t read;
  size_t len = 0;
  char *line = NULL;
	//getline allocates the buffor (line*), it should be freed even if getline failed
  read = getline(&line, &len, stdin);
  if( read == -1)
    {
        perror(NULL);
	return NULL;
    }
	return line;
}

//Splits line by space character
//Note: Returns pointer that needs to be freed
//Return value: returns NULL on failure
char** getTokens(char* line, int *tokenCount)
{
	char* delim = " \t\r\n\a";
  int bufsize = 64, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token = NULL;
  //printf("%s\n",line);
  token = strtok(line, delim);
  while (token != NULL)
  {
    tokens[position] = token;
    position++;
	//TODO: Reallocate memory here
	if(position >= bufsize)
	{
		fprintf(stderr, "getTokens: position out of range\n");
		return NULL;
	}
	token = strtok(NULL, delim);
  }
	//Set correct tokenCount
	*tokenCount = position;
	////Add NULL value at t
	//position++;
	//tokens[position] = NULL;
//  int i=0;
//  for(;i<position;i++)
//  {
//    printf("%s ",tokens[i]);
//  }
  return tokens;
}

void execute(char** command, int tokenCount)
{
	int i;
	//Create table with types
	int* tokenType = (int*)malloc(tokenCount * sizeof(int));
	//types:
	//pipe = 1, redirect = 2, parameter = 3, command = 4, backgroundProcess = 5
	//TODO: check type of token (-, >>, | etc.) and do action
	for(i=0;i<tokenCount;i++)
	{
		//Reset type
		tokenType[i] = 0;
		int tokenLength = strlen(command[i]);
		//background process
		if(i == tokenCount -1 && tokenLength == 1 && command[i][0] == '&')
		{
			tokenType[i] = 5;
		}
		//Pipe
		else if(tokenLength == 1 && command[i][0] == '|')
		{
			tokenType[i] = 1;
		}
		//>>
		else if(tokenLength == 2 && command[i][0] == '>' && command[i][1] == '>')
		{
			tokenType[i] = 2;
		}
		//command
		//TODO:check if previous was pipe or it is first token
		else if(i == 0 || tokenType[i-1] == 1)
		{
			tokenType[i] = 4;
		}
		//Parameter (does not have to start with -, example cat a.txt)
		else
		{
			tokenType[i] = 3;
		}
		////Print type
		//printf("%d\n", tokenType[i]);
	}
	//Check if there's a pipe
	/*for(i=0;i<tokenCount;i++)
	{
		if(tokenType[i] == 1)
		{
			printf("Our shell does not support '|' yet\n");
			free(tokenType);
			return;
		}
	}*/
	//Check if there's a redirect
	/*
	for(i=0;i<tokenCount;i++)
	{
		if(tokenType[i] == 2)
		{
			printf("Our shell does not support '>>' yet\n");
			free(tokenType);
			return;
		}
	}*/
  char** first_buffer = malloc(tokenCount * sizeof(char*));
  char** second_buffer = malloc(tokenCount * sizeof(char*));
  int is_pipe=0; //by default set this flag to false
  int is_redirect=0;  //by default set this flag to false
  int is_background_process=0; //by default set this flag to false
  int j=0; //first_buffer's index
  int k=0; //second_buffer's index
  for (i = 0; i < tokenCount; i++)
  {
    //fill buffers
    if (tokenType[i]==3 || tokenType[i]==4)
    {
      if(is_pipe==0 && is_redirect==0)
      {
        first_buffer[j] = command[i];
        j++;
      }
      else
      {
        second_buffer[k]=command[i];
        k++;
      }
    }
    //if current token is |
      if(tokenType[i]==1)
      {
        //CASE: a|b|
        if(is_pipe==1)
        {
      		//wywolaj funkcje bo doszlismy do drugiego pipe'a
      		//DEBUG CODE
      		int l;
      		printf("DEBUG REDIRECT\n");
      		for(l=0;l<tokenCount;l++)
      		{
      			printf("1: %s", first_buffer[l]);
      			printf("2: %s", second_buffer[l]);
      		}
      		//DEBUG END
      		execWithRedirect(first_buffer,second_buffer,0);
        }
        //CASE: a|B>>
        if(is_redirect==2)
        {
          execToFile(first_buffer, second_buffer[0], 0);
        }
        //set flag to true, because pipe was found in command
        is_pipe=1;
      }
      //if current token is >>
      else if(tokenType[i]==2)
      {
        //CASE: a>>b|
        //CZY POWINIEN WGL BYC UWZGLEDNIANY?
              if(is_pipe==1)
              {
      		//DEBUG CODE
          int l;
      		printf("DEBUG REDIRECT\n");
      		for(l=0;l<tokenCount;l++)
      		{
      			printf("1: %s", first_buffer[l]);
      			printf("2: %s", second_buffer[l]);
      		}
      		//DEBUG END
      		//wywolaj funkcje bo doszlismy do drugiego pipe'a
      		execWithRedirect(first_buffer,second_buffer,0);
              }
              if(is_redirect==2)
              {
                execToFile(first_buffer, second_buffer[0], 0);
              }
              is_redirect=2;
      }
      //if we reached end of command
	else if(tokenType[i] == 4 && i == tokenCount-1)
	{
          //CASE: a|b
        	if(is_pipe==1)
        	{
      			//DEBUG CODE
      			int l;
      			printf("DEBUG REDIRECT\n");
      			for(l=0;l<tokenCount;l++)
      			{
      				printf("1: %s", first_buffer[l]);
      				printf("2: %s", second_buffer[l]);
      			}
      			//DEBUG END
      			//wywolaj funkcje bo doszlismy do drugiego pipe'a
      			execWithRedirect(first_buffer,second_buffer,0);
        	}
          //CASE a>>b
		if(is_redirect==2)
		{
		  execToFile(first_buffer, second_buffer[0], 0);
		}
    /*
    SUGGESTION:
    CASE: a
    execToStdout(first_buffer,0);
    */
	}
      //pipe = 1, redirect = 2, parameter = 3, command = 4, backgroundProcess = 5
    	//TODO: check type of token (-, >>, | etc.) and do action

  }
	execToStdout(first_buffer, 0);
  int freeMemoryIndex;
  for(freeMemoryIndex=0;freeMemoryIndex<tokenCount;freeMemoryIndex++)
  {
    free(first_buffer[freeMemoryIndex]);
    free(second_buffer[freeMemoryIndex]);
  }
  free(first_buffer);
  free(second_buffer);
  //printf("PATH : %s\n", getenv("PATH"));
	free(tokenType);
}

int main()
{
  //initialize queue
  init(&q);

  //get user home directory
  const char *homedir;
  if ((homedir = getenv("HOME")) == NULL)
  {
    homedir = getpwuid(getuid())->pw_dir;
  }
  printf("HOME: %s \n",homedir);

  //read log file
  FILE *fp;
  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  char* shellLogName = "/shell_log";
  char* pathToShellLogFile=malloc(strlen(homedir)+strlen(shellLogName)+1);
  strcpy(pathToShellLogFile,homedir);
  strcat(pathToShellLogFile,shellLogName);
  printf("Path to log file: %s\n",pathToShellLogFile);
  fp = fopen(pathToShellLogFile, "r+");
    if (fp == NULL)
    {
      perror(pathToShellLogFile);
      printf("Creating file log...\n");
      fp=fopen(pathToShellLogFile, "w+");
      if(fp == NULL)
      {
        perror(pathToShellLogFile);
        exit(EXIT_FAILURE);
      }
    }
  printf("Opened file log\n");
         //CZY TO OK?


  while ((read = getline(&line, &len, fp)) > 0)
  {
      //initialize queue with values from log file
      size_t length = strlen(line);
       if((length > 0) && (line[length-1] == '\n'))
       {
           line[length-1] ='\0';
       }
      push(&q,line);
      printf("Line from file: %s Value in queue:", line);
      printf("%s",front(&q));
      printf("\n Queue size: %d",current_queue_size(&q));
      free(line);
      line=NULL;
      len=0;
  }
  printf("OK działa\n");
  if(read == -1)
    {
      free(line);
    }
  fclose(fp);
	signal(SIGQUIT, handler);
  while(1)
  {
	printCommandPrompt();
	char* userResponse = readLineFromCommandPrompt();
	if(userResponse == NULL)
	{
		//fprintf(stderr, "readLineFromCommandPrompt() failed\n");
		//printf("EOF?\n");
		break;
	}
  char* currentCommand=malloc(strlen(userResponse)+1);
  strcpy(currentCommand,userResponse);
	int tokenCount = 0;
	char** tokens = getTokens(userResponse, &tokenCount);
	if(tokens == NULL)
	{
		fprintf(stderr, "getTokens() failed\n");
		return 2;
	}
//TODO: FIX THIS
 int i;
  printf("Otrzymane tokeny: ");
  for(i=0;i<tokenCount;i++)
    printf("%s ",tokens[i]);
  printf("\n");

	//Execute
	execute(tokens, tokenCount);
  //add command to history queue
  if(current_queue_size(&q)==20)
  {
    //delete old history
    pop(&q);
  }
  printf("CZY DZIAŁA?");
  printf("%s",currentCommand);
  printf("TY KURWO JEBANA ZMARNOWALAS MI 20 lat zycia");
  push(&q, currentCommand);
  printf("NIE DZIAŁA");
	//Free allocated memory
  //HELP: https://stackoverflow.com/questions/13148119/what-does-pointer-being-freed-was-not-allocated-mean-exactly
  // int freeTokensIndex;
  // for(freeTokensIndex=0;freeTokensIndex<tokenCount;freeTokensIndex++)
  // {
  //   free(tokens[freeTokensIndex]);
  // }
	free(tokens);
	free(userResponse);
  free(currentCommand);

  }

  fp = fopen(pathToShellLogFile, "a");
    if (fp == NULL)
        exit(EXIT_FAILURE); //CZY TO OK?*/

  while(front(&q)!=NULL)
  {
    fprintf(fp,"%s",front(&q));
    printf("%s",front(&q));
    pop(&q);
  }
  fclose(fp);
	printf("Jestem tu\n");
  return 0;
}
