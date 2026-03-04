#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
  while (1)
  {
    // Flush after every printf
    setbuf(stdout, NULL);

    printf("$ ");

    char cmd[100];
    fgets(cmd, 100, stdin);
    // fgets will include the '\n' symbol in the end
    cmd[strlen(cmd) - 1] = '\0';

    if (strcmp(cmd, "exit") == 0){
      break;
    }
    else if (strncmp(cmd, "echo ", 5) == 0)
    {
      printf("%s", cmd + 5);
      printf("\n");
    }
    else{
      printf("%s: command not found\n", cmd);
    }
  }

  return 0;
}
