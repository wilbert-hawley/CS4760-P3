#include <stdio.h>
#include <sys/ipc.h> 
#include <sys/shm.h>
#include "libmonitor.h"
#include <stdlib.h>

int main(int argc, char** argv) {
  int x = atoi(argv[1]);
  produce(x);
}
