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

  if(producer_num > consumer_num)
  {
    while(producer_num > consumer_num)
      consumer_num++;
    printf("Conumer number increased to %d so that there are more consumers than producers", consumer_num); 
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

  shmid = shm_open(path, O_CREAT | O_EXCL | O_RDWR,S_IRUSR | S_IWUSR);
  if (shmid == -1) {
    fprintf(stderr, "%s: ", argv[0]);
    perror("Shared memory failed to open. Change path variable. See -h for instructions");
    exit(1);
  } 

  ftruncate(shmid, sizeof(struct shmbuf));
 
  shmp = mmap(NULL, sizeof(*shmp), PROT_READ | PROT_WRITE, MAP_SHARED, shmid, 0);

  if (shmp == MAP_FAILED) {
    fprintf(stderr, "%s: ", argv[0]);        
    perror("mmap didn't work");
    exit(1);
  }  

  if(sem_init(&shmp->semS, 1, 1) == -1) {
    fprintf(stderr, "%s: ", argv[0]);
    perror("shmp->semS didn't work");
    exit(1);
  }

  if(sem_init(&shmp->semN, 1, 0) == -1) {
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
  int child;

  int p = 0;
  int i;
  for(i = 0; i < producer_num; i++) {
    child = fork();
    char num[50];
    sprintf(num, "%d", i);
    
    if(child == 0)
      execl("./producer", "./producer", num, NULL);
    else {
      shmp->array[p] = child;
      p++; 
    }
  }

  for(i = 0; i < consumer_num; i++) {
    child = fork();
    char num2[50];
    sprintf(num2, "%d", i);

    if(child == 0)
      execl("./consumer", "./consumer", num2, NULL);
    else {
      shmp->array[p] = child;
      p++;
    }
  }

  int status;
  while ((child = wait(&status)) > 0);

  printf("\nAfter calling both children, item = %d\n", shmp->item);
  

  shm_unlink (path);
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
