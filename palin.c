//Paul Passiglia
//cs_4760
//Project 2
//Date: 10/5/2020


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// isPalindrome function below sourced from geeksforgeeks.org/c-program-check-given-string-palindrome/
void isPalindrome(char str[])
{
  // Start from leftmost and rightmost corners of str
  int l = 0;
  int h = strlen(str) - 1;
  // Keep comparing characters while they are same
  while (h > l)
  {
    if (str[l++] != str[h--])
    {
      printf("%s is not a palindrome.\n", str);
      return;
    }
  }
  printf("%s is a palindrome!.\n", str);
}


int main(int argc, char *argv[])
{
 char str1[80] = "hello";
 char str2[80] = "racecar";
 
 isPalindrome(str1);
 isPalindrome(str2);

 //function check if string is palindrome
 //execute code to enter critical section which is when your writing out to the palin.out and nopalin.out
 //sleep for a random amount of time (between 0 and 2 seconds)
 //execute code to exit from critical section
  
  printf("Testing...\n");
  return 0;
}
