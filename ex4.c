#include <stdio.h>
#include <omp.h>

static long num_steps = 100000000;
double step;

#define NUM_THREADS 4

int main() {
    int i;
    double x=0.0, pi, sum=0.0;

    step = 1.0/(double) num_steps;
    omp_set_num_threads(NUM_THREADS);
    double start = omp_get_wtime();
    #pragma omp parallel for schedule(auto) private(x) reduction(+:sum)
        for(i = 0; i < num_steps; i++) {
            x = (i+0.5) * step;
            sum += 4.0/(1.0 + x * x);
        }
    double end = omp_get_wtime();
    pi = step * sum;
    printf("%f\n", pi);
    printf("Total Time: %f\n", end - start);
    return 0;
}
