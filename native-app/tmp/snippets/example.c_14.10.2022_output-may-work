#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

/*
https://developer.chrome.com/docs/apps/nativeMessaging/#native-messaging-host-protocol:

> Chrome starts each native messaging host in a separate process and communicates with it using standard input (stdin) and standard output (stdout). The same format is used to send messages in both directions: each message is serialized using JSON, UTF-8 encoded and is preceded with 32-bit message length in native byte order. The maximum size of a single message from the native messaging host is 1 MB, mainly to protect Chrome from misbehaving native applications. The maximum size of the message sent to the native messaging host is 4 GB.
*/
long int messageToAddonLimit = 1048576; // 2**20 = 1 MB
long int messageToHostLimit = 4294967296; // 2**32 = 4 GB

void main() {

  int myPipe[2];
  if (pipe(myPipe) == -1) {
    perror("pipe");
    exit(1);
  }

  pid_t pid = fork();
  if (pid == -1) {
    perror("fork");
    exit(1);
  } else if (pid == 0) {
    // Child.
    while ((dup2(myPipe[1], STDOUT_FILENO) == -1) && (errno == EINTR)) {}
    close(myPipe[1]);
    close(myPipe[0]);
    execlp("node", "/usr/bin/node", "--version", (char*)NULL);
    perror("execlp");
    _exit(1);
  }
  // Parent.
  close(myPipe[1]);

  char buffer[4096];
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
      break;
    } else {
      long int quotedLen = count + 2;
      char quotedMessage[quotedLen];
      buffer[count] = '\0';
      sprintf(quotedMessage, "\"%s\"", buffer);
      fwrite(&quotedLen, sizeof(quotedLen), 1, stdout);
      fwrite(quotedMessage, 1, quotedLen, stdout);
      //write(STDOUT_FILENO, quotedMessage, quotedLen);
      fflush(stdout);
    }
  }
  close(myPipe[0]);
  wait(0);

}
