#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

int IfToContinue = 1;
int IfToBreak = 0;

int countToState(ssize_t count) {
  if (count == -1) {
    if (errno == EINTR) {
      return IfToContinue;
    } else {
      perror("read");
      exit(1);
    }
  } else if (count == 0) {
    return IfToBreak;
  }
  return IfToContinue;
}

enum {BUF_SIZE = 4096};

bool readStdIn() {
  char buffer[BUF_SIZE];

  ssize_t count = read(STDIN_FILENO, buffer, sizeof(buffer));
  if (count == -1) {
    if (errno == EINTR) {
      return IfToContinue;
    } else {
      perror("read");
      exit(1);
    }
  } else if (count == 0) { // Reached the end.
    return IfToBreak;
  }
  // Remove quotes.
  buffer[count - 1] = '\0';
  char* msg = buffer + 4 + 1; // E.g. 0000 0000 has 2**32 of combinations or 4 bytes/chars.
  //printf("Write to child: <%s>\n", msg);
  write(STDOUT_FILENO, msg, sizeof(msg));
  return IfToContinue;
}



int main() {
  while(readStdIn()) {};
  return 0;
}
