#include <stdio.h>
#include <omp.h>

static long num_steps = 100000000;
double step;

#define PAD 8 // This pad is to make sure that each element, is on a different cache line.
#define NUM_THREADS 4 

void main() {
    int i, nthreads;
    double pi, sum[NUM_THREADS][PAD];
    step = 1.0 / (double)num_steps;

    omp_set_num_threads(NUM_THREADS);
    double start = omp_get_wtime();
    #pragma omp parallel
    {
        int i, id, nthrds;
        double x, temp = 0.0;
        id = omp_get_thread_num();
        nthrds = omp_get_num_threads();
        // We are here getting the number of actual threads given to us.
        // As, we may get fewer than expected.
        if(id == 0)
            nthreads = nthrds;
        
        for(i = id, sum[id][0]=0.0; i < num_steps; i = i+nthrds) {
            x = (i + 0.5)*step;
            // Using temp here instead of sum[id], made it much faster.
            // Using sum[id] is slower, becuase for each update, it causes the cache
            // lines to "slosh back and forth". 
            // We can either use this pad method, or the temp.
            // temp += 4.0/(1.0 + x * x);
            sum[id][0]+= 4.0/(1.0 + x * x);
        }
        //sum[id] = temp;
    }
    double end = omp_get_wtime();
    for(i=0, pi=0.0; i < nthreads; i++)
        pi += sum[i][0] * step;
    printf("%f\n", pi);
    printf("Total time: %f\n", end-start);
    printf("Number of threads: %d\n", nthreads);
}
