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
#include <time.h>
#include "palinArray.h"
#include <semaphore.h>


// Prototypes
void raiseAlarm();

// Global vars
PalArray *sharedArray;
int shmid;

int main (int argc, char *argv[])
{
  /*
  if (argc < 2)
  {
    fprintf(stderr, "Error: Missing arguments.\nUsage 1: ./master -h for help.\nUsage 2: ./master -n # -s # -t # to run program. \n");
    return (-1);
  }*/

  signal(SIGALRM, raiseAlarm);  // Start Alarm.

  
  int option;                // Holds option values.
  int maxChildCreated = 4;   // Option -n; Holds max # of children to be created ever.
  int maxChildRunning = 2;   // Option -s; Holds max # of children to be running together
  int timeTillTerm = 10;    // Option -t; Holds time limit until all processes must end.
  
  struct timespec ts1, ts2;  // For nanosleep iteration smoother
  ts1.tv_sec = 0;
  ts1.tv_nsec = 500L;
  
  char *buffer = NULL;        // Buffer for getline  
  size_t bufferSize = 80;     // Buffer size for getline
  int line_count = 0;         // Keep track of lines in infile
  int i;                      // Loop counter
  int buffArray[100];         // Keep track of buffer sizes for each palidrome during getline process
  
  int pr_count = 0;           // Holds number of processes created
  int pr_active = 0;          // Holds active number of processes

  // Flags for getopt
  int optN = 0;
  int optS = 0;
  int optT = 0;
  
  // Getopt Menu Loop
  while ((option = getopt (argc, argv, "hn:s:t:" )) != -1)
  {
    switch (option)
    {

      case 'h' :
        fprintf(stderr, "Usage: ./master -n # -s # -t #\n");
        fprintf(stderr, "Master will fork() and exec() children to execute ./palin which checks\n");
        fprintf(stderr, "each string inside infile to determine if it is a palindrome or not.\n");
        fprintf(stderr, "-n decides max child processes to be created in total. Defaults to 4 if not specified.\n");
        fprintf(stderr, "-s decides max child processes to be running simultaneously. Defaults to 2 if not specified.\n");
        fprintf(stderr, "-t is the amount of time until the program terminates all children and itself. Defaults to 40 if not specified.\n");

        return -1;

      case 'n' :
        if (atoi(optarg) == 0)
        {
          perror("MASTER: Error: The -n option must be an integer from 1-20: \n");
          return -1;
        }
        maxChildCreated = atoi(optarg);
        if (maxChildCreated > 20 || maxChildCreated < 1)
        {
          fprintf(stderr, "MASTER: Error: -n option must be an integer from 1-20. \n");
          return -1;
        }
       
        optN = 1; // Set flag.
        break;
        
      case 's' :
        if (atoi(optarg) == 0)
        {
          perror("MASTER: Error: the -s option must be an integer from 1-20. \n");
          return -1;
        }
        maxChildRunning = atoi(optarg);
        if (maxChildRunning > 20 || maxChildRunning < 1)
        {
          fprintf(stderr, "MASTER: Error: -s option must be an integer from 1-20. \n");
          return -1;
        }
        
        optS = 1; // Set flag.
        break;
        
      case 't' :
        if (atoi(optarg) == 0)
        {
          perror("MASTER: Error: The -t option must be an integer from 1-100. \n");
          return -1;
        }
        timeTillTerm = atoi(optarg);
        if (timeTillTerm > 100 || timeTillTerm < 40)
        {
          fprintf(stderr, "MASTER: Error: -t option must be an integer from 1-100. \n");
          return -1;
        }
        
        optT = 1; // Set flag.
        break;
        
      case '?' : // Deal with unknowns.
        if (optopt == 'n')
        {
          fprintf(stderr, "MASTER: Error: Option -c requires an argument. \n");
          return -1;
        }
        if (optopt == 's')
        {
          fprintf(stderr, "MASTER: Error: Option -s requires an argument. \n");
          return -1;
        }
        if (optopt == 't')
        {
          fprintf(stderr, "MASTER: Error: Option -t requires an argument. \n");
          return -1;
        }
        else if (isprint (optopt))
        {
          fprintf(stderr, "MASTER: Error: Unknown option detected. \n");
          return -1;
        }
        else
        {
          fprintf(stderr, "Error: Unknown error from options detected, \n");
          return -1;
        }
      default:
        fprintf(stderr, "Default error. \n");
        return -1;
    }
  }

  // Set defaults.
  if (optN == 0)
  {
    maxChildCreated = 4;
  }

  if (optS == 0)
  {
    maxChildRunning = 2;
  }

  if (optT == 0)
  {
    timeTillTerm = 40;
  }

  fprintf(stderr, "\nOption -n: You chose %d max processes to be created. \n", maxChildCreated);
  fprintf(stderr, "Option -s: You chose %d child processes to exist at the same time. \n", maxChildRunning);
  fprintf(stderr, "Option -t: You chose %d seconds until termination of program. \n", timeTillTerm);

  // Allocate shared memory -------------------------------------------------

  // Key generator for shared memory
  key_t key = ftok("./master", 'j');
  if (key == -1)
  {
    perror("MASTER: Error: ftok failure.\n");
    exit(-1);
  }

  // Set shared memory ID
  if ((shmid = shmget(key, sizeof(PalArray), 0600 | IPC_CREAT)) == -1)
  {
    perror("MASTER: Error: shmget failure.\n");
    exit(-1);
  }

  // Attach to the shared memory
  sharedArray = (PalArray *)shmat(shmid, (void *)0, 0);
  if (sharedArray == (void *)(-1))
  {
    perror("MASTER: Error: shmat failure.\n");
    exit(-1);
  }

  // Open file stream to gain access to infile which contains the palindromes to be tested
  FILE *fptr;
  if ((fptr = fopen("infile", "r")) == NULL)
  {
    fprintf(stderr, "Error! File will not open. \n");
    return -1;
  }

  alarm(timeTillTerm); // Set alarm

  // Trying to copy infile into sharedArray -----------------------------------
  sem_init(&(sharedArray->mutex), 1, 1);
  sem_wait(&(sharedArray->mutex));
  while (getline(&buffer, &bufferSize, fptr) != -1)
  {
    //printf("Line %d: %s\n", line_count, buffer);
    buffArray[line_count] = strlen(buffer);

    for (i = 0; i < strlen(buffer); i++)
    {
       sharedArray->palinArray[line_count * 80 + i] = buffer[i];
       //printf("%c", sharedArray->palinArray[line_count * 80 + i]);
    }
    //printf("\n");

    line_count++; 
  }
  // Close file
  fclose(fptr);

  // Write a begin statement into both palin.out and nopalin.out
  fptr = fopen("palin.out", "w");
  fprintf(fptr, "--------------- BEGIN ----------------\n");
  fclose(fptr);

  fptr = fopen("nopalin.out", "w");
  fprintf(fptr, "--------------- BEGIN ----------------\n");
  fclose(fptr);
  
  // Now add nulls into sharedArray
  for (i = 0; i < line_count; i++)
  {
    sharedArray->palinArray[i * 80 + buffArray[i]] = '\0';
  } 
  sem_post(&(sharedArray->mutex));

  // Print array
  /*for (i = 0; i < 1600; i++)
  { 
    
    printf("%c", sharedArray->palinArray[i]);
  }
  printf("\n");*/

  //***************** BEGIN FORKING/EXEC CHILDREN *****************//--------------
  pid_t childPid;
  int childControl = maxChildCreated; 
  int j;
  int k;
  
  // Get pr_count as a string to shoot off index of palindrome 
  char indexToCheck[2];
  snprintf (indexToCheck, sizeof(indexToCheck), "%d", pr_count);
  //printf("TEST MASTER ---->: pr_count: %s \n", indexToCheck);

  for (i = 0; i < childControl; i++)
  {
    if (pr_active <= maxChildRunning)
    {
      wait(NULL); // wait for child to die
      if (pr_active != 0)
      {
        pr_active--; // decrement active children
      }
      if (pr_count <= maxChildCreated)
      {
        childPid = fork(); // fork child
      }

      if (childPid == 0 && pr_count <= maxChildCreated)
      {   
        char *args[] = {"./palin", indexToCheck, NULL};
        execvp(args[0], args);
      }

      else if (childPid < 0)
      {
        childControl++; // fork fails, increment loop by one to try again
        printf("Child fork failed.\n");
        while(nanosleep(&ts1, &ts2)); // retry loop
      }

      else if (childPid > 0 && pr_count <= maxChildCreated)
      {
        pr_active++;
        pr_count++;
        snprintf (indexToCheck, 4, "%d", pr_count);
        //printf("MASTER TEST 2: %s \n", indexToCheck);
        printf("\nMASTER: Child#: %d with pid: %d is born -------> \n", pr_count, childPid);
      }
      
      // Start deallocation once process count reaches max child creation
      if (pr_count == maxChildCreated)
      {
        //printf("pr_count: %d \n", pr_count);

        // Wait for any children left to die
        for (j = 0; j < maxChildCreated; j++)
        {
          // printf("Test 1\n");
          wait(NULL);
        }

        sleep(1); // sleep for 1 second to make sure before deallocation

        // Detach from segment
        if (shmdt (sharedArray) == -1)
        {
          perror("MASTER: Error: shmdt failure.\n");
          exit(-1);
        }
  
        // Deallocate shared memory segment
        if (shmctl(shmid, IPC_RMID, NULL) == -1)
        {
          perror("Master: Error: shmctl failure.\n");
          exit(-1);
        }
      }
    }
  }
  printf("\nMASTER: PROGRAM ENDED. \n\n");

  return 0;
}

void raiseAlarm() // Kill everything within timelimit
{
  printf("\n Time limit hit, terminating all processes. \n");
  shmdt(sharedArray);
  shmctl(shmid, IPC_RMID, NULL);
  kill(0, SIGKILL);
}
