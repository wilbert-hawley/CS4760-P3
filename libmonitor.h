// libmonitor header file

#include <semaphore.h>
#include <stdbool.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>

extern char* logfile_name;
extern char* path;
void produce(int);
void consume(int);
void help_message();
void error_halt(char*);

struct shmbuf {
  sem_t semS;
  sem_t semN;
  sem_t semE;
  int item;
  pid_t array[100];
};

int setupTime(int);
int timerInterrupt();
void sig_alrm_handler();
void processKiller();
void sig_int_handler();
int start_ctrlc_int_handler();
