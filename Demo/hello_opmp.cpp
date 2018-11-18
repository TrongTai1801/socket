#include <stdio.h>
#include <omp.h>

int main(int argc, char const *argv[])
{
    int tid, nthreads;
    // omp_set_num_threads(16);
    tid = omp_get_num_threads();
    #pragma omp parallel
    {
        printf("Hello word\n");
        // if (tid == 0) {
        //     nthreads = omp_get_num_threads();
            printf("I'm the master, Number of thread = %d\n ", tid);
        // }
    }
    // printf("Program exit\n");
    return 0;
}
