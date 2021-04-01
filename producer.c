#include <stdio.h>
#include <sys/ipc.h> 
#include <sys/shm.h>
#include "libmonitor.h"
#include <stdlib.h>

int main(int argc, char** argv) {
  printf("\nargc = %d", argc);
  printf("\nargv[1] = %s", argv[1]);
  int x = atoi(argv[1]);
  produce(x);
}
