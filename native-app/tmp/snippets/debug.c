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
  //printf("COUNT IS:(%zd)\n", count);
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
  //printf("BUFFER IS:(%s)\n", buffer);
  return count;
};

int main() {
  char buffer[BUF_SIZE];
  char *pbuf = buffer;
  ssize_t count = readInput(pbuf);
  if (count > 0) {
    pbuf = pbuf + 4 + 1;
    //printf("PBUF:%s:", pbuf);
    //printf("SIZEOF:%ld:", sizeof(pbuf));
    //printf("STRLEN:%ld:", strlen(pbuf));
    do {
      if (pbuf[strlen(pbuf) - 1] == '"') {
        int nextInt = getc(stdin);
        if (nextInt == EOF) {
          pbuf[strlen(pbuf) - 1] = '\0';
        } else {
          ungetc(nextInt, stdin);
        }
      }
      write(STDOUT_FILENO, pbuf, strlen(pbuf));
      count = readInput(pbuf);      
    } while (count);
  }
  // E.g. 0000 0000 has 2**32 of combinations or 4 bytes/chars.
  return 0;
}