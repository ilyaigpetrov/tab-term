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

  int pipeToParent[2];
  int pipeToChild[2];
  if (pipe(pipeToParent) == -1 || pipe(pipeToChild) == -1) {
    perror("pipe");
    exit(1);
  }

  pid_t pid = fork();
  if (pid == -1) {
    perror("fork");
    exit(1);
  } else if (pid == 0) {
    // Child.
    while ((dup2(pipeToParent[1], STDOUT_FILENO) == -1) && (errno == EINTR)) {}
    close(pipeToParent[1]);
    close(pipeToParent[0]);
    while ((dup2(STDIN_FILENO, pipeToChild[0]) == -1) && (errno == EINTR)) {}
    close(pipeToChild[1]);
    close(pipeToChild[0]);
    execlp("uname", "/usr/bin/uname", (char*)NULL);
    perror("execlp");
    _exit(1);
  }
  // Parent.
  //close(pipeToParent[1]);
  //close(pipeToChild[0]);
  enum {BUF_SIZE = 4096};
  char buffer[BUF_SIZE];

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


  bool writeStdOut() {
    char buffer[BUF_SIZE];

    ssize_t count = read(pipeToParent[0], buffer, sizeof(buffer));
    //printf("Read count=%ld\n", count);
    int state = countToState(count);
    if (state == IfToBreak) {
      return IfToBreak;
    }
    int quotedLen = count + 2; // +2 double quotes.
    char quotedMessage[quotedLen];
    sprintf(quotedMessage, "\"%s\"", buffer);
    write(STDOUT_FILENO, &quotedLen, sizeof(quotedLen));
    write(STDOUT_FILENO, quotedMessage, quotedLen);
    return IfToContinue;
  }

  bool readStdIn() {
    char buffer[BUF_SIZE];

    ssize_t count = read(STDIN_FILENO, buffer, sizeof(buffer));
    //printf("Read count=%ld\n", count);
    int state = countToState(count);
    if (state == IfToBreak) {
      return state;
    }
    // Remove quotes.
    buffer[count - 1] = '\0';
    char* msg = buffer + 1 + 4;
    printf("Write to child: <%s>\n", msg);
    write(pipeToChild[1], msg, sizeof(msg));
    return IfToContinue;
  }

  int ifContinue1 = true;
  int ifContinue2 = true;
  while (ifContinue1 || ifContinue2) {
    //puts("Cycle iteration stared.");
    ifContinue1 = writeStdOut();
    //printf("ifContinue1=%d", ifContinue1);
    //ifContinue2 = readStdIn();
    //printf("ifContinue2=%d", ifContinue2);
  }

  close(pipeToParent[0]);
  close(pipeToChild[0]);
  wait(0);

}
