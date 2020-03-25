#include <stdio.h>
#include <omp.h>

int main() {
    // This apparently says give me a bunch of threads. 
    // Gives default number of threads, as we haven't mentioned how many we want.
    // The default number depends on the system.
    #pragma omp parallel 
    {
        // This is structured block. 
        

        // Returns an identifer for each thread. 
        // Ranges from 0 to num_of_threads - 1.
        int ID = omp_get_thread_num(); 
        printf("hello(%d) ", ID);
        printf(" world(%d)  \n", ID);
    }
    return 0;
}

