#include <stdio.h>
#include <syscall.h>
#include <stdlib.h>

#define barrier() asm volatile ("" : : : "memory")

int 
main(int argc, const char* argv[])
{
  if (argc > 1)
  {
     int loops = atoi(argv[1]);
     int i;
     printf("Loops: argv[1]=%d\n", loops);
     for(i = 0; i < loops; i++)
     {
         barrier();
     }
  }
  printf("Loops: finished.");
  return EXIT_SUCCESS;
}
