#include <stdlib.h>
#include <stdio.h>
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



char* logfile_name = "logfile";

void produce() {
  key_t sharedMemoryKey;
  int shmid;

  if ((sharedMemoryKey = ftok("./monitor.c", 0)) == ((key_t) - 1))
  {
    //fprintf(stderr, "%s: ", argv[0]);
    perror("Error: Failed to derive key from a.c\n");
    exit(1);
  }

  if ((shmid = shmget(sharedMemoryKey, sizeof(int), IPC_CREAT | 0600)) == -1)
  {
    //fprintf(stderr, "%s: ", argv[0]);
    perror("Error: Failed to create semaphore with key\n");
    exit(1);
  }
  // sem_wait() function
  //
  int* shmp = (int *)shmat(shmid, NULL, 0);
  
  *shmp += 1;
  printf("\nProducer just incremented = %d", *shmp);
  printf("\nLogile = %s", logfile_name);
  shmdt(shmp);
  
  //             // sem_post() function
}

void consume() {
  int shmid;
  int* shmp;
  key_t sharedMemoryKey;
  if ((sharedMemoryKey = ftok("./monitor.c", 0)) == ((key_t) - 1))
  {
    //fprintf(stderr, "%s: ", argv[0]);
    perror("Error: Failed to derive key from a.c\n");
    exit(EXIT_FAILURE);
  }

  if ((shmid = shmget(sharedMemoryKey, sizeof(int), IPC_CREAT | 0600)) == -1)
  {
    //fprintf(stderr, "%s: ", argv[0]);
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
}

void help_message() {
  printf("\nHelp Message - fill in later\n");
  exit(0);
}

void error_halt(char* title) {
    char* error_message = title;
    strcat(error_message, ": Error");
    errno = EINVAL;
    perror(error_message);
    exit(-1);
}
