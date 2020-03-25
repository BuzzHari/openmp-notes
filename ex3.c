#include <stdio.h>
#include <omp.h>

static long num_steps = 100000000;
double step;

#define NUM_THREADS 4 

void main() {
    int i, nthreads;
    double pi, sum = 0.0; 
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
        
        for(i = id; i < num_steps; i = i+nthrds) {
            x = (i + 0.5)*step;
            // this is bad. Its basically serializing the code.
            // look at ex3b.c for the proper implementation.
#pragma omp critical 
                sum += 4.0/(1.0 + x * x);
        }
    }
    double end = omp_get_wtime();
    pi += sum * step;
    printf("%f\n", pi);
    printf("Total time: %f\n", end-start);
    printf("Number of threads: %d\n", nthreads);
}
