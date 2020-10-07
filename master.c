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
#include <sys/shm.h>
#include <sys/stat.h>

int main (int argc, char *argv[])
{
  if (argc < 2)
  {
    fprintf(stderr, "Error: Missing arguments.\nUsage 1: ./master -h for help.\nUsage 2: ./master -n # -s # -t # to run program. \n");
    return (-1);
  }

  
  int option;                 // Holds option values.
  int maxChildCreated = -1;   // Option -n; Holds max # of children to be created ever.
  int maxChildRunning = -1;   // Option -s; Holds max # of children to be running together
  int timeTillTerm = -1;      // Option -t; Holds time limit until all processes must end.
  int pr_count = 0;           // Holds number of active processes
  
  // Variables for allocating, deallocating shared memory
  int shmid;
  char sharedArray[800];
  
  

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
          fprintf(stderr, "Error: -n option must be from numbers 1-20. \n");
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
          fprintf(stderr, "Error: -s option must be from numbers 1-20. \n");
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
  // Default value logic, not sure about it yet
  if (maxChildCreated == -1 || maxChildRunning == -1 || timeTillTerm == -1)
  {
    if (maxChildCreated == -1)
    {
      maxChildCreated = 4;
    }
    if (maxChildRunning == -1)
    {
      maxChildRunning = 2;
    }
    if (timeTillTerm == -1)
    {
      timeTillTerm = 100;
    }
  }
  

  // Allocate shared memory

  // Key generator for shared memory
  key_t key = ftok("./master", 'j');
  printf("%d is the key.\n", key);
  // Set shared memory ID
  if ((shmid = shmget(key, 1024, 0600 | IPC_CREAT)) == -1)
  {
    perror("Master: Error: shmget");
    exit(1);
  }

  // Attach
  sharedArray = shmat(shmid, NULL, 0);
  if  

  
  

  // Open file stream to gain access to infile which contains the palindromes to be tested
 /* FILE *fptr;
  if ((fptr = fopen("infile", "r")) == NULL)
  {
    fprintf(stderr, "Error! File will not open. \n");
    return -1;
  }*/

  // Need loop here to scan infile and put it into shared memory like sarr[800]


  
  fprintf(stderr, "Option -n: You chose %d max processes to be created. \n", maxChildCreated);
  fprintf(stderr, "Option -s: You chose %d child processes to exist at the same time. \n", maxChildRunning);
  fprintf(stderr, "Option -t: You chose %d seconds until termination of program. \n", timeTillTerm);
  printf("Program ended. \n");
  
  //Close file 
  fclose(fptr);

  return 0;
}
