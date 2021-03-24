#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <errno.h>
#include <stdbool.h>
#include "libmonitor.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char** argv) {
  int shmid;
  int* shmp;
  key_t sharedMemoryKey;
  if ((sharedMemoryKey = ftok("./monitor.c", 0)) == ((key_t) - 1))
  {
    fprintf(stderr, "%s: ", argv[0]);
    perror("Error: Failed to derive key from a.c\n");
    exit(EXIT_FAILURE);
  }

  if ((shmid = shmget(sharedMemoryKey, sizeof(int), IPC_CREAT | 0600)) == -1)
  {
    fprintf(stderr, "%s: ", argv[0]);
    perror("Error: Failed to create semaphore with key\n");
    exit(EXIT_FAILURE);
  }

  // sem_wait() function

  shmp = (int *)shmat(shmid, NULL, 0);
  *shmp -= 1;
  printf("\nConsumer called, decrementing %d", *shmp); 
  printf("\nlogfile named = %s", logfile_name);
  // sem_post() function
  shmdt(shmp);

  return 0;
}
