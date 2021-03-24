// libmonitor header file

#include <semaphore.h>

void fun();
extern char* logfile_name;
void produce();
void consume();
void help_message();
void error_halt(char*);

struct shmbuf {
  sem_t semS;
  sem_t semN;
  sem_t semE;
  int item;
};
