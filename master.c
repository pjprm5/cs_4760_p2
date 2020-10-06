//Paul Passiglia
//cs_4760
//Project 2
//Date: 10/5/2020



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>
#include <getopt.h>
#include <errno.h>

int main (int argc, char *argv[])
{
  
  int option;                // Holds option values.
  int maxChildCreated = 0;   // Option -n; Holds max # of children to be created ever.
  int maxChildRunning = 0;   // Option -s; Holds max # of children to be running together
  int timeTillTerm = 0;      // Option -t; Holds time limit until all processes must end.

  while ((option = getopt (argc, argv, "hn:s:t:" )) != -1)
  {
    switch (option)
    {

      case 'h' :
        fprintf(stderr, "Explanation here.\n");
        return -1;

      case 'n' :
        maxChildCreated = atoi(optarg);
        if (maxChildCreated > 20 || maxChildCreated < 1)
        {
          fprintf(stderr, "Error; -n option must be from numbers 1-20. \n");
        }
        //fprintf(stderr, "Option -n: You chose %d max processes to be created. \n", maxChildCreated);
        //default 4 on printout
        else
        {
          break;
        }
      case 's' :
        maxChildRunning = atoi(optarg);
        if (maxChildRunning > 20 || maxChildRunning < 1)
        {
          fprintf(stderr, "Error; -s option must be from numbers 1-20. \n");
        }
        //fprintf(stderr, "Option -s: You chose %d child processes to exist at the same time. \n", maxChildRunning);
        //default 2 on printout
        else
        {
          break;
        }
      case 't' :
        timeTillTerm = atoi(optarg);
        if (timeTillTerm > 100 || timeTillTerm < 1)
        {
          fprintf(stderr, "Error: -t option must be from numbers 1-100. \n");
        }
        //fprintf(stderr, "Option -t: You chose %d seconds until termination of program. \n", timeTillTerm);
        //default 100 on printout
        else
        {
          break;
        }
      case '?' :
        if (optopt == 'n')
        {
          fprintf(stderr, "Option -%c requires an argument. \n", optopt);
        }
        if (optopt == 's')
        {
          fprintf(stderr, "Option -%c requires an argument. \n", optopt);
        }
        if (optopt == 't')
        {
          fprintf(stderr, "Option -%c requires an argument. \n", optopt);
        }
        
        return -1;
      default:
         fprintf(stderr, "Default error. \n");
         return -1;
    }
  }

  
  fprintf(stderr, "Option -n: You chose %d max processes to be create. \n", maxChildCreated);
  fprintf(stderr, "Option -s: You chose %d child processes to exist at the same time. \n", maxChildRunning);
  fprintf(stderr, "Option -t: You chose %d seconds until termination of program. \n", timeTillTerm);
  printf("Program ended. \n");


  return 0;
}
