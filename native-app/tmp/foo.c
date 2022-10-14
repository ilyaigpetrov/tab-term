#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

void nmPrint(char* message) {
  int length = strlen(message) + 2;
  char quotedMessage[length]; 
  sprintf(quotedMessage, "\"%s\"", message);
  //printf("SIZEOF UINT: %ld", sizeof(length));
  fwrite(&length, sizeof(length), 1, stdout);
  fwrite(quotedMessage, sizeof(*quotedMessage), length, stdout);
  fflush(stdout);
}

int main() {
  nmPrint("Hello from the native app!");

  // Create a pipe.
  int pipeToParent[2], pipeToChild[2];
  if (pipe(pipeToParent) == -1 || pipe(pipeToChild) == -1) {
    perror("pipe");
    exit(EXIT_FAILURE);
  }

  pid_t cpid = fork();
  if (cpid == -1) {
    perror("fork");
    exit(EXIT_FAILURE);
  }

  if (cpid == 0) {
    // Child.
    while ((dup2(pipeToParent[1], STDOUT_FILENO) == -1) && (errno == EINTR)) {}
    while ((dup2(pipeToParent[1], STDERR_FILENO) == -1) && (errno == EINTR)) {}
    //close(pipeToParent[1]);

    while ((dup2(pipeToChild[0], STDIN_FILENO) == -1) && (errno == EINTR)) {}
    //close(pipeToChild[0]);
    nmPrint("EXECUTING BASH!");
    execl("/usr/bin/bash", "/usr/bin/bash", (char  *) NULL);
    perror("execl");
    _exit(1);
  }
  nmPrint("HELLO FROM PARENT");
  //close(pipeToChild[1]); // TODO: Must be closed once we have nothing to write to the child process.

  char buffer[4096];
  while (1) {
    nmPrint("ITERATION: gonna read...");
    ssize_t count = read(pipeToParent[0], buffer, sizeof(buffer));
    printf("Read count=%li", count);
    if (count == -1) {
      if (errno == EINTR) {
        continue;
      } else {
        perror("read");
        exit(1);
      }
    } else if (count == 0) {
      // Read till the end.
      break;
    } else {
      buffer[count - 1] = '\0';
      nmPrint(buffer);
    }
  }
  //close(pipeToParent[0]);
  wait(0);
  return EXIT_SUCCESS;
}
