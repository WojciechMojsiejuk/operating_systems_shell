#define _GNU_SOURCE
//#define DEBUG
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <pwd.h>
#include <signal.h>
#include "queue.h"


volatile sig_atomic_t running = 1;
struct Queue q;

void handler(int signum)
{
	if(signum == SIGINT)
	{
		running = 0;
		signal(signum, handler);
	}
	//exit(signum);
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
void execWithRedirect(char** bufor, int buferSize, char** bufor2, int bufer2Size, int backgroundProcess)
{
	#ifdef DEBUG
	printf("In execWithRedirect() function\n");
	printf("\tbuferSize: %d\n", buferSize);
	printf("\tbufer2Size: %d\n", bufer2Size);
	printf("\texecWithRedirect() - printing bufor tokens from parameter\n");
	printf("\tBufer 1: ");
	int testIter;
	for(testIter=0;testIter<buferSize;testIter++)
	{
		if(bufor[testIter]==NULL)
			printf("NULL ");
		else
			printf("%s ", bufor[testIter]);
	}
	printf("\n");
	printf("\t Bufer 2: ");
	for(testIter=0;testIter<bufer2Size;testIter++)
	{
		if(bufor2[testIter]==NULL)
			printf("NULL ");
		else
			printf("%s ", bufor2[testIter]);
	}
	printf("\n");
	#endif
	if(buferSize <=0 || bufer2Size <=0)
	{
		printf("Invalid command\n");
		return;
	}
	int fds[2];
	pid_t pid, pid2;
	/* Create a pipe. File descriptors for the two ends of the pipe are placed in fds. */
	int pipeResult = pipe(fds);
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
		/* Connect the read end of the pipe to standard input. */
		dup2 (fds[1], STDOUT_FILENO);
		close(fds[0]);
		close (fds[1]);
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
		pid2 = fork();
		//Child 2 - Success fork
		if (pid2 == (pid_t) 0)
		{
			close(fds[1]);
			dup2(fds[0], STDIN_FILENO);
			close(fds[0]);
			int execvp2Result = execvp(bufor2[0], bufor2);
			if(execvp2Result == -1)
			{
				perror("execvp2 failed");
				return;
			}
		}
		else if(pid2 < 0)
		{
			printf("Fork2 failed");
			return;
		}
		//Parent
		else
		{
			/* Close our copy of the read end of the file descriptor. */
			close (fds[0]);
		/* Convert the write file descriptor to a FILE object, and write to it. */
			close (fds[1]);
			/* Wait for the child process to finish (unless there was a & character)*/
			waitpid(pid, NULL, 0);
			if(!backgroundProcess)
			{
				waitpid(pid2, NULL, 0);
			}
		}
	}
	return;
}

//Execvp (with fork), prints output to stdout
void execToStdout(char** bufor,int bufferSize, int backgroundProcess)
{	
/*
	printf("%d", bufferSize);
  char** restrictedBuffer=(char**)malloc((bufferSize+1)*sizeof(char*));
  int i=0;
  for(;i<bufferSize;i++)
  {
    restrictedBuffer[i]=bufor[i];
	//strcpy(restrictedBuffer[i],bufor[i]);
  }
  // strcpy(restrictedBuffer[i],bufor[i]);
  restrictedBuffer[i+1]=NULL;
	printf("ALL EXECVP ARGS: \n");*/
	#ifdef DEBUG
	printf("In execToStdout() function:\n");
	printf("\tbuferSize: %d\n", bufferSize);
	printf("\texecToStdout() - printing bufor tokens from parameter\n\t");
	int testIter;
	for(testIter=0;testIter<bufferSize;testIter++)
	{
		if(bufor[testIter]==NULL)
			printf("NULL ");
		else
			printf("%s ", bufor[testIter]);
		//printf("%s ", bufor[tes
		/*if(restrictedBuffer[testIter] == NULL)
			printf("NULL ");
		else
			printf("%s ", restrictedBuffer[testIter]);*/
	}
	printf("\n");
	#endif
	if(bufferSize <=0)
	{
		printf("Invalid command\n");
		return;
	}
	//printf("\nEND\n");
  	//printf("EXEC: %s\n",bufor[0]);
	pid_t pid;
	/* Fork a child process. */
	pid = fork();
	//This is the child process.
	if (pid == (pid_t) 0)
	{
		/* Replace the child process with our program. */
		//int execvpResult = execvp (restrictedBuffer[0], restrictedBuffer);
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
		{
			waitpid(pid, NULL, 0);
		}
	}
	return;
}

//Execvp (with fork), prints output to file
void execToFile(char** bufor, int buferSize, char* fileName, int backgroundProcess)
{
	#ifdef DEBUG
	printf("In execToFile() function\n");
	printf("\tbuferSize: %d\n", buferSize);
	printf("\texecToFile() - printing bufor tokens from parameter\n\t");
	int testIter;
	for(testIter=0;testIter<buferSize;testIter++)
	{
		if(bufor[testIter]==NULL)
			printf("NULL ");
		else
			printf("%s ", bufor[testIter]);
	}
	printf("\n");
	#endif
	if(buferSize <=0)
	{
		printf("Invalid command\n");
		return;
	}
	int fds[2];
	pid_t pid;
	/* Create a pipe. File descriptors for the two ends of the pipe are placed in fds. */
	int pipeResult = pipe(fds);
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
		printf("Fork failed\n");
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
  char **tokens = (char**)malloc(bufsize * sizeof(char*));
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
		fprintf(stderr, "Make sure command is not too long\n");
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
	#ifdef DEBUG
	printf("Command to execute: ");
	int debugCommandIterator = 0;
	for(;debugCommandIterator<tokenCount;debugCommandIterator++)
		printf("%d: %s ",debugCommandIterator, command[debugCommandIterator]);
	printf("\nTokenCount: %d\n",tokenCount);
	#endif
	int i;
	int backgroundProcess = 0;
	//Create table with types
	int* tokenType = (int*)malloc(tokenCount * sizeof(int));
	if(tokenType==NULL)
	{
		fprintf(stderr,"tokenType memorry error\n");
		return;
	}
	//types:
	//pipe = 1, redirect = 2, parameter = 3, command = 4, backgroundProcess = 5
	for(i=0;i<tokenCount;i++)
	{
		//Reset type
		tokenType[i] = 0;
		int tokenLength = strlen(command[i]);
		//background process
		if(i == tokenCount -1 && tokenLength == 1 && command[i][0] == '&')
		{
			#ifdef DEBUG
			printf("Command: %s is &\n",command[i]);
			#endif			
			tokenType[i] = 5;
			backgroundProcess = 1;
		}
		//Pipe
		else if(tokenLength == 1 && command[i][0] == '|')
		{
			#ifdef DEBUG
			printf("Command: %s is |\n",command[i]);
			#endif
			tokenType[i] = 1;
		}
		//>>
		else if(tokenLength == 2 && command[i][0] == '>' && command[i][1] == '>')
		{
			#ifdef DEBUG
			printf("Command: %s is >>\n",command[i]);
			#endif
			tokenType[i] = 2;
		}
		//command
		else if(i == 0 || tokenType[i-1] == 1)
		{
			#ifdef DEBUG
			printf("Command: %s is a shell command\n",command[i]);
			#endif
			tokenType[i] = 4;
		}
		//Parameter (does not have to start with -, example cat a.txt)
		else
		{
			#ifdef DEBUG
			printf("Command: %s is a command parameter\n",command[i]);
			#endif
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
	/*
	char** first_buffer = (char**)malloc(tokenCount * sizeof(char*));
	char** second_buffer = (char**)malloc(tokenCount * sizeof(char*));
	*/
	char** first_buffer = calloc(tokenCount, sizeof(char*));
	char** second_buffer = calloc(tokenCount, sizeof(char*));
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
				#ifdef DEBUG
				printf("First buffer filled with: %s on index: %d\n",command[i],j);
				#endif
				first_buffer[j] = command[i];
				j++;
	      		}
			else
			{
				#ifdef DEBUG
				printf("Second buffer filled with: %s on index: %d\n",command[i],k);
				#endif
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
		      		//DEBUG CODE
				/*
		      		int l;
		      		printf("DEBUG REDIRECT\n");
		      		for(l=0;l<tokenCount;l++)
		      		{
		      			printf("1: %s", first_buffer[l]);
		      			printf("2: %s", second_buffer[l]);
		      		}
				*/
		      		//DEBUG END
				//TODO: Set correct buffors size
				//Call execWithRedirect function because we reached another pipe
				#ifdef DEBUG
				printf("First buffer:");
				int debugExecWithRedirectIterator;
				for(debugExecWithRedirectIterator=0;debugExecWithRedirectIterator<j;debugExecWithRedirectIterator++)
					printf("%s ",first_buffer[debugExecWithRedirectIterator]);
				printf("\nSecond buffer:");			
				for(debugExecWithRedirectIterator=0;debugExecWithRedirectIterator<k;debugExecWithRedirectIterator++)
					printf("%s ",second_buffer[debugExecWithRedirectIterator]);
				printf("\nCalling function execWithRedirect...\n");
				#endif


	      			execWithRedirect(first_buffer, j, second_buffer, k, backgroundProcess);


				int cleaningBufferIndex;
				//int swapBufferIterator;			
				#ifdef DEBUG
				printf("Swapping buffers...\n");
				#endif
				
				//Clear first buffer
				for(cleaningBufferIndex=0;cleaningBufferIndex<tokenCount;cleaningBufferIndex++)
					first_buffer[cleaningBufferIndex]=NULL;
				//Swaping all content from second buffer to the first one
/*				for(swapBufferIterator=0;second_buffer[swapBufferIterator]!=NULL;swapBufferIterator++)
				{					
					first_buffer[swapBufferIterator]=second_buffer[swapBufferIterator];
				}
*/			
				//Clear second buffer
			
				for(cleaningBufferIndex=0;cleaningBufferIndex<tokenCount;cleaningBufferIndex++)
					second_buffer[cleaningBufferIndex]=NULL;
				free(second_buffer);			
				//Set the index of first buffer onto value of second's
				j=k;
				//reset index of second buffer
				k=0;
				#ifdef DEBUG
				printf("Checking buffer swapping...\nFirst buffer:");
				int debugSwapBufferIterator;
				for(debugSwapBufferIterator=0;debugSwapBufferIterator<tokenCount;debugSwapBufferIterator++)
					printf("%s ",first_buffer[debugSwapBufferIterator]);
				printf("\nSecond buffer:");			
				for(debugSwapBufferIterator=0;debugSwapBufferIterator<tokenCount;debugSwapBufferIterator++)
					printf("%s ",second_buffer[debugSwapBufferIterator]);
				printf("\nEnd execWithRedirect case...\n");
				#endif
			}
			//CASE: a>>B|
			if(is_redirect==2)
			{	
				//Call execWithFile function because we reached pipe
				#ifdef DEBUG
				printf("First buffer:");
				int debugExecWithRedirectIterator;
				for(debugExecWithRedirectIterator=0;debugExecWithRedirectIterator<j;debugExecWithRedirectIterator++)
					printf("%s ",first_buffer[debugExecWithRedirectIterator]);
				printf("\nSecond buffer:");			
				for(debugExecWithRedirectIterator=0;debugExecWithRedirectIterator<k;debugExecWithRedirectIterator++)
					printf("%s ",second_buffer[debugExecWithRedirectIterator]);
				printf("\nCalling function execWithFile...\n");
				#endif


				//TODO: set valid tokenSize
		  		execToFile(first_buffer, j, second_buffer[0], backgroundProcess);


				int cleaningBufferIndex;
				//int swapBufferIterator;			
				#ifdef DEBUG
				printf("Swapping buffers...\n");
				#endif
				//Clear first buffer
				for(cleaningBufferIndex=0;cleaningBufferIndex<tokenCount;cleaningBufferIndex++)
					first_buffer[cleaningBufferIndex]=NULL;
				//Swaping all content from second buffer to the first one
	/*			for(swapBufferIterator=0;second_buffer[swapBufferIterator]!=NULL;swapBufferIterator++)
				{					
					first_buffer[swapBufferIterator]=second_buffer[swapBufferIterator];
				}
		*/	
				//Clear second buffer
			
				for(cleaningBufferIndex=0;cleaningBufferIndex<tokenCount;cleaningBufferIndex++)
					second_buffer[cleaningBufferIndex]=NULL;
				free(second_buffer);			
				//Set the index of first buffer onto value of second's
				j=k;
				//reset index of second buffer
				k=0;
				#ifdef DEBUG
				printf("Checking buffer swapping...\nFirst buffer:");
				int debugSwapBufferIterator;
				for(debugSwapBufferIterator=0;debugSwapBufferIterator<tokenCount;debugSwapBufferIterator++)
					printf("%s ",first_buffer[debugSwapBufferIterator]);
				printf("\nSecond buffer:");			
				for(debugSwapBufferIterator=0;debugSwapBufferIterator<tokenCount;debugSwapBufferIterator++)
					printf("%s ",second_buffer[debugSwapBufferIterator]);
				printf("\nEnd execWithFile case...\n");
				#endif
        		}
        		//set flag to true, because pipe was found in command
        		is_pipe=1;
      		}
		//if current token is >>
		else if(tokenType[i]==2)
		{
			//CASE: a>>b|
			if(is_pipe==1)
			{
				//DEBUG CODE
				/*
				int l;
				printf("DEBUG REDIRECT\n");
				for(l=0;l<tokenCount;l++)
				{
				printf("1: %s", first_buffer[l]);
				printf("2: %s", second_buffer[l]);
				}
				*/
				//DEBUG END
				//Call execWithRedirect function because we reached pipe
				#ifdef DEBUG
				printf("First buffer:");
				int debugExecWithRedirectIterator;
				for(debugExecWithRedirectIterator=0;debugExecWithRedirectIterator<j;debugExecWithRedirectIterator++)
					printf("%s ",first_buffer[debugExecWithRedirectIterator]);
				printf("\nSecond buffer:");			
				for(debugExecWithRedirectIterator=0;debugExecWithRedirectIterator<k;debugExecWithRedirectIterator++)
					printf("%s ",second_buffer[debugExecWithRedirectIterator]);
				printf("\nCalling function execWithRedirect...\n");
				#endif

				execWithRedirect(first_buffer,j, second_buffer,k, backgroundProcess);


				int cleaningBufferIndex;
				//int swapBufferIterator;			
				#ifdef DEBUG
				printf("Swapping buffers...\n");
				#endif
				//Clear first buffer
				for(cleaningBufferIndex=0;cleaningBufferIndex<tokenCount;cleaningBufferIndex++)
					first_buffer[cleaningBufferIndex]=NULL;
				//Swaping all content from second buffer to the first one
	/*			for(swapBufferIterator=0;second_buffer[swapBufferIterator]!=NULL;swapBufferIterator++)
				{					
					first_buffer[swapBufferIterator]=second_buffer[swapBufferIterator];
				}
	*/		
				//Clear second buffer
			
				for(cleaningBufferIndex=0;cleaningBufferIndex<tokenCount;cleaningBufferIndex++)
					second_buffer[cleaningBufferIndex]=NULL;
				free(second_buffer);			
				//Set the index of first buffer onto value of second's
				j=k;
				//reset index of second buffer
				k=0;
				#ifdef DEBUG
				printf("Checking buffer swapping...\nFirst buffer:");
				int debugSwapBufferIterator;
				for(debugSwapBufferIterator=0;debugSwapBufferIterator<tokenCount;debugSwapBufferIterator++)
					printf("%s ",first_buffer[debugSwapBufferIterator]);
				printf("\nSecond buffer:");			
				for(debugSwapBufferIterator=0;debugSwapBufferIterator<tokenCount;debugSwapBufferIterator++)
					printf("%s ",second_buffer[debugSwapBufferIterator]);
				printf("\nEnd execWithRedirect case...\n");
				#endif
			}
			if(is_redirect==2)
			{

				#ifdef DEBUG
				printf("First buffer:");
				int debugExecWithRedirectIterator;
				for(debugExecWithRedirectIterator=0;debugExecWithRedirectIterator<j;debugExecWithRedirectIterator++)
					printf("%s ",first_buffer[debugExecWithRedirectIterator]);
				printf("\nSecond buffer:");			
				for(debugExecWithRedirectIterator=0;debugExecWithRedirectIterator<k;debugExecWithRedirectIterator++)
					printf("%s ",second_buffer[debugExecWithRedirectIterator]);
				printf("\nCalling function execToFile...\n");
				#endif

				execToFile(first_buffer, j, second_buffer[0], backgroundProcess);


				int cleaningBufferIndex;
				//int swapBufferIterator;			
				#ifdef DEBUG
				printf("Swapping buffers...\n");
				#endif
				//Clear first buffer
				for(cleaningBufferIndex=0;cleaningBufferIndex<tokenCount;cleaningBufferIndex++)
					first_buffer[cleaningBufferIndex]=NULL;
/*
				//Swaping all content from second buffer to the first one
				for(swapBufferIterator=0;second_buffer[swapBufferIterator]!=NULL;swapBufferIterator++)
				{					
					first_buffer[swapBufferIterator]=second_buffer[swapBufferIterator];
				}
*/		
				//Clear second buffer
			
				for(cleaningBufferIndex=0;cleaningBufferIndex<tokenCount;cleaningBufferIndex++)
					second_buffer[cleaningBufferIndex]=NULL;
				free(second_buffer);			
				//Set the index of first buffer onto value of second's
				j=k;
				//reset index of second buffer
				k=0;
				#ifdef DEBUG
				printf("Checking buffer swapping...\nFirst buffer:");
				int debugSwapBufferIterator;
				for(debugSwapBufferIterator=0;debugSwapBufferIterator<tokenCount;debugSwapBufferIterator++)
					printf("%s ",first_buffer[debugSwapBufferIterator]);
				printf("\nSecond buffer:");			
				for(debugSwapBufferIterator=0;debugSwapBufferIterator<tokenCount;debugSwapBufferIterator++)
					printf("%s ",second_buffer[debugSwapBufferIterator]);
				printf("\nEnd execToFile case...\n");
				#endif
			}
		  	is_redirect=2;
		}
      		
  	}
	//if we reached end of command

	//CASE: a|b
        if(is_pipe==1)
	{
		//DEBUG CODE
			/*
		int l;
		printf("DEBUG REDIRECT\n");
		for(l=0;l<tokenCount;l++)
		{
			printf("1: %s", first_buffer[l]);
			printf("2: %s", second_buffer[l]);
		}
			*/
		//DEBUG END
		//wywolaj funkcje bo doszlismy do drugiego pipe'a
			//TODO: Set valid buffers size
		#ifdef DEBUG
		printf("First buffer:");
		int debugExecWithRedirectIterator;
		for(debugExecWithRedirectIterator=0;debugExecWithRedirectIterator<j;debugExecWithRedirectIterator++)
			printf("%s ",first_buffer[debugExecWithRedirectIterator]);
		printf("\nSecond buffer:");			
		for(debugExecWithRedirectIterator=0;debugExecWithRedirectIterator<k;debugExecWithRedirectIterator++)
			printf("%s ",second_buffer[debugExecWithRedirectIterator]);
		printf("\nCalling function execWithRedirect...\n");
		#endif


		execWithRedirect(first_buffer, j, second_buffer, k, backgroundProcess);


		int cleaningBufferIndex;
		//int swapBufferIterator;			
		#ifdef DEBUG
		printf("Swapping buffers...\n");
		#endif
		//Clear first buffer
				for(cleaningBufferIndex=0;cleaningBufferIndex<tokenCount;cleaningBufferIndex++)
					first_buffer[cleaningBufferIndex]=NULL;
/*
				//Swaping all content from second buffer to the first one
				for(swapBufferIterator=0;second_buffer[swapBufferIterator]!=NULL;swapBufferIterator++)
				{					
					first_buffer[swapBufferIterator]=second_buffer[swapBufferIterator];
				}
*/	
		//Clear second buffer
	
		for(cleaningBufferIndex=0;cleaningBufferIndex<tokenCount;cleaningBufferIndex++)
			second_buffer[cleaningBufferIndex]=NULL;
		free(second_buffer);			
		//Set the index of first buffer onto value of second's
		j=k;
		//reset index of second buffer
		k=0;
		#ifdef DEBUG
		printf("Checking buffer swapping...\nFirst buffer:");
		int debugSwapBufferIterator;
		for(debugSwapBufferIterator=0;debugSwapBufferIterator<tokenCount;debugSwapBufferIterator++)
			printf("%s ",first_buffer[debugSwapBufferIterator]);
		printf("\nSecond buffer:");			
		for(debugSwapBufferIterator=0;debugSwapBufferIterator<tokenCount;debugSwapBufferIterator++)
			printf("%s ",second_buffer[debugSwapBufferIterator]);
		printf("\nEnd execWithRedirect case...\n");
		#endif
	}
	//CASE a>>b
	else if(is_redirect==2)
	{
		#ifdef DEBUG
		printf("First buffer:");
		int debugExecWithRedirectIterator;
		for(debugExecWithRedirectIterator=0;debugExecWithRedirectIterator<j;debugExecWithRedirectIterator++)
			printf("%s ",first_buffer[debugExecWithRedirectIterator]);
		printf("\nSecond buffer:");			
		for(debugExecWithRedirectIterator=0;debugExecWithRedirectIterator<k;debugExecWithRedirectIterator++)
			printf("%s ",second_buffer[debugExecWithRedirectIterator]);
		printf("\nCalling function execToFile...\n");
		#endif

		execToFile(first_buffer, j, second_buffer[0], backgroundProcess);

		int cleaningBufferIndex;
		//int swapBufferIterator;			
		#ifdef DEBUG
		printf("Swapping buffers...\n");
		#endif
		//Clear first buffer
		for(cleaningBufferIndex=0;cleaningBufferIndex<tokenCount;cleaningBufferIndex++)
			first_buffer[cleaningBufferIndex]=NULL;
/*
		//Swaping all content from second buffer to the first one
		for(swapBufferIterator=0;second_buffer[swapBufferIterator]!=NULL;swapBufferIterator++)
		{					
			first_buffer[swapBufferIterator]=second_buffer[swapBufferIterator];
		}
*/	
		//Clear second buffer
	
		for(cleaningBufferIndex=0;cleaningBufferIndex<tokenCount;cleaningBufferIndex++)
			second_buffer[cleaningBufferIndex]=NULL;
		free(second_buffer);			
		//Set the index of first buffer onto value of second's
		j=k;
		//reset index of second buffer
		k=0;
		#ifdef DEBUG
		printf("Checking buffer swapping...\nFirst buffer:");
		int debugSwapBufferIterator;
		for(debugSwapBufferIterator=0;debugSwapBufferIterator<tokenCount;debugSwapBufferIterator++)
			printf("%s ",first_buffer[debugSwapBufferIterator]);
		printf("\nSecond buffer:");			
		for(debugSwapBufferIterator=0;debugSwapBufferIterator<tokenCount;debugSwapBufferIterator++)
			printf("%s ",second_buffer[debugSwapBufferIterator]);
		printf("\nEnd execToFile case...\n");
		#endif

	}
	//CASE a
	else
	{
		#ifdef DEBUG
		printf("First buffer:");
		int debugExecWithRedirectIterator;
		for(debugExecWithRedirectIterator=0;debugExecWithRedirectIterator<j;debugExecWithRedirectIterator++)
			printf("%s ",first_buffer[debugExecWithRedirectIterator]);
		printf("\nSecond buffer:");			
		for(debugExecWithRedirectIterator=0;debugExecWithRedirectIterator<k;debugExecWithRedirectIterator++)
			printf("%s ",second_buffer[debugExecWithRedirectIterator]);
		printf("\nCalling function execToStdout...\n");
		#endif

		execToStdout(first_buffer,i,backgroundProcess);	

		int cleaningBufferIndex;
		//int swapBufferIterator;			
		#ifdef DEBUG
		printf("Swapping buffers...\n");
		#endif
		for(cleaningBufferIndex=0;cleaningBufferIndex<tokenCount;cleaningBufferIndex++)
			first_buffer[cleaningBufferIndex]=NULL;
/*
		//Swaping all content from second buffer to the first one
		for(swapBufferIterator=0;second_buffer[swapBufferIterator]!=NULL;swapBufferIterator++)
		{					
			first_buffer[swapBufferIterator]=second_buffer[swapBufferIterator];
		}
	*/
		//Clear second buffer
	
		for(cleaningBufferIndex=0;cleaningBufferIndex<tokenCount;cleaningBufferIndex++)
			second_buffer[cleaningBufferIndex]=NULL;
		free(second_buffer);			
		//Set the index of first buffer onto value of second's
		j=k;
		//reset index of second buffer
		k=0;
		#ifdef DEBUG
		printf("Checking buffer swapping...\nFirst buffer:");
		int debugSwapBufferIterator;
		for(debugSwapBufferIterator=0;debugSwapBufferIterator<tokenCount;debugSwapBufferIterator++)
			printf("%s ",first_buffer[debugSwapBufferIterator]);
		printf("\nSecond buffer:");			
		for(debugSwapBufferIterator=0;debugSwapBufferIterator<tokenCount;debugSwapBufferIterator++)
			printf("%s ",second_buffer[debugSwapBufferIterator]);
		printf("\nEnd execToFile case...\n");
		#endif
	}
	
  	//free(first_buffer);
  	//free(second_buffer);
	free(tokenType);
}

int main()
{
	#ifdef DEBUG
	printf("Using shell in debug mode!!\n");
	#endif
	if(signal(SIGINT, handler) == SIG_ERR)
	{
		printf("Can't catch SIGINT\n");
	}
	//initialize queue
	init(&q);
	//get user home directory
	const char *homedir;
	if((homedir = getenv("HOME")) == NULL)
	{
	homedir = getpwuid(getuid())->pw_dir;
	}
	#ifdef DEBUG
	printf("HOME PATH: %s \n",homedir);
	#endif

	//read log file
	FILE *fp;
	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	char* shellLogName = "/shell_log";
	char* pathToShellLogFile=malloc(strlen(homedir)+strlen(shellLogName)+1);
	strcpy(pathToShellLogFile,homedir);
	strcat(pathToShellLogFile,shellLogName);
	#ifdef DEBUG
	printf("Path to log file: %s\n",pathToShellLogFile);
	#endif
	fp = fopen(pathToShellLogFile, "r+");
	if(fp == NULL)
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
	printf("Opened file log!\n");
	while ((read = getline(&line, &len, fp)) > 0)
	{
		//initialize queue with values from log file
		size_t length = strlen(line);
		/*if((length > 0) && (line[length-1] == '\n'))
		{
			line[length-1] ='\0';
		}*/
		push(&q,line);
		#ifdef DEBUG
		printf("Line from file: %s\n", line);
		printf("Queue front: %s\n",front(&q));
		printf("Queue size: %d\n",current_queue_size(&q));
		#endif
      free(line);
      line=NULL;
      len=0;
}
if(read == -1)
{
	free(line);
}
fclose(fp);
while(running)
{
	#ifdef DEBUG
	printf("BEGINNING SHELL LOOP\n");
	#endif
	printCommandPrompt();
	char* userResponse = readLineFromCommandPrompt();
	//EOF character from console
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
	#ifdef DEBUG
	int i;
	printf("Tokens get: ");
	for(i=0;i<tokenCount;i++)
		printf("%s ",tokens[i]);
	printf("\n");
	#endif
	//Execute
	execute(tokens, tokenCount);
	#ifdef DEBUG
	printf("AFTER EXECUTE() FUNCTION\n");
	#endif
	//add command to history queue
	if(current_queue_size(&q)==20)
	{
	  //delete old history
	  pop(&q);
	}
	//If string empty or first char is \n then we do not want to add it to history
	if(currentCommand != NULL)
	{
		if(currentCommand[0] != '\0' && currentCommand[0] != '\n')
			push(&q, currentCommand);
	}
	  // printf("NIE DZIAŁA");
		// //Free allocated memory
	  // //HELP: https://stackoverflow.com/questions/13148119/what-does-pointer-being-freed-was-not-allocated-mean-exactly
	  // // int freeTokensIndex;
	  // // for(freeTokensIndex=0;freeTokensIndex<tokenCount;freeTokensIndex++)
	  // // {
	  // //   free(tokens[freeTokensIndex]);
	  // // }
	 free(tokens);
	 free(userResponse);
	// free(currentCommand);

}
printf("Terminating shell\n");
#ifdef DEBUG
printf("After main loop\n");
#endif
fp = fopen(pathToShellLogFile, "w+");
if(fp == NULL)
{
	perror(NULL);
	exit(EXIT_FAILURE); //CZY TO OK?*/
}
//#ifdef DEBUG
printf("Log history: \n");
//#endif
while(front(&q)!=NULL)
{
	fprintf(fp,"%s",front(&q));
	//#ifdef DEBUG
	printf("%s",front(&q));
	//#endif
	pop(&q);
}
fclose(fp);
return 0;
}
