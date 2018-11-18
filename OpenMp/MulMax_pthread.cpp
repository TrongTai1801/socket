#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
using namespace std;

#define NUM_THREADS 4

//Matrix Stuff: Note this is really bad style, these should be put into a struct
int **matrix;
int **vector;
int **result;
int num_matrix_row = 0;
int num_matrix_column = 0;
int num_vector_row = 0;
int num_vector_column = 0;

int** new_matrix(int rows, int columns)
{
  int **matrix = (int **)malloc(rows * sizeof(int *));
  int i;
  for (i = 0; i < rows; i++)
  {
    matrix[i] = (int *)malloc(columns * sizeof(int));
  }
  return matrix;
}

void free_matrix(int **matrix)
{
  int i;
  for (i = 0; i < 2; i++)
  {
    free(matrix[i]);
  }
  free(matrix);
}

void print_matrix(int **m, int rows, int columns)
{
  int i, j;
  for (i = 0; i < rows; i++)
  {
    for (j = 0; j < columns; j++)
      printf("%d\t", m[i][j]);
    printf("\n");
  }
}

void *multiply(void *id)
{
  int thread_num = *((int *)id);
  //Partition the matrix data into smaller groups
  printf("s = %d\n", thread_num);
  int from = (thread_num * num_matrix_row) / NUM_THREADS;     // note that this 'slicing' works fine
  int to = ((thread_num + 1) * num_matrix_row) / NUM_THREADS; // even if SIZE is not divisible by NUM_THREADS
  printf("to = %d\t", to);
  printf("from = %d\n", from);

  //Compute the result matrix
  int i, j, k;
  for (i = 0; i < num_vector_column; i++)
  {
    for (j = from; j < to; j++)
    {
      result[j][i] = 0;
      for (k = 0; k < num_vector_row; k++)
      {
        result[j][i] = result[j][i] + matrix[j][k] * vector[k][i];
      }
    }
  }
  pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
  pthread_t thread[NUM_THREADS];

  //Matrix Mult
  int i, j = 0;

  //clock_t start, end;
  timeval start, end;
  double dif;

  //Read in data
  printf("\nEnter the number of rows and columns of first matrix: ");
  scanf("%d%d", &num_matrix_row, &num_matrix_column);

  matrix = new_matrix(num_matrix_row, num_matrix_column);
  for (i = 0; i < num_matrix_row; i++)
    for (j = 0; j < num_matrix_column; j++)
      matrix[i][j] = i + j;

  printf("\nEnter the number of rows and columns of second matrix: ");
  scanf("%d%d", &num_vector_row, &num_vector_column);

  //Do some basic error checking
  if (num_matrix_column != num_vector_row)
  {
    printf("Matrices with entered orders can't be multiplied with each other.\n");
    free_matrix(matrix);
    return 1;
  }

  vector = new_matrix(num_vector_row, num_vector_column);
  for (i = 0; i < num_vector_row; i++)
    for (j = 0; j < num_vector_column; j++)
      vector[i][j] = i * j;

  int thread_numbers[NUM_THREADS];
  for (i = 0; i < NUM_THREADS; i++)
  {
    thread_numbers[i] = i;
  }

  result = new_matrix(num_matrix_row, num_vector_column);

  gettimeofday(&start, NULL);
  //start = clock();
  //printf("Start %f\n", (double)start/CLOCKS_PER_SEC);
  for (i = 0; i < NUM_THREADS; i++)
  {
    if (pthread_create(&thread[i], NULL, multiply, (void *)&thread_numbers[i]) != 0)
    {
      perror("Can't create thread");
      free_matrix(matrix);
      free_matrix(vector);
      free_matrix(result);
      exit(-1);
    }
  }

  // main thread waiting for other thread to complete
  for (i = 0; i < NUM_THREADS; i++)
    pthread_join(thread[i], NULL);

  gettimeofday(&end, NULL);
  //end = clock();
  //printf("End %f\n", (double)end/CLOCKS_PER_SEC);
  //Print the results
  //printf("Product of entered matrices: \n");
  //print_matrix(result, num_vector_row, num_vector_column);

  dif = end.tv_sec - start.tv_sec;
  //dif = ((double) (end - start))/CLOCKS_PER_SEC;
  printf("Time %f\n", dif);

  free_matrix(matrix);
  free_matrix(vector);
  free_matrix(result);
  return 0;
}