#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

enum {BUF_SIZE = 4096};

int main() {

  puts("Started...");
  // Create a pipe.
  int pipeToParent[2], pipeToChild[2];
  if (pipe(pipeToParent) == -1 || pipe(pipeToChild) == -1) {
    perror("pipe");
    exit(EXIT_FAILURE);
  }
  puts("2 pipes created.");

  pid_t cpid = fork();
  if (cpid == -1) {
    perror("fork");
    exit(EXIT_FAILURE);
  }

  if (cpid == 0) {
    // CHILD:
    puts("Child started.");
    while ((dup2(pipeToParent[1], STDOUT_FILENO) == -1) && (errno == EINTR)) {
      puts("Associating toParent_1 to STDOUT");
    }
    puts("1st pipeflow configured.");
    close(pipeToParent[1]);
    close(pipeToParent[0]);

    while ((dup2(pipeToChild[0], STDIN_FILENO) == -1) && (errno == EINTR)) {
      puts("Associating toChild_0 to STDIN");
    }
    puts("3rd pipeflow configured.");
    close(pipeToChild[0]);
    close(pipeToChild[1]);

    puts("Finished association in child.");

    puts("Childs execs node or bash...");
    execlp("node", "/usr/bin/node", "--version", (char  *) NULL);
    perror("execl");
    _exit(1);
  }
  // PARENT:
  puts("Parent continues after the fork.");
  close(pipeToParent[1]);
  while ((dup2(pipeToChild[1], STDIN_FILENO) == -1) && (errno == EINTR)) {
    puts("Associating toChild_1 to STDIN");
  }
  puts("toChild_1 is STDIN");
  while ((dup2(pipeToParent[0], STDOUT_FILENO) == -1) && (errno == EINTR)) {
    puts("Associating toParent_0 to STDOUT");
  }
  puts("Finished associations in parent.");
  ssize_t numRead;
  char buf[BUF_SIZE];
  while (1) {
    puts("READING FROM CHILD...");
    numRead = read(STDIN_FILENO, buf, BUF_SIZE);
    printf("%li read.", numRead);

    write(pipeToChild[1], buf, numRead);
    write(pipeToChild[1], "\n", 1);
    puts("WRITING TO PARENT");
    /*
    if (close(pipeToParent[0]) == -1) {
      perror("close");
      exit(EXIT_FAILURE);
    }
    */
    _exit(EXIT_SUCCESS);
  }
}
