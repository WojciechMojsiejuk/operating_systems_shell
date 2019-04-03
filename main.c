#include <unistd.h>
#include <stdio.h>
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
int readLineFromCommandPrompt()
{
  ssize_t read;
  size_t len = 0;
  char *line = NULL;
  int bufsize = 64, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token;
  read = getline(&line, &len, stdin);
  if( read == -1)
    {
        perror("Unable to allocate buffer");
        return read;
    }
  printf("%s\n",line);
  token = strtok(line, " \t\r\n\a");
  while (token != NULL)
  {
    tokens[position] = token;
    position++;
  }
  int i=0;
  for(;i<position;i++)
  {
    printf("%s",tokens[i]);
  }
  return read;

}
int main()
{
  while(1)
  {
    printCommandPrompt();
    int user_response = readLineFromCommandPrompt();
    if( user_response != -1)
      {
        printf("Dzieki dziala");
      }

  }
  return 0;
}
