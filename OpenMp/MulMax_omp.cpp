#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

int main(int argc, char *argv[])
{
    int **a, **b, **c;                                 // variables to store allocated memory
    int a_r, a_c, b_r, b_c, nthreads, tid, chunk = 10; //variables to input matrix size and variables to be used by OpenMP functions
    double dif;                                        //variable to calculate the time difference between the parallelization
    int i, j, k;                                       // variables to be used in for loops to generate matrices
again:
    printf("\nEnter rows and columns for matrix one: ");
    scanf("%d%d", &a_r, &a_c);
    printf("\nEnter rows and columns for matrix two: ");
    scanf("%d%d", &b_r, &b_c);
    if (a_c != b_r)
    {
        printf("\nCan not multiply");
        goto again;
    }
    /* allocate memory for matrix one */
    a = (int **) malloc(sizeof(int*) * a_r);
    for (i = 0; i < a_c; i++) {
        a[i] = (int *) malloc(sizeof(int) * a_c);
    }
    /* allocate memory for matrix two */
    b = (int **) malloc(sizeof(int*) * b_r);
    for (i = 0; i < b_c; i++) {
        b[i] = (int *) malloc(sizeof(int) *  b_c);
    }
    /* allocate memory for sum matrix */
    c = (int **) malloc(sizeof(int*) *  a_r);
    for (i = 0; i < b_c; i++) {
        c[i] = (int *) malloc(sizeof(int) *  b_c);
    }
    printf("Initializing matrices...\n");
    double start = omp_get_wtime(); //start the timer
/*** Spawn a parallel region explicitly scoping all variables ***/
#pragma omp parallel shared(a, b, c, nthreads, chunk) private(tid, i, j, k)
    {
        tid = omp_get_thread_num();
        if (tid == 0)
        {
            nthreads = omp_get_num_threads();
            printf("Starting matrix multiple example with %d threads\n", nthreads);
        }
//initializing first matrix
#pragma omp for schedule(static, chunk)
        for (i = 0; i < a_r; i++)
        {
            for (j = 0; j < a_c; j++)
            {
                a[i][j] = i + j;
            }
        }
// initializing second matrix
#pragma omp for schedule(static, chunk)
        for (i = 0; i < b_r; i++)
        {
            for (j = 0; j < b_c; j++)
            {
                b[i][j] = i * j;
            }
        }
/*initialize product matrix */
#pragma omp for schedule(static, chunk)
        for (i = 0; i < a_r; i++)
        {
            for (j = 0; j < b_c; j++)
            {
                c[i][j] = 0;
            }
        }
/*** Do matrix multiply sharing iterations on outer loop ***/
/*** Display who does which iterations for demonstration purposes ***/
//printf("Thread %d starting matrix multiply...\n",tid);
#pragma omp for schedule(static, chunk)
        for (i = 0; i < a_r; i++)
        {
            // printf("Thread=%d did row=%d\n",tid,i);
            for (j = 0; j < a_c; j++)
            {
                for (k = 0; k < b_c; k++)
                {
                    c[i][j] = c[i][j] + a[i][k] * b[k][j];
                }
            }
        }
    } /*****end of parallel region*****/
    printf("******************************************************\n");
    printf("Done.\n");
    double end = omp_get_wtime(); //end the timer
    dif = end - start;            //store the difference
    printf("Parallelization took %f sec. time.\n", dif);
    /*free memory*/
    for (i = 0; i < a_r; i++)
    {
        free(a[i]);
    }
    free(a);
    for (i = 0; i < a_c; i++)
    {
        free(b[i]);
    }
    free(b);
    for (i = 0; i < b_c; i++)
    {
        free(c[i]);
    }
    free(c);
}