#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

enum {
  BUF_SIZE = 4096
};

ssize_t readInput(char *buffer) {
  ssize_t count = read(STDIN_FILENO, buffer, sizeof(buffer));
  printf("COUNT IS:?%zd? BUFFER IS:!%s!\n", count, buffer);
  if (count == -1) {
    if (errno == EINTR) {
      return count;
    } else {
      perror("read");
      exit(1);
    }
  } else if (count == 0) { // Reached the end.
    return count;
  }
  buffer[count] = '\0';
  return count;
};

int main() {
  char buffer[BUF_SIZE];
  char *pbuf = buffer;
  ssize_t count = readInput(pbuf);
  if (count > 0) {
    pbuf = pbuf + 4 + 1;
  }
  do {
    write(STDOUT_FILENO, pbuf, sizeof(pbuf));
  } while(readInput(buffer))
  ; // E.g. 0000 0000 has 2**32 of combinations or 4 bytes/chars.
  return 0;
}