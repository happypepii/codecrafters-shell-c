#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char *isExecutable(char *arg);

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
        char *full_path = isExecutable(arg);
        if (full_path != NULL)
        {
          printf("%s is %s\n", arg, full_path);
          free(full_path);
        }
        else
        {
          printf("%s: not found\n", arg);
        }
      }
    }
    else if (isExecutable(cmd) != NULL)
    {
      // an executable program
      pid_t pid = fork();
      if (pid == -1)
        perror("fork failed");
      else if (pid == 0) // child process
      {
        char *my_args[64];
        int i = 0;
        my_args[i++] = cmd;

        char *token = strtok(arg, " ");
        while (token != NULL && i < 63)
        {
          my_args[i++] = token;
          token = strtok(NULL, " ");
        }
        my_args[i] = NULL; // the last parameter for execvp has to be NULL
        execvp(isExecutable(cmd), my_args); // if child process

        // if execvp succeed, this will never be called.
        perror("execv failed"); 
        exit(1);
      }
      else
        wait(NULL); // if parent process, wait for child process before proceeding.
      // printf(".%s %s\n", isExecutable(cmd), arg);
    }
    else
    {
      printf("%s: command not found\n", cmd);
    }
  }

  return 0;
}

char *isExecutable(char *arg)
{
  char *path_env = getenv("PATH");
  if (!path_env)
    return NULL;

  char *path_copy = strdup(path_env); // duplicate the path in case of modification
  char *dir = strtok(path_copy, ":");
  char temp_path[1024];
  char *result = NULL;

  while (dir != NULL)
  {
    sprintf(temp_path, "%s/%s", dir, arg);

    // if find an executable file
    if (access(temp_path, X_OK) == 0)
    {
      result = strdup(temp_path);
      break;
    }

    // return NULL when finishing splitting
    dir = strtok(NULL, ":");
  }

  free(path_copy);

  return result;
}
