#include <stdio.h>
#include <sys/ipc.h> 
#include <sys/shm.h>
#include "libmonitor.h"
#include "stdlib.h"

int main(int argc, char** argv) {
  key_t sharedMemoryKey;
  int shmid;

  if ((sharedMemoryKey = ftok("./monitor.c", 0)) == ((key_t) - 1))
  {
    fprintf(stderr, "%s: ", argv[0]);
    perror("Error: Failed to derive key from a.c\n");
    exit(1);
  }

  if ((shmid = shmget(sharedMemoryKey, sizeof(int), IPC_CREAT | 0600)) == -1)
  {
    fprintf(stderr, "%s: ", argv[0]);
    perror("Error: Failed to create semaphore with key\n");
    exit(1);
  }



  // sem_wait() function


  int* shmp = (int *)shmat(shmid, NULL, 0);

  *shmp += 1;
  printf("\nProducer just incremented = %d", *shmp);
  printf("\nLogile = %s", logfile_name);
  shmdt(shmp);

  // sem_post() function

  return 0;
}
