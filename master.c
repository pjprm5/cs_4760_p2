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
  
  // Variables for allocating, deallocating shared memory
  
  int shmid;                  // Contains segment ID
  char *sharedArray;          // Shared memory array contains infile contents
  char tempArray[800];        // Hoping to store infile contents in here
  char *buffer = NULL;        // Buffer for getline  
  size_t bufferSize = 80;     // Buffer size for getline
  int line_count = 0;         // Keep track of lines in infile
  int i;                      // Loop counter
  int buffArray[100];         // Keep track of buffer sizes for each palidrome during getline process

  // Variables for fork() exec()

  pid_t childpid = 0;
  int pr_count = 0;                // Holds number of active processes
  int pr_limit = maxChildCreated;  // Holds max allowed processes to be created
  
  // Initialize tempArray
  for (i = 0; i < 800; i++)
  {
    tempArray[i] = ' ';
  }
  
  // Getopt Menu Loop
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

  // Attach to the shared memory
  sharedArray = (char *)shmat(shmid, (void *)0, 0);
  if (sharedArray == (char *)(-1))
  {
    perror("Master: Error: shmat");
    exit(1);
  }
  
  // Copy a string into shared memory for testing
  //strncpy(sharedArray, test, 7);

  //printf("Shared memory contains: %s \n", sharedArray);

  // Detach from segment.
  /*if (shmdt (sharedArray) == -1)
  {
    perror("Master: Error: shmdt");
    exit(1);
  }*/


  
  // Deallocate shared memory segment

 /* if (shmctl(shmid, IPC_RMID, NULL) == -1)
  {
    perror("Master: Error: shmctl");
  }*/


  
  

  // Open file stream to gain access to infile which contains the palindromes to be tested
  FILE *fptr;
  if ((fptr = fopen("infile", "r")) == NULL)
  {
    fprintf(stderr, "Error! File will not open. \n");
    return -1;
  }
  

  // Trying to copy infile into tempArray
  
  while (getline(&buffer, &bufferSize, fptr) != -1)
  {
    printf("Line 1: %s\n", buffer);
    buffArray[line_count] = strlen(buffer);

    for (i = 0; i < strlen(buffer); i++)
    {
       tempArray[line_count * 80 + i] = buffer[i];
       printf("%c", tempArray[line_count * 80 + i]);
    }

    //tempArray[line_count * 80 + (i+1)] = '\0';
    //printf("Null should print nothing here:%c", tempArray[line_count * 80 + (i+1)]);
    printf("\n");

    line_count++; 
  }
  
  // Now copy tempArray into sharedArray...
  strncpy(sharedArray, tempArray, 800);
  
  
  /*// Display whats in sharedArray...
  for (i = 0; i < 800; i++)
  {
    printf("%c", sharedArray[i]); 
  }*/
  
  /*// buffArray display
  for (i = 0; i < line_count; i++)
  {
    printf("Buff array:%d", buffArray[i]);
  }*/

  
  // Now add nulls into sharedArray...
  
  for (i = 0; i < line_count; i++)
  {
    sharedArray[i * 80 + buffArray[i]] = '\0';
  }

 
  /* //buffArray display again
  for (i = 0; i < 800; i++)
  {
    printf("%c", sharedArray[i]);
  }
 */

  //printf("Should print d here --> %c\n", sharedArray[1*80 + 3]);

  //***************** BEGIN FORKING/EXEC CHILDREN *****************//
  if ((childpid = fork()) <= 0)
  {
    //break;
    printf("Child released!\n");
  }
  
  // Check to see if fork failed
  if (childpid == -1)
  {
    perror("Master: Error: Fork failed: ");
    return -1;
  }
  
  // Exec() child
  execl("palin", "palin", NULL);
  

  // Detach from segment
  if (shmdt (sharedArray) == -1)
  {
    perror("Master: Error: shmdt");
    exit(1);
  }
  
  // Deallocate shared memory segment
  if (shmctl(shmid, IPC_RMID, NULL) == -1)
  {
    perror("Master: Error: shmctl");
    exit(1);
  }



  
  fprintf(stderr, "Option -n: You chose %d max processes to be created. \n", maxChildCreated);
  fprintf(stderr, "Option -s: You chose %d child processes to exist at the same time. \n", maxChildRunning);
  fprintf(stderr, "Option -t: You chose %d seconds until termination of program. \n", timeTillTerm);
  printf("Program ended. \n");
  
  //Close file 
  fclose(fptr);

  return 0;
}
