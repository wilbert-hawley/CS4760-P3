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
#include <signal.h>
int shmid;
struct shmbuf *shmp;

int main(int argc, char** argv) {
  bool logfile_flag = false;
  int producer_num = 2,
      consumer_num = 6,
      time_end = 100;
  char* logfile_name2;
  char options;
  //signal(SIGINT, term_handler);
  while (true) {
    options = getopt(argc, argv, ":ho:p:c:t:");
    
    if((options == -1) || argc < 2)
      break;

    switch(options) {
      case 'h':
        help_message();  
      case 'o':
	printf("\nOption -%c: %s\n", options, optarg);
        logfile_name2 = strdup(optarg);
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
  
  if(producer_num >= consumer_num)
  {
    while(producer_num >= consumer_num) {
      consumer_num++;
    }
  }

  if (setupTimer(time_end) == -1)
  {
    fprintf(stderr, "%s: ", argv[0]);
    perror("Failed to set up the ITIMER_REAL interval timer");
    exit(1);;
  }

  if (timerInterrupt() == -1)
  {
    fprintf(stderr, "%s: ", argv[0]);
    perror("Failed to set up the inturrept SIGALRM");
    exit(1);
  }

  if (start_ctrlc_int_handler() == -1) 
  {
    fprintf(stderr, "%s: ", argv[0]);
    perror("Failed to set up the SIGINT handler\n");
    exit(1);
  }

  // Set up shared memory
  key_t sharedMemoryKey;
  if ((sharedMemoryKey = ftok("./README", 0)) == ((key_t) - 1))
  {
    fprintf(stderr, "%s: ", argv[0]);
    perror("Error: Failed to derive key from a.c\n");
    exit(EXIT_FAILURE);
  }

  if ((shmid = shmget(sharedMemoryKey, sizeof(struct shmbuf), IPC_CREAT | 0600)) == -1)
  {
    fprintf(stderr, "%s: ", argv[0]);
    perror("Error: Failed to create semaphore with key\n");
    exit(EXIT_FAILURE);
  }

  shmp = (struct shmbuf *)shmat(shmid, NULL, 0);  

  if(sem_init(&shmp->semS, 1, 1) == -1) {
    fprintf(stderr, "%s: ", argv[0]);
    perror("shmp->semS didn't work");
    exit(1);
  }

  if(sem_init(&shmp->semE, 1, 0) == -1) {
    fprintf(stderr, "%s: ", argv[0]);
    perror("shmp->semN didn't work");
    exit(1);
  }

  if(sem_init(&shmp->semN, 1, 4) == -1) {
    fprintf(stderr, "%s: ", argv[0]);
    perror("shmp->semN didn't work");
    exit(1);
  }

  shmp->item = 0;
  shmp->consumer_count = 0;
  shmp->consumer_num = consumer_num;
  
  int child,
      proc_num = 0,
      proc_max = 19,
      p = 0,
      i;
  for(i = 0; i < producer_num; i++) {
    if(proc_num > proc_max) {
      printf("Reached proc_max, waiting\n");
      wait(NULL);
      proc_num--;
    }
    child = fork();
    char num[50];
    sprintf(num, "%d", i);
    
    if(child == 0)
      execl("./producer", "./producer", num, NULL);
    else {
      shmp->array[p] = child;
      p++;
      proc_num++; 
    }
  }

  for(i = 0; i < consumer_num; i++) {
    if(proc_num > proc_max) {
      printf("Reached proc_max in consumer loop, waiting\n");
      wait(NULL);
      proc_num--;
    }
    child = fork();
    char num2[50];
    sprintf(num2, "%d", i);

    if(child == 0)
      execl("./consumer", "./consumer", num2, NULL);
    else {
      shmp->array[p] = child;
      p++;
      proc_num++;
    }
  }

  int status;
  while ((child = wait(&status)) > 0) {
    //printf("one child has died\n");
  }
  /*while(proc_num > producer_num) {
    wait(NULL);
    proc_num--;
  }*/

  printf("\nAfter calling both children, item = %d\n", shmp->item);
  printf("\nshmp->consumer_num = %d, consumer_num = %d\n", shmp->consumer_num, consumer_num); 
 /* for(i = 0; i < 100; i++) {
    printf("killed child\n");
    if( shmp->array[i] == 0) {
      break;
    }
    kill(shmp->array[i], SIGTERM);
  }*/  

  shmdt(shmp);

  if (shmctl(shmid, IPC_RMID, NULL) == -1)
  {
    perror("Error: Failed to destory shared memory segment.");
    exit(EXIT_FAILURE);
  }
  
  if(logfile_flag) {
    printf("\n~~~~~%s\n", logfile_name2);
    printf("\n here1");
    FILE *f1;
    printf("\n here1");
    f1 = fopen("./logfile", "r");
    if(f1 == NULL)
    {
      fprintf(stderr, "%s: ", argv[0]);
	perror("Error opening logfile to copy.");
	exit(1);
    }  

    FILE *f2;
    f2 = fopen(logfile_name2, "w");
    if(f2 == NULL)
    {
	fprintf(stderr, "%s: ", argv[0]);
	perror("Error opening custom logfile to write.");
	exit(1);
    }

    char c = fgetc(f1);
    while (c != EOF)
    {
      fputc(c, f2);
      c = fgetc(f1);
    }

    fclose(f1);
    fclose(f2);    
    free(logfile_name2);
  }
  return EXIT_SUCCESS;
}
