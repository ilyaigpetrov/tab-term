#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

int main() {
  char* foo = "f\"o\"obar";
  printf("%s\n", foo);
}
