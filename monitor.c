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
#include "libmonitor.h"
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

int main(int argc, char** argv) {
  fun();
  char* logfile_name = "logfile";
  int producer_num = 2,
      consumer_num = 6,
      time_end = 100;
  bool logfile_flag = false;     

  printf("\n\nParsing Begins\n\n");
  
   

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
 
  printf("\n\nParsing Commences\n\n");
  printf("\nName of logfile = %s", logfile_name);
  printf("\nNumber of producers = %d", producer_num);
  printf("\nNumber of consumers = %d", consumer_num);
  printf("\nTime until termination = %d\n\n", time_end);
  
  if(logfile_flag)
    free(logfile_name);
  return EXIT_SUCCESS;
}
