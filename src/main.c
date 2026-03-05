#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
  char *builtinCmd[] = {"echo", "type", "exit"};
  int length = 3;

  while (1)
  {
    // Flush after every printf
    setbuf(stdout, NULL);

    printf("$ ");

    char cmd[100];
    fgets(cmd, 100, stdin);
    // fgets will include the '\n' symbol in the end
    cmd[strlen(cmd) - 1] = '\0';

    if (strcmp(cmd, "exit") == 0)
    {
      break;
    }
    else if (strncmp(cmd, "type ", 5) == 0)
    {
      int found = 0;
      for (int i = 0; i < length; i++)
      {
        if (strcmp(builtinCmd[i], cmd + 5) == 0)
        {
          found = 1;
          printf("%s is a shell builtin\n", cmd + 5);
          break;
        }
      }
      if(found == 0) printf("%s: not found\n", cmd + 5);
    }
    else if (strncmp(cmd, "echo ", 5) == 0)
    {
      printf("%s", cmd + 5);
      printf("\n");
    }
    else
    {
      printf("%s: command not found\n", cmd);
    }
  }

  return 0;
}
