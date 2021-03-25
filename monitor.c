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


  shmid = shm_open(path, O_CREAT | O_EXCL | O_RDWR,S_IRUSR | S_IWUSR);
  if (shmid == -1) {
	printf("fd didn't work");
	exit(1);
  } 

  ftruncate(shmid, sizeof(struct shmbuf));
 
  shmp = mmap(NULL, sizeof(*shmp), PROT_READ | PROT_WRITE, MAP_SHARED, shmid, 0);

  if (shmp == MAP_FAILED) {
	printf("mmap didn't work");
	exit(1);
  }  

  if(sem_init(&shmp->semS, 1, 1) == -1) {
    printf("shmp->semS didn't work");
    exit(1);
  }

  if(sem_init(&shmp->semN, 1, 0) == -1) {
    printf("shmp->semN didn't work");
    exit(1);
  }

  if(sem_init(&shmp->semN, 1, 4) == -1) {
    printf("shmp->semN didn't work");
    exit(1);
  }

  shmp->item = 5;
  int child;

  int i;
  for(i = 0; i < producer_num; i++) {
    child = fork();
    //char num[50];
    //sprintf(num, "%d", i);
    //char* pr_arg = arg_str("./producer", num);
    //printf("\npr_arg = %s", pr_arg);
    if(child == 0) {
      execl("./producer", "./producer", NULL);
      //execl("./producer", pr_arg, NULL);
      //execl("./producer", "./producer", i, NULL);
    }
  }

  for(i = 0; i < 2; i++) {
    child = fork();
    if(child == 0) {
      execl("./consumer", "./consumer", NULL);
    }
  }

  int status;
  while ((child = wait(&status)) > 0);

  printf("\nAfter calling both children, item = %d\n", shmp->item);
  

  shm_unlink (path);
  if(logfile_flag)
    free(logfile_name);
  return EXIT_SUCCESS;
}
