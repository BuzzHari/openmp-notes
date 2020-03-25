// omp_get_num_threads(); Number threads in the team.
// omp_get_thread_num(); Thread ID or rank
// double omp_get_wtime(); time in seconds since a fixed point in the past.

#include <stdio.h>
#include <omp.h>

const static long num_steps = 10000000;
double step = 1.0/(double) num_steps;

double calcSum(int start, int end) {
    printf("In thread: %d\n", omp_get_thread_num());
    double x;
    double sum = 0.0;
    for (int i = start; i < end ; i++){
        x = (i + 0.5) * step;
        sum = sum + 4.0/(1.0 + x * x);
    }
    return sum;
}

int main() {
    int i; 
    double x, pi;
    double sum1, sum2, sum3, sum4; 
    double start_time = omp_get_wtime();
    #pragma omp parallel 
    {
        sum1 = calcSum(0, num_steps/4);
        sum2 = calcSum(num_steps/4, num_steps/2);
        sum3 = calcSum(num_steps/2, num_steps/2 + num_steps/4);
        sum4 = calcSum(num_steps/2+num_steps/4, num_steps);
    } 
    double end_time = omp_get_wtime();
    pi = step * (sum1 + sum2 + sum3 + sum4);
    printf("%lf\n", pi);
    printf("Total time: %f\n", (end_time - start_time));
    return 0;
}

