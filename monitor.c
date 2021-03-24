// Wilbert Hawley
// CS4760 - Assignment 3
// Semaphores
// https://github.com/wilbert-hawley/CS4760-P3

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <errno.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
#include "libmonitor.h"

int shmid;
struct shmbuf *shmp;

int main(int argc, char** argv) {
  int producer_num = 2,
      consumer_num = 6,
      time_end = 100;
  bool logfile_flag = false;

  char options;
  while (true) {
    options = getopt(argc, argv, ":ho:p:c:t:");
    
    if((options == -1) || argc < 2)
      break;

    switch(options) {
      case 'h':
        help_message();  
      case 'o':
	printf("\nOption -%c: %s\n", options, optarg);
        logfile_name = strdup(optarg);
        logfile_flag = true;
        break;
      case 'p':
        printf("\nOption -%c: %s\n", options, optarg); 
        producer_num = atoi(optarg);
        break;
      case 'c':
        printf("\nOption -%c: %s\n", options, optarg);
        consumer_num = atoi(optarg);
        break;  
      case 't':
        printf("\nOption -%c: %s\n", options, optarg);
        time_end = atoi(optarg);
      break;
      default:
         error_halt(argv[0]);
     }
  }


  int fd = shm_open("/shared", O_CREAT | O_EXCL | O_RDWR,S_IRUSR | S_IWUSR);
  if (fd == -1) {
	printf("fd didn't work");
	exit(1);
  } 

  printf("\nvalue of fd = %d\n", fd);

  ftruncate(fd, sizeof(struct shmbuf));
 
  shmp = mmap(NULL, sizeof(*shmp), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

  if (shmp == MAP_FAILED) {
	printf("mmap didn't work");
	exit(1);
  }  

  if(sem_init(&shmp->semS, 1, 1) == -1) {
    printf("mmap didn't work");
    exit(1);
  }

  if(sem_init(&shmp->semN, 1, 0) == -1) {
    printf("mmap didn't work");
    exit(1);
  }

  if(sem_init(&shmp->semN, 1, 4) == -1) {
    printf("mmap didn't work");
    exit(1);
  }



  /*
  printf("\n\nParsing Commences\n\n");
  printf("\nName of logfile = %s", logfile_name);
  printf("\nNumber of producers = %d", producer_num);
  printf("\nNumber of consumers = %d", consumer_num);
  printf("\nTime until termination = %d\n\n", time_end); 
  
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

  shmp = (int *)shmat(shmid, NULL, 0);

  *shmp = 5; 

  int child;

  child = fork();

  if(child == 0) {
    execl("./producer", "./producer", NULL);
    
  }

  child = fork();
  if(child == 0) {
    execl("./consumer", "./consumer", NULL);
  }

  int status;
   while ((child = wait(&status)) > 0);

  printf("\nAfter calling both children, shmp = %d", *shmp);
  
  shmdt(shmp);

  shmctl(shmid, IPC_RMID, NULL); */
  shm_unlink ("/shared");
  if(logfile_flag)
    free(logfile_name);
  return EXIT_SUCCESS;
}
