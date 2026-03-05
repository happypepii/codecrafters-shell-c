#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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
        char *path_env = getenv("PATH");
        char *dir = strtok(path_env, ":");
        int found = 0;

        while (dir != NULL)
        {
          // try to find arg in *path
          char full_path[1024];
          sprintf(full_path, "%s/%s", dir, arg);

          // if find an executable file
          if (access(full_path, X_OK) == 0)
          {
            printf("%s is %s\n", arg, full_path);
            found = 1;
            break;
          }

          // return NULL when finishing splitting
          dir = strtok(NULL, ":");
        }
        if (found == 0)
        {
          printf("%s: not found\n", arg);
        }
      }
    }
    else
    {
      printf("%s: command not found\n", cmd);
    }
  }

  return 0;
}
