#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <fcntl.h>

void nmPrint(char* message) {
  int length = strlen(message) + 3;
  char quotedMessage[length];
  sprintf(quotedMessage, "\"%s\"\n", message);
  //printf("SIZEOF UINT: %ld", sizeof(length));
  //fwrite(&length, sizeof(length), 1, stdout);
  char* prefix = "M:";
  fwrite(prefix, sizeof(*prefix), 2, stdout);
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
    // CHILD
    nmPrint("Hello from child!");
    while ((dup2(pipeToParent[1], STDOUT_FILENO) == -1) && (errno == EINTR)) {}
    while ((dup2(pipeToParent[1], STDERR_FILENO) == -1) && (errno == EINTR)) {}
    //close(pipeToParent[1]);

    while ((dup2(pipeToChild[0], STDIN_FILENO) == -1) && (errno == EINTR)) {}
    //close(pipeToChild[0]);
    puts("EXECUTING BASH!");
    execlp("node", "/usr/bin/node", "--version", (char  *) NULL);
    perror("execlp");
    _exit(1);
  }
  // PARENT
  nmPrint("HELLO FROM PARENT");
  while ((dup2(STDIN_FILENO, pipeToChild[1]) == -1) && (errno == EINTR)) {
    puts("Associating toChild_1 to STDIN");
  }
  //close(pipeToChild[1]);
  puts("toChild_1 is STDIN");
  while ((dup2(STDOUT_FILENO, pipeToParent[0]) == -1) && (errno == EINTR)) {
    puts("Associating toParent_0 to STDOUT");
  }
  //close(pipeToParent[0]);
  puts("Finished associations in parent.");

  char buffer[4096];
  while (1) {
    nmPrint("ITERATION: gonna read...");
    ssize_t count = read(pipeToParent[0], buffer, sizeof(buffer));
    nmPrint("COUNT IS READ");
    if (count == -1) {
      if (errno == EINTR) {
        continue;
      } else {
        perror("read");
        exit(1);
      }
    } else if (count == 0) {
      // Read till the end.
      nmPrint("Read till the end. Breaking.");
      break;
    } else {
      nmPrint("WRITE CHILD BUFFER TO PARENT STDOUT");
      write(pipeToChild[1], buffer, count);
    }
  }
  //close(pipeToParent[0]);
  return EXIT_SUCCESS;
}
