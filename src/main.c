#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
  char input[1024];
  // Flush after every printf
  setbuf(stdout, NULL);

  while (1)
  {
    printf("$ ");
    fgets(input, sizeof(input), stdin);
    // fgets will include the '\n' symbol in the end
    input[strlen(input) - 1] = '\0';

    char *cmd = strtok(input, " ");
    char *arg = strtok(NULL, "");

    if (strcmp(cmd, "exit") == 0)
    {
      break;
    }
    else if (strcmp(cmd, "echo") == 0)
    {
      printf("%s", arg);
      printf("\n");
    }
    else if (strcmp(cmd, "type") == 0)
    {
      if (!strcmp(arg, "exit") || !strcmp(arg, "type") || !strcmp(arg, "echo"))
      {
        printf("%s is a shell builtin\n", arg);
      }
      else
      {
        printf("%s: not found\n", arg);
      }
    }
    else
    {
      printf("%s: input not found\n", cmd);
    }
  }

  return 0;
}
