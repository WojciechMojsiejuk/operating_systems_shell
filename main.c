#define _GNU_SOURCE
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

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
        perror("Unable to allocate buffer");
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
	for(i=0;i<tokenCount;i++)
	{
		if(tokenType[i] == 1)
		{
			printf("Our shell does not support '|' yet\n");
			free(tokenType);
			return;
		}
	}
	//Check if there's a redirect
	for(i=0;i<tokenCount;i++)
	{
		if(tokenType[i] == 2)
		{
			printf("Our shell does not support '>>' yet\n");
			free(tokenType);
			return;
		}
	}
  char** bufor = malloc(tokenCount * sizeof(char*));
  for (i = 0; i < tokenCount; i++)
  {
    if (tokenType[i]==3 || tokenType[i]==4)
    {
        bufor[i] = command[i];
    }
    //printf("%s",bufor[i]);

    execvp(bufor[0], &bufor[0]);
/*
    if i==tokenCount
      exec
    if tokenType==1
      execv
      redirect()
    if tokenType==2
      exec
      doPliku
      r
    parameter = 3, command = 4
    code */
  }
  //printf("PATH : %s\n", getenv("PATH"));
	free(tokenType);
}

int main()
{
  while(1)
  {
	printCommandPrompt();
	char* userResponse = readLineFromCommandPrompt();
	if(userResponse == NULL)
	{
		fprintf(stderr, "readLineFromCommandPrompt() failed\n");
		return 1;
	}
	int tokenCount = 0;
	char** tokens = getTokens(userResponse, &tokenCount);
	if(tokens == NULL)
	{
		fprintf(stderr, "getTokens() failed\n");
		return 2;
	}
	//Execute
	execute(tokens, tokenCount);
	//Free allocated memory
	free(tokens);
	free(userResponse);
  }
  return 0;
  /* wait pid jeżeli & to jest wyłączony */

}
