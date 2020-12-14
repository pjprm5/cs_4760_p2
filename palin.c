//Paul Passiglia
//cs_4760
//Project 2
//Date: 10/5/2020


#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>
#include <errno.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include "palinArray.h"
#include <semaphore.h>

// Global vars
int shmid;
PalArray *sharedArray;

// isPalindrome function below sourced from geeksforgeeks.org/c-program-check-given-string-palindrome/
int isPalindrome(char str[])
{
  // Start from leftmost and rightmost corners of str
  int l = 0;
  int h = strlen(str) - 1;
  // Keep comparing characters while they are same
  while (h > l && strlen(str) != 1)
  {
    if (str[l++] != str[h--])
    {
      printf("%s is NOT a palindrome.\n", str);
      return 2;
    }
  }
  printf("%s is a palindrome!\n", str);
  return 1;
}

/*int randomTime()
{
  int randomNum = ((rand() % (2 - 0 + 1)));
  return randomNum;
}*/



int main(int argc, char *argv[])
{ 
  //printf("PALIN: Begin program.\n");
  
  char palindrome[80] = { 0 };        // Stores palindrome to check.
  int palinIndex = atoi(argv[1]);     // Contains index to check in shared memory from master.
  //srand(time(0) ^ getpid());          // Seed random time.
  FILE *fptr; 

  // Allocate shared memory
  
  key_t key = ftok("./master", 'j');
  if (key == -1)
  {
    perror("PALIN: Error: ftok failure. \n");
    exit(-1);
  }

  if ((shmid = shmget(key, sizeof(PalArray), 0600 | IPC_CREAT)) == -1)
  {
    perror("PALIN: Error: shmget.\n");
    exit(-1);
  }

  sharedArray = (PalArray *)shmat(shmid, (void *)0, 0);
  if (sharedArray == (void *)(-1))
  {
    perror("PALIN: Error: shmat failure.\n");
    exit(-1);
  }
  
  //int randomSleep = randomTime(); // figure out random time to sleep from 0-2 seconds
  //printf("RANDOM SLEEP: %d \n", randomSleep);
  // Critical Section ------------------------------------------------------------------------ 
  sem_wait(&(sharedArray->mutex));
  // Announce entered cs
  fprintf(stderr, "PALIN: INDEX#: %d CHILD#: %d ENTERED CRITICAL SECTION.\n", palinIndex, getpid());

  // Grab palindrome from specified index
  //printf("Test index palindrome #%d: ", palinIndex);
  int i;
  int j;
  printf("PALIN: INFILE PALINDROME CONTAINS: ");
  for (i = 0; i < 80; i++)
  {
    printf("%c", sharedArray->palinArray[palinIndex * 80 + i]);
    // Remove everything fom array besides alphabet or numbers
    if (isalpha(sharedArray->palinArray[palinIndex * 80 + i]) || isdigit(sharedArray->palinArray[palinIndex * 80 + i]))
    {
      
      palindrome[j] = sharedArray->palinArray[palinIndex * 80 + i];
      //printf("%c", palindrome[j]);
      j++;
    }
  }
  printf("PALIN: STRING STORED TO CHECK PALINDROME STATUS: %s \n", palindrome);

  // Sleep, then check palindrome status and write to palin.out or nopalin.out 
  //printf("PALIN: SLEEPING: %d secs.\n", randomSleep);
  sleep(1);
  int palCheck = isPalindrome(palindrome); 
  if (palCheck == 1)
  {
    //printf("%s is a palindrome. \n", palindrome);
    fptr = fopen("palin.out", "a");
    fprintf(fptr, "PID: %d Index: %d String: %s \n", getpid(), palinIndex, palindrome);
    fclose(fptr);
  }
  if (palCheck == 2)
  {
    //printf("%s is NOT a palindrome. \n", palindrome);
    fptr = fopen("nopalin.out", "a"); 
    fprintf(fptr, "PID: %d Index: %d String: %s \n", getpid(), palinIndex, palindrome);
    fclose(fptr);
  }
  
  fprintf(stderr, "PALIN: INDEX#: %d CHILD#: %d EXITING CRITICAL SECTION.\n", palinIndex, getpid());

  sem_post(&(sharedArray->mutex));
 
  // Detach shared memory.
  if (shmdt (sharedArray) == -1)
  {
    perror("PALIN: Error: shmdt");
    exit(-1);
  }

  return 0;
}
