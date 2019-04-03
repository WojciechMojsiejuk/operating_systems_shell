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
	*tokenCount = position;
  int i=0;
  for(;i<position;i++)
  {
    printf("%s ",tokens[i]);
  }
  return tokens;
}

void execute(char** command, int tokenCount)
{
	int i;
	//TODO: check type of token (-, >>, | etc.) and do action
	for(i=0;i<tokenCount;i++)
	{
		printf("Token number: %d\n", i+1);
		if(command[i][0] == '-')
		{
			
		}
		else if(command[i][0] == '|')
		{
			
		}
		else
		{
			
		}
	}
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
}
