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
#include <string.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>


char* logfile_name = "./logfile";

char* path = "practice20"; 

char* arg_str(char* ex, char* num) {
  char *final = malloc(strlen(ex) + strlen(num) + 2);
  //printf("\nex = %s, num = %s", ex, num);
  strcat(final, ex);
  //printf("\nFinal = %s\n", final);
  strcat(final, " ");
  strcat(final, num);
  //printf("\nFinal = %s\n", final);
  return final;
}

void produce() {
  int shmid = shm_open(path, O_RDWR, 0);
  if(shmid == -1) {
    printf("\nProducer failed to open semaphore.\n");
    exit(1);
  }
  //else
  //  printf("\nSemaphore opened in producer\n");

  struct shmbuf *shmp = mmap(NULL, sizeof(*shmp), PROT_READ | PROT_WRITE, MAP_SHARED, shmid, 0);  
  if (shmp == MAP_FAILED) {
    printf("\nmmap in producer failed\n");
    exit(1);
  }  

  if (sem_wait(&shmp->semN) == -1) {
    printf("\nsem_wait(E) failed\n");
    exit(1);
  }
  if (sem_wait(&shmp->semS) == -1) {
    printf("\nsem_wait(S) failed\n");
    exit(1);
  }

  shmp->item += 1;
  printf("\nProducer has decremented. Value of item = %d\n", shmp->item);
  //open an append file
  FILE *file;
  file = fopen(logfile_name,"a");
  fprintf(file, "Producer wrote to file\n");
  fclose(file);
  
  if (sem_post(&shmp->semS) == -1) {
    printf("\nsem_post(S) failed\n");
    exit(1);
  } 
  if (sem_post(&shmp->semE) == -1) {
    printf("\nsem_post(E) failed\n");
    exit(1);
  } 
  //printf("\nMade it to the bottom of producer\n");
}

void consume() {
  sleep(8);
  int shmid = shm_open(path, O_RDWR, 0);
  if(shmid == -1) {
    printf("\nConsumer failed to open semaphore.\n");
    exit(1);
  }

  struct shmbuf *shmp = mmap(NULL, sizeof(*shmp), PROT_READ | PROT_WRITE, MAP_SHARED, shmid, 0);
  if(shmp == MAP_FAILED) { 
    printf("\nmmap in producer failed\n");
    exit(1);
  }

  if(sem_wait(&shmp->semE) == -1) { 
    printf("\nsem_wait(E) failed\n");
    exit(1);
  }
  if(sem_wait(&shmp->semS) == -1) {
    printf("\nsem_wait(S) failed\n");
    exit(1);
  }
  
  shmp->item -= 1;
  printf("\nConsumer has decremented. Value of item = %d\n", shmp->item);
  // open an append file
  FILE *file;
  file = fopen(logfile_name,"a");
  fprintf(file, "Consumer wrote to file\n");
  fclose(file);
 
  if(sem_post(&shmp->semS) == -1) {
    printf("\nsem_post(S) failed\n");
    exit(1);
  }
  if (sem_post(&shmp->semN) == -1) {
    printf("\nsem_post(N) failed\n");
    exit(1);
  }

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
