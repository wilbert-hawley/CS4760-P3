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
#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include <stdbool.h>
#include <signal.h>

char* logfile_name = "./logfile";

char* path = "practice51"; 

bool random_flag = true;

char* arg_str(char* ex, char* num) {
  char *final = malloc(strlen(ex) + strlen(num) + 2);
  strcat(final, ex);
  strcat(final, " ");
  strcat(final, num);
  return final;
}

void produce(int x) {
  time_t now;
  FILE *file;
  key_t sharedMemoryKey;
  if ((sharedMemoryKey = ftok("./README", 0)) == ((key_t) - 1))
  {
    fprintf(stderr, "producer: ");
    perror("Error: Failed to derive key from a.c\n");
    exit(EXIT_FAILURE);
  }
  int shmid;
  if ((shmid = shmget(sharedMemoryKey, sizeof(struct shmbuf), IPC_CREAT | 0600)) == -1)
  {
    fprintf(stderr, "producer: ");
    perror("Error: Failed to create semaphore with key\n");
    exit(EXIT_FAILURE);
  }

  struct shmbuf *shmp = (struct shmbuf *)shmat(shmid, NULL, 0);
  
  int count = 0,
      flag = 0;
  while(1) {
    if (sem_wait(&shmp->semN) == -1) {
      fprintf(stderr, "producer:");
      perror("sem_wait(N) failed");
      exit(1);
    }
    if (sem_wait(&shmp->semS) == -1) {
      fprintf(stderr, "producer:");
      perror("\nsem_wait(S) failed\n");
      exit(1);
    }

    /*if(shmp->item < 4) {
      shmp->item += 1;
      time(&now);

      file = fopen(logfile_name,"a");
      fprintf(file, "Producer %d wrote to the logfile at %s", x, ctime(&now));
      fclose(file);
    }
    else
      flag = 1;
    */
    if(shmp->consumer_count >= shmp->consumer_num) {
      //printf("\nleaving producer %d\n", x);
      if(sem_post(&shmp->semS) == -1) {
        fprintf(stderr, "consumer:");
        printf("sem_post(S) failed");
        exit(1);
      }
      if (sem_post(&shmp->semN) == -1) {
        fprintf(stderr, "consumer:");
        perror("\nsem_post(N) failed\n");
        exit(1);
      }
      printf("\nProducer %d detaching from shared memory\n", x);
      shmdt(shmp);
      return;
    }
    shmp->item += 1; 
    time(&now);

    file = fopen(logfile_name,"a");
    fprintf(file, "Producer %d wrote to the logfile at %s", x, ctime(&now));
    fclose(file);
   
    if (sem_post(&shmp->semS) == -1) {
      fprintf(stderr, "producer:");
      perror("sem_post(S) failed");
      exit(1);
    } 
    if (sem_post(&shmp->semE) == -1) {
      fprintf(stderr, "producer:");
      perror("\nsem_post(E) failed\n");
      exit(1);
    }
    if (random_flag) {
      time_t t;
      srand((unsigned) time(&t)+x);
      int r = rand() % 5;
      printf("Producer %d about to sleep for %d seconds\n", x, r + 1);
      sleep(r + 1);
      printf("Producer %d woke up after %d seconds\n", x, r+1);
    }
  } 
}

void consume(int x) {
  int r = 1;
  time_t now;
  FILE *file;
  key_t sharedMemoryKey;
  if ((sharedMemoryKey = ftok("./README", 0)) == ((key_t) - 1))
  {
    fprintf(stderr, "consumer:");
    perror("Error: Failed to derive key from a.c\n");
    exit(EXIT_FAILURE);
  }
  int shmid;
  if ((shmid = shmget(sharedMemoryKey, sizeof(struct shmbuf), IPC_CREAT | 0600)) == -1)
  {
    fprintf(stderr, "consumer:");
    perror("Error: Failed to create semaphore with key\n");
    exit(EXIT_FAILURE);
  }

  struct shmbuf *shmp = (struct shmbuf *)shmat(shmid, NULL, 0);

  if (random_flag) {
    //time_t t;
    srand((unsigned int) time(NULL)+x);
    r = rand() % 10;
    printf("Consumer %d about to sleep for %d seconds\n", x, r);
  }
  sleep(r);
  printf("Consumer %d woke up after  %d seconds\n", x, r);
  if(sem_wait(&shmp->semE) == -1) { 
    fprintf(stderr, "consumer:");
    perror("sem_wait(E) failed");
    exit(1);
  }
  if(sem_wait(&shmp->semS) == -1) {
    fprintf(stderr, "consumer:");
    perror("sem_wait(S) failed");
    exit(1);
  }
 
  // Critical section 
  shmp->item -= 1;
  shmp->consumer_count++;
  time(&now);

  file = fopen(logfile_name,"a");
  fprintf(file, "Consumer %d wrote to the logfile at %s", x, ctime(&now));
  fclose(file);
 
  if(sem_post(&shmp->semS) == -1) {
    fprintf(stderr, "consumer:");
    printf("sem_post(S) failed");
    exit(1);
  }
  if (sem_post(&shmp->semN) == -1) {
    fprintf(stderr, "consumer:");
    perror("\nsem_post(N) failed\n");
    exit(1);
  }
  // detaching consumer from shared memory
  shmdt(shmp);
  return;
}

void help_message() {
  printf("\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~HELP MESSAGE~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
  printf("monitor [-h] [-o logfile] [-p m] [-c n] [-t time]\n");
  printf("\n-h        Use the -h option to call for help. This project will");
  printf("\n          demonstrate the use of semaphores a solution to the"); 
  printf("\n          Consumer-Producer problem. The producers will increment");
  printf("\n          an item and the consumers will decrement the item.");
  printf("\n-o logfile The activity of the producers and consumers will be");
  printf("\n          logged to a file which is placed in the current");
  printf("\n          directory. The default name of this file is 'logfile'.");
  printf("\n          If you wish to change the name of this file, use the -o");
  printf("\n          option followed by the name you would like to see. You");
  printf("\n          will have to manually remove it as the 'make clean'");  
  printf("\n          command will only remove the default file name.");
  printf("\n-p m      To specify the number of producers, use -p followed by"); 
  printf("\n          an integer m. This must be less than the number of");
  printf("\n          consumers. The defualt value of m is 2.");
  printf("\n-c n      To specify the number of consumers, use -c followed by"); 
  printf("\n          an integer n. This must be greater than the number of");
  printf("\n          producers; otherwise the program will increment until n");
  printf("\n          is greater than m. The default value of n is 6.");
  printf("\n-t time   There is a default timer which will terminate the");
  printf("\n          program and all child processes. To specify a time at");
  printf("\n          which to terminate use the -t option followed the number");
  printf("\n          of seconds you wish the termination to wait.");
  printf("\n          The default value of time is 100 seconds.");
  printf("\n\nSpecial instruction: if the program terminates via the timer or by other");
  printf("\ninterupts, you must change the char* path variable in libmonitor.c to a unique value.\n\n");              
  exit(0);
}

void error_halt(char* title) {
    char* error_message = title;
    strcat(error_message, ": Error");
    errno = EINVAL;
    perror(error_message);
    exit(-1);
}

int setupTimer(int time)
{
    struct itimerval value;

    value.it_interval.tv_sec = time;
    value.it_interval.tv_usec = 0;
    value.it_value = value.it_interval;

    return setitimer(ITIMER_REAL, &value, NULL);
}

void sig_alrm_handler()
{
  fprintf(stderr, "monitor:");
  perror("Time limit reached!");
  processKiller();
  exit(1);
}

int timerInterrupt()
{
  struct sigaction act;

  act.sa_handler = sig_alrm_handler;
  act.sa_flags = 0;

  return (sigemptyset(&act.sa_mask) || sigaction(SIGALRM, &act, NULL));
}

void processKiller()
{
  key_t sharedMemoryKey;
  if ((sharedMemoryKey = ftok("./README", 0)) == ((key_t) - 1))
  {
    //fprintf(stderr, "%s: ", argv[0]);
    perror("Error: Failed to derive key from a.c\n");
    exit(EXIT_FAILURE);
  }
  int shmid;
  if ((shmid = shmget(sharedMemoryKey, sizeof(struct shmbuf), IPC_CREAT | 0600)) == -1)
  {
    //fprintf(stderr, "%s: ", argv[0]);
    perror("Error: Failed to create semaphore with key\n");
    exit(EXIT_FAILURE);
  }

  struct shmbuf *shmp = (struct shmbuf *)shmat(shmid, NULL, 0);
  int i; 
  for(i = 0; i < 100; i++) {
    printf("killed child\n");
    if( shmp->array[i] == 0) {
      break;
    }
    kill(shmp->array[i], SIGTERM);
  }
  printf("\nkilling shared memory");
  shmdt(shmp);
  shmctl(shmid, IPC_RMID, NULL);
  exit(0);
}

void sig_int_handler()
{
    fprintf(stderr, "monitor:");
    perror("ctrl-c interupt, all children terminated.\n");
    processKiller();
}


int start_ctrlc_int_handler()
{
  struct sigaction act;

  act.sa_handler = sig_int_handler;
  act.sa_flags = 0;

  return (sigemptyset(&act.sa_mask) || sigaction(SIGINT, &act, NULL));
}
