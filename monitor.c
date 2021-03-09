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
void help_message() {
  printf("\nHelp Message - fill in later\n");
}

void error_halt(char* title) {
    char* error_message = title;
    strcat(error_message, ": Error");
    errno = EINVAL;
    perror(error_message);
    exit(-1);
}

int main(int argc, char** argv) {

  printf("\n\nParsing Begins\n\n");
  
  

  char options;
  while (true) {
    options = getopt(argc, argv, ":ho:p:c:t:");
    
    if((options == -1) || argc < 2)
      break;

    switch(options) {
      case 'h':
        help_message();
        exit(0);
        break;
      case 'o':
	printf("\nOption -%c: %s\n", options, optarg);
        break;
      case 'p':
        printf("\nOption -%c: %s\n", options, optarg); 
        break;
      case 'c':
        printf("\nOption -%c: %s\n", options, optarg);
        break;  
      case 't':
        printf("\nOption -%c: %s\n", options, optarg);
        break;
      default:
         error_halt(argv[0]);
     }
  }
 
  printf("\n\nParsing Commences\n\n");

  return EXIT_SUCCESS;
}
