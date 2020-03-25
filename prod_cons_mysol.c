/*
**  PROGRAM: A simple serial producer/consumer program
**
**  One function generates (i.e. produces) an array of random values.  
**  A second functions consumes that array and sums it.
**
**  HISTORY: Written by Tim Mattson, April 2007.
*/
#include <omp.h>
#ifdef APPLE
#include <stdlib.h>
#else
#include <malloc.h>
#endif
#include <stdio.h>

#define N        100000000

/* Some random number constants from numerical recipies */
#define SEED       2531
#define RAND_MULT  1366
#define RAND_ADD   150889
#define RAND_MOD   714025
int randy = SEED;

/* function to fill an array with random numbers */
void fill_rand(int length, double *a)
{
   int i; 
   for (i=0;i<length;i++) {
     randy = (RAND_MULT * randy + RAND_ADD) % RAND_MOD;
     *(a+i) = ((double) randy)/((double) RAND_MOD);
   }   
}

/* function to sum the elements of an array */
double Sum_array(int length, double *a)
{
   int i;  double sum = 0.0;
   for (i=0;i<length;i++)  sum += *(a+i);  
   return sum; 
}
  
int main()
{
  double *A, sum, runtime;
  int flag = 0, flg_temp = 0;

  A = (double *)malloc(N*sizeof(double));

  runtime = omp_get_wtime();
#pragma omp parallel sections 
  {
#pragma omp section
      {
        fill_rand(N, A);        // Producer: fill an array of data
#pragma omp flush // flush the make the value of array A visible to other thread.
#pragma omp atomic write
        flag = 1;
#pragma omp flush(flag) // flushing the value of flag, to make it visible to other thread.
      }
#pragma omp section
      {
#pragma omp flush(flag) // Making sure getting right value of flag.
          while(1){
              // Checking for changes in flag, in a loop - spinlock.
              // This flush is required, because the compiler might put the value of the flag in 
              // a register, as it's not changing in this block, and always pull the value from
              // that register, hence never seeing the new value of flag.
              // This flush, forces the compiler to go down the memory heirarchy and grap the fresh
              // value of flag.
#pragma omp flush(flag) 
#pragma omp atomic read
              flg_temp = flag;
              if(flg_temp) break;
          }
          // After we see the value of flag change, we flush again, to get update everything.
#pragma omp flush
          sum = Sum_array(N, A);  // Consumer: sum the array

      }
  } 
  runtime = omp_get_wtime() - runtime;

  printf(" In %f seconds, The sum is %f \n",runtime,sum);
}
 
