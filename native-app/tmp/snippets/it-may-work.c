#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

void main() {

  int filedes[2];
  if (pipe(filedes) == -1) {
    perror("pipe");
    exit(1);
  }

  pid_t pid = fork();
  if (pid == -1) {
    perror("fork");
    exit(1);
  } else if (pid == 0) {
    while ((dup2(filedes[1], STDOUT_FILENO) == -1) && (errno == EINTR)) {}
    close(filedes[1]);
    close(filedes[0]);
    execlp("node", "/usr/bin/node", (char*)NULL);
    perror("execlp");
    _exit(1);
  }
  close(filedes[1]);

  char buffer[4096];
  while (1) {
    ssize_t count = read(filedes[0], buffer, sizeof(buffer));
    if (count == -1) {
      if (errno == EINTR) {
        continue;
      } else {
        perror("read");
        exit(1);
      }
    } else if (count == 0) {
      break;
    } else {
      write(STDOUT_FILENO, buffer, count);
    }
  }
  close(filedes[0]);
  wait(0);

}
