#define _GNU_SOURCE
#define DEBUG
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
		/*
		printf("To quit shell please use ctrl+d\n");
		printf("Terminating\n");
		*/
		running = 0;
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
  int bufsize = 128, position = 0;
  char **tokens = (char**)malloc(bufsize * sizeof(char*));
  char *token = NULL;
  //printf("%s\n",line);
  token = strtok(line, delim);
  while (token != NULL)
  {
    tokens[position] = token;
    position++;
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
	int backgroundProcess = 0; //by default set this flag to false
	//Create table with types
	int* tokenType = (int*)malloc(tokenCount * sizeof(int));
	if(tokenType==NULL)
	{
		fprintf(stderr,"tokenType memorry error\n");
		return;
	}
	//counts how pipes are in command to execute
	int howManyPipes=0;
	//flag to check if >> in command
	int isRedirect=0;
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
			howManyPipes++;
			tokenType[i] = 1;
		}
		//>>
		else if(tokenLength == 2 && command[i][0] == '>' && command[i][1] == '>')
		{
			#ifdef DEBUG
			printf("Command: %s is >>\n",command[i]);
			#endif
			if(isRedirect==1)
			{
				fprintf(stderr,"Too many redirects in command.Aborting...\n");
				return;
			}
			else
			{
				isRedirect=1;
			}
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
	}
	#ifdef DEBUG
	printf("%d pipes received\n",howManyPipes);
	#endif
	int* fds = (int*)malloc((howManyPipes+isRedirect)*sizeof(int));
	pid_t* pid = (pid_t*)malloc((howManyPipes+isRedirect)*sizeof(pid_t));
	int tempToCreatePipes=0;
	for(tempToCreatePipes=0;tempToCreatePipes<howManyPipes+isRedirect;tempToCreatePipes++)
	{
		pipe(fds+2*tempToCreatePipes);
	}
	//Exec to stdout or redirect
	if(howManyPipes == 0)
	{
		//ExecToFile
		if(isRedirect == 1)
		{
		}
		//ExecToStdout
		else
		{
		}
	}
	//Atleast one pipe
	//TODO: error handling
	else
	{
		pid[0] = fork();
		//First child
		if(pid[0] == 0)
		{
			dup2(fds[1],1);
			for(int i=0;i<2*(howManyPipes+isRedirect);i++)
			{
				close(fds[i]);
			}
			char* catArgs[] = {"ls",NULL};
			int execvpResult = execvp(*catArgs, catArgs);
		}
		//Middle childs
		else
		{
			//Loop through middle childs
			for(int i=0;i<howManyPipes+isRedirect-1;i++)
			{
				pid[i] = fork();
				if(pid[i] == 0)
				{
					dup2(fds[2*i],0);
					dup2(fds[2*(i+1)+1],1);
					//Close all pipes
					for(int i=0;i<2*(howManyPipes+isRedirect);i++)
					{
						close(fds[i]);
					}
					char* catArgs[] = {"grep","main",NULL};
					int execvpResult = execvp(*catArgs, catArgs);
				}
			}
		}
		//Last child
		pid[howManyPipes+isRedirect-1] = fork();
		if(pid[howManyPipes+isRedirect-1] == 0)
		{
			#ifdef DEBUG
			printf("Executing last child...\n");
			#endif
			//TODO: Set correct fds[]
			dup2(fds[2],0);
			for(int i=0;i<2*(howManyPipes+isRedirect);i++)
			{
				close(fds[i]);
			}
			char* catArgs[] = {"wc",NULL};
			int execvpResult = execvp(*catArgs, catArgs);
		}
		//Parent process
		else
		{
			for(int i=0;i<2*(howManyPipes+isRedirect);i++)
			{
				close(fds[i]);
			}
			for(int i=0;i<2*(howManyPipes+isRedirect);i++)
			{
				waitpid(pid[i], NULL, 0);
			}
		}
	}
	free(tokenType);
}

int main(int argc, char* argv[])
{
	#ifdef DEBUG
	printf("Using shell in debug mode!!\n");
	#endif
	if(signal(SIGQUIT, handler) == SIG_ERR)
	{
		printf("Can't catch SIGQUIT\n");
	}
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
		/*size_t length = strlen(line);
		if((length > 0) && (line[length-1] == '\n'))
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
#ifdef DEBUG
  printf("Checking if script was launched from different file...\n");
#endif
if(argc==1)
{
while(running)
{
	#ifdef DEBUG
	printf("BEGINNING SHELL LOOP\n");
	#endif
	printCommandPrompt();
	char* userResponse = readLineFromCommandPrompt();
	//EOF character
	if(userResponse == NULL)
	{
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
	if(currentCommand[0] != '\0' && currentCommand[0] != '\n')
		push(&q, currentCommand);
	 free(tokens);
	 free(userResponse);
  }
}
else
{
  #ifdef DEBUG
	printf("Shell launched from bash file!\n");
	#endif
  FILE *fileFromBash;
	char *fileFromBashLine = NULL;
	size_t fileFromBashLen = 0;
	ssize_t fileFromBashRead;

  #ifdef DEBUG
	printf("Path to bash file: %s\n",argv[1]);
	#endif
	fileFromBash = fopen(argv[1], "r+");
	if(fileFromBash == NULL)
	{
		perror(argv[0]);
		exit(EXIT_FAILURE);
	}
  #ifdef DEBUG
	printf("Opened bash log!\n");
	#endif
  //omit file header
  fileFromBashRead = getline(&fileFromBashLine, &fileFromBashLen, fileFromBash);
  free(fileFromBashLine);
  fileFromBashLine=NULL;
  fileFromBashLen=0;
	while ((fileFromBashRead = getline(&fileFromBashLine, &fileFromBashLen, fileFromBash)) > 0)
	{

		#ifdef DEBUG
		printf("Line from bash file: %s\n", fileFromBashLine);
		#endif

    char* bashLine = fileFromBashLine;
  	//EOF character
  	if(bashLine == NULL)
  	{
  		break;
  	}
  	char* currentBashCommand=malloc(strlen(bashLine)+1);
  	strcpy(currentBashCommand,bashLine);
  	int bashTokenCount = 0;
  	char** bashTokens = getTokens(bashLine, &bashTokenCount);
  	if(bashTokens == NULL)
  	{
  		fprintf(stderr, "getTokens() failed\n");
  		return 2;
  	}
  	//TODO: FIX THIS
  	#ifdef DEBUG
  	int i;
  	printf("Tokens get: ");
  	for(i=0;i<bashTokenCount;i++)
  		printf("%s ",bashTokens[i]);
  	printf("\n");
  	#endif
  	//Execute
  	execute(bashTokens, bashTokenCount);
  	#ifdef DEBUG
  	printf("AFTER EXECUTE() FUNCTION\n");
  	#endif
  	//add command to history queue
  	if(current_queue_size(&q)==20)
  	{
  	  //delete old history
  	  pop(&q);
  	}
  	  push(&q, currentBashCommand);
  	 free(bashTokens);
    free(fileFromBashLine);
    fileFromBashLine=NULL;
    fileFromBashLen=0;

  }

}
#ifdef DEBUG
printf("After main loop\n");
#endif
fp = fopen(pathToShellLogFile, "w+");
if(fp == NULL)
{
	perror(NULL);
	exit(EXIT_FAILURE);
}
#ifdef DEBUG
printf("Current queue: \n");
#endif
while(front(&q)!=NULL)
{
	fprintf(fp,"%s",front(&q));
	#ifdef DEBUG
	printf("%s",front(&q));
	#endif
	 pop(&q);
}
fclose(fp);
return 0;
}
