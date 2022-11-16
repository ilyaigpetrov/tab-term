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
    //close(pipeToParent[1]);
    //close(pipeToParent[0]);

    while ((dup2(pipeToChild[0], STDIN_FILENO) == -1) && (errno == EINTR)) {
      puts("Associating toChild_0 to STDIN");
    }
    puts("3rd pipeflow configured.");
    //close(pipeToChild[0]);
    //close(pipeToChild[1]);

    puts("Finished association in child.");

    puts("Childs execs node or bash...");
    execlp("node", "/usr/bin/node", (char  *) NULL);
    perror("execl");
    _exit(1);
  }
  // PARENT:
  puts("Parent continues after the fork.");
  //close(pipeToParent[1]);
  //close(pipeToChild[0]);
  puts("Finished associations in parent.");
  int readFrom(int from, int to) {
    char buf[BUF_SIZE];
    ssize_t count = read(from, buf, sizeof(buf));
    if (count == -1) {
      if (errno == EINTR) {
        return 0;
      } else {
        perror("read");
        exit(1);
      }
    } else if (count != 0) {
      write(to, buf, count);
    }
    // If count == 0 -> we reached the end of input.
    return 0;
  }  
  while (1) {
    readFrom(STDIN_FILENO, pipeToChild[1]);
    readFrom(pipeToParent[0], STDOUT_FILENO);
  }
}
