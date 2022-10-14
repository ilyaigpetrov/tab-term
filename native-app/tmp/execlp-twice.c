#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

int main() {
  puts("Script started.");
  int myPipe[2];
  if (pipe(myPipe) == -1) {
    perror("pipe");
    exit(EXIT_FAILURE);
  }
  puts("BEFORE DUP");
  dup2(STDOUT_FILENO, myPipe[1]);
  puts("AFTER DUP");

  pid_t cpid = fork();
  if (cpid == -1) {
    perror("fork");
    exit(EXIT_FAILURE);
  }
  if (cpid == 0) {
    // CHILD
    dup2(myPipe[1], STDOUT_FILENO);
    close(myPipe[0]);
    close(myPipe[1]);
    puts("EXECUTING BASH!");
    execlp("bash", "/usr/bin/bash", "--version", (char  *) NULL);
    perror("execlp");
    _exit(1);
  }
  // PARENT
  char* buffer[4096];
  //write(STDOUT_FILENO, buffer, sizeof(buffer));
  //execlp("echo", "/usr/bin/echo", "OUTPUTED FROM ECHO", (char*) NULL);

  while (1) {
    ssize_t count = read(myPipe[0], buffer, sizeof(buffer));
    if (count == -1) {
      if (errno == EINTR) {
        continue;
      } else {
        perror("read");
        exit(1);
      }
    } else if (count == 0) {
      // Finished reading.
      break;
    } else {
      write(STDOUT_FILENO, buffer, count);
    }
  }  
  
}
