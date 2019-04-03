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
  read = getline(&line, &len, stdin);
  if( read == -1)
    {
        perror("Unable to allocate buffer");
        return read;
    }
  printf("%s",line);
  return read;

}
int main()
{
  printCommandPrompt();
  int user_response = readLineFromCommandPrompt();
  if( user_response != -1)
    {
      printf("Dzieki dziala");
    }
  return 0;
}
