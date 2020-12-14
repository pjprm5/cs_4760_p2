// Paul Passiglia
// cs 4760
// Project 2
// 12/12/2020
// Struct for shared memory

#ifndef PALINARRAY_H
#define PALINARRAY_H

#include <semaphore.h>

typedef struct {
  char palinArray[1600]; // holds palindromes read from infile
  sem_t mutex;
} PalArray;

#endif

