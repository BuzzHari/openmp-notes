#include <stdio.h>
#include <omp.h>

static long num_steps = 100000000;
double step;

int main() {
    int i;
    double x, pi, sum=0.0;
   
    step = 1.0/(double)num_steps;
    
    double start = omp_get_wtime();
    for(i = 0; i < num_steps; i++) {
        x = (i+0.5) * step;
        sum = sum + 4.0/(1.0 + x * x);
    }
    double end = omp_get_wtime();
    
    pi = step * sum;
    printf("%f\n", pi);
    printf("Totat Time:%f\n", end - start);
    return 0;
}
