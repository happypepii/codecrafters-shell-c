#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

char *findExecutable(char *name);
int isBuiltIn(char *cmd);
void printFileContent(char *file_path);

int main(int argc, char *argv[])
{
  char input[1024];
  setbuf(stdout, NULL); // Flush after every printf

  while (1)
  {
    printf("$ ");
    fgets(input, sizeof(input), stdin);
    input[strlen(input) - 1] = '\0'; // fgets includes '\n', strip it

    char *cmd = strtok(input, " ");
    char *rest_args = strtok(NULL, ""); // everything after the command

    if (strcmp(cmd, "exit") == 0)
    {
      break;
    }
    else if (strcmp(cmd, "echo") == 0)
    {
      char *ptr = input + strlen(cmd) + 1;
      int in_quotes = 0;
      int has_printed_space = 0;

      while (*ptr != '\0')
      {
        if (*ptr == '\"' || *ptr == '\'')
        {
          in_quotes = !in_quotes;
        }
        else if (in_quotes)
        {
          putchar(*ptr);
          has_printed_space = 0; // reset flag
        }
        else
        {
          if (*ptr != ' ')
          {
            putchar(*ptr);
            has_printed_space = 0;
          }
          else if (!has_printed_space)
          {
            putchar(' ');
            has_printed_space = 1;
          }
        }
        ptr++;
      }
      printf("\n");
    }
    else if (strcmp(cmd, "type") == 0)
    {
      // rest_args might be NULL if user types "type" with no arguments
      if (rest_args == NULL)
      {
        printf("type: missing argument\n");
      }
      else if (isBuiltIn(rest_args))
      {
        printf("%s is a shell builtin\n", rest_args);
      }
      else
      {
        char *full_path = findExecutable(rest_args);
        if (full_path != NULL)
        {
          printf("%s is %s\n", rest_args, full_path);
          free(full_path);
        }
        else
        {
          printf("%s: not found\n", rest_args);
        }
      }
    }
    else if (strcmp(cmd, "pwd") == 0)
    {
      char buf[1024];
      if (getcwd(buf, sizeof(buf)) != NULL)
      {
        printf("%s\n", buf);
      }
      else
      {
        perror("pwd error");
      }
    }
    else if (strcmp(cmd, "cd") == 0)
    {
      if (strcmp(rest_args, "~") == 0)
      {
        char *home = getenv("HOME");
        chdir(home);
      }
      else
      {
        if (chdir(rest_args) != 0)
        {
          printf("cd: %s: No such file or directory\n", rest_args);
        }
      }
    }
    else if (strcmp(cmd, "cat") == 0)
    {
      char *ptr = input + strlen(cmd) + 1;
      int in_quotes = 0;
      char file_path[1000] = {0};
      int i = 0;

      while (*ptr != '\0')
      {
        if (*ptr == ' ' && !in_quotes && i == 0)
        {
          ptr++;
          continue;
        }

        if (*ptr == '\"' || *ptr == '\'')
        {
          in_quotes = !in_quotes;
        }
        else if (!in_quotes && *ptr == ' ')
        {
          file_path[i] = '\0';
          printFileContent(file_path);
          i = 0;
        }
        else
        {
          file_path[i++] = *ptr;
        }
        ptr++;
      }
      if (i > 0)
      {
        file_path[i] = '\0';
        printFileContent(file_path);
      }
      printf("\n");
    }
    else
    {
      // Declare exec_path here so its scope is limited to where it's needed,
      // and we can free it in the same block without leaking in builtin branches
      char *exec_path = findExecutable(cmd);
      if (exec_path != NULL)
      {
        pid_t pid = fork();
        if (pid == -1)
        {
          perror("fork failed");
        }
        else if (pid == 0) // child process
        {
          char *exec_argv[64];
          int i = 0;
          exec_argv[i++] = cmd;

          char *token = strtok(rest_args, " ");
          while (token != NULL && i < 63)
          {
            exec_argv[i++] = token;
            token = strtok(NULL, " ");
          }
          exec_argv[i] = NULL; // execvp requires NULL-terminated argv

          execvp(exec_path, exec_argv);

          // Only reached if execvp fails
          perror("execvp failed");
          exit(1);
          // No need to free exec_path here: on execvp success the process image
          // is replaced entirely, and on failure we exit immediately anyway
        }
        else // parent process
        {
          wait(NULL); // wait for child to finish before next prompt
        }

        free(exec_path); // only parent reaches here
      }
      else
      {
        printf("%s: command not found\n", cmd);
      }
    }
  }

  return 0;
}

// Search each directory in PATH for an executable named `name`.
// Returns a heap-allocated full path if found, or NULL if not found.
// Caller is responsible for freeing the returned string.
char *findExecutable(char *name)
{
  char *path_env = getenv("PATH");
  if (!path_env)
    return NULL;

  char *path_copy = strdup(path_env); // strtok modifies the string, so duplicate it
  char *dir = strtok(path_copy, ":");
  char temp_path[1024];
  char *result = NULL;

  while (dir != NULL)
  {
    sprintf(temp_path, "%s/%s", dir, name);

    if (access(temp_path, X_OK) == 0) // X_OK checks execute permission
    {
      result = strdup(temp_path);
      break;
    }

    dir = strtok(NULL, ":");
  }

  free(path_copy);
  return result;
}

int isBuiltIn(char *cmd)
{
  char *builtins[] = {"exit", "type", "echo", "pwd", "cd", NULL};
  for (int i = 0; builtins[i] != NULL; i++)
  {
    if (strcmp(cmd, builtins[i]) == 0)
    {
      return 1;
    }
  }
  return 0;
}

void printFileContent(char *file_path)
{
  if (strlen(file_path) == 0)
    return;

  FILE *fp = fopen(file_path, "r");
  if (fp == NULL)
  {
    fprintf(stderr, "cat: %s: No such file or directory\n", file_path);
  }
  else
  {
    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), fp))
    {
      size_t len = strlen(buffer);
      if (len > 0 && buffer[len - 1] == '\n')
      {
        buffer[len - 1] = '\0';
      }
      printf("%s", buffer);
    }
    printf(" ");
    fclose(fp);
  }
}