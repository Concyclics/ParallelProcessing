/* File:     mat_vect_mult.c
 *
 * Purpose:  Implement serial matrix-vector multiplication using
 *           one-dimensional arrays to store the vectors and the
 *           matrix.
 *
 * Input:
 *     None unless compiled with DEBUG flag.
 *     With DEBUG flag, A, x
 *
 * Output:
 *     y: the product vector
 *     Elapsed time for the computation
 *
 * Compile:
 *    openmp: gcc -g -Wall -o mat_vect_mult mat_vect_mult.c -fopenmp
 *    mpi:    mpicc -g -Wall -o mat_vect_mult mat_vect_mult.c
 * Usage:
 *    openmp: mat_vect_mult <m> <n> <thread_count>
 *    mpi:    mpiexec -n <process_count> mat_vect_mult <m> <n> <thread_count>
 *
 * Errors:   if the number of user-input rows or column isn't
 *           positive, the program prints a message and quits.
 * Note:     Define DEBUG for verbose output
 *
 * IPP:      Section 3.4.9 (pp. 113 and ff.), Section 4.3 (pp. 159
 *           and ff.), and Section 5.9 (pp. 252 and ff.)
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void Get_args(int argc, char* argv[], int* thread_count_p, int* m_p, int* n_p);
void Usage(char* prog_name);
void Get_dims(int* m_p, int* n_p);
void Read_matrix(char prompt[], double A[], int m, int n);
void Read_vector(char prompt [], double x[], int n);
void Gen_matrix(double A[], int m, int n);
void Gen_vector(double x[], int n);
void Print_matrix(char title[], double A[], int m, int n);
void Print_vector(char title[], double y[], int m);
void Mat_vect_mult(double A[], double x[], double y[], int m, int n, int thread_count);
double Get_time();

/*------------------------------------------------------------------*/
int main(int argc, char* argv[]) {
    int     thread_count;
    int     m, n;
    double* A;
    double* x;
    double* y;

    Get_args(argc, argv, &thread_count, &m, &n);

    A = malloc(m * n * sizeof(double));
    x = malloc(n * sizeof(double));
    y = malloc(m * sizeof(double));

# ifdef DEBUG
    Read_matrix("Enter the matrix", A, m, n);
    Print_matrix("We read", A, m, n);
    Read_vector("Enter the vector", x, n);
    Print_vector("We read", x, n);
# else
    Gen_matrix(A, m, n);
    /*    Print_matrix("We generated", A, m, n); */
    Gen_vector(x, n);
    /*    Print_vector("We generated", x, n); */
# endif

    double start = Get_time();
    Mat_vect_mult(A, x, y, m, n, thread_count);
    double end = Get_time();

    printf("Time elapsed: %.5lf\n", end - start);

#  ifdef DEBUG
    Print_vector("The product is", y, m);
#  else
    /*    Print_vector("The product is", y, m); */
#  endif

    free(A);
    free(x);
    free(y);

    return 0;
}  /* main */

/*------------------------------------------------------------------
 * Function:  Get_time
 * Purpose:   To get the current time in seconds.
 */
double Get_time() {

#ifdef MPI_INCLUDED
    return MPI_Wtime();
#endif // MPI_INCLUDED

#ifdef _OPENMP
    return omp_get_wtime();
#endif // _OPENMP

#if defined _INC_TIME || defined _TIME_H
    // Not accurate under concurrent environment.
    return ((double) clock()) / CLOCKS_PER_SEC;
#endif // _INC_TIME
    
    fprintf(stderr, "Unable to get the current time.\n");
    fprintf(stderr, "To be able to use Get_time() function, mpi.h, omp.h or time.h should be included.\n");
    exit(1);

}  /* Get_time */

/*------------------------------------------------------------------
 * Function:  Get_args
 * Purpose:   Get command line args
 * In args:   argc, argv
 * Out args:  thread_count_p, m_p, n_p
 */
void Get_args(int argc, char* argv[], int* thread_count_p,
    int* m_p, int* n_p) {

    if (argc != 4) Usage(argv[0]);
    *m_p = strtol(argv[1], NULL, 10);
    *n_p = strtol(argv[2], NULL, 10);
    *thread_count_p = strtol(argv[3], NULL, 10);
    if (*thread_count_p <= 0 || *m_p <= 0 || *n_p <= 0) Usage(argv[0]);

}  /* Get_args */

/*------------------------------------------------------------------
 * Function:  Usage
 * Purpose:   print a message showing what the command line should
 *            be, and terminate
 * In arg :   prog_name
 */
void Usage(char* prog_name) {
    fprintf(stderr, "usage: %s <m> <n> <thread_count>\n", prog_name);
    exit(0);
}  /* Usage */

/*-------------------------------------------------------------------
 * Function:   Get_dims
 * Purpose:    Read the dimensions of the matrix from stdin
 * Out args:   m_p:  number of rows
 *             n_p:  number of cols
 *
 * Errors:     If one of the dimensions isn't positive, the program
 *             prints an error and quits
 */
void Get_dims(
              int*  m_p  /* out */, 
              int*  n_p  /* out */) {
   printf("Enter the number of rows\n");
   scanf("%d", m_p);
   printf("Enter the number of columns\n");
   scanf("%d", n_p);

   if (*m_p <= 0 || *n_p <= 0) {
      fprintf(stderr, "m and n must be positive\n");
      exit(-1);
   }
}  /* Get_dims */

/*-------------------------------------------------------------------
 * Function:   Read_matrix
 * Purpose:    Read the contents of the matrix from stdin
 * In args:    prompt:  description of matrix
 *             m:       number of rows
 *             n:       number of cols
 * Out arg:    A:       the matrix
 */
void Read_matrix(
                 char    prompt[]  /* in  */, 
                 double  A[]       /* out */, 
                 int     m         /* in  */, 
                 int     n         /* in  */) {
   int i, j;

   printf("Enter the matrix %s\n", prompt);
   for (i = 0; i < m; i++)
      for (j = 0; j < n; j++)
         scanf("%lf", &A[i*n+j]);
}  /* Read_matrix */

/*-------------------------------------------------------------------
 * Function:   Read_matrix
 * Purpose:    Read a vector from stdin
 * In args:    prompt:  description of matrix
 *             n:       order of matrix
 * Out arg:    x:       the vector being read in
 */
void Read_vector(
                 char    prompt[]  /* in  */, 
                 double  x[]       /* out */, 
                 int     n         /* in  */) {
   int i;

   printf("Enter the vector %s\n", prompt);
   for (i = 0; i < n; i++)
      scanf("%lf", &x[i]);
}  /* Read_vector */

/*------------------------------------------------------------------
 * Function: Gen_matrix
 * Purpose:  Use the random number generator random to generate
 *    the entries in A
 * In args:  m, n
 * Out arg:  A
 */
void Gen_matrix(double A[], int m, int n) {
    int i, j;
    for (i = 0; i < m; i++)
        for (j = 0; j < n; j++)
            A[i * n + j] = random() / ((double)RAND_MAX);
}  /* Gen_matrix */

/*------------------------------------------------------------------
 * Function: Gen_vector
 * Purpose:  Use the random number generator random to generate
 *    the entries in x
 * In arg:   n
 * Out arg:  A
 */
void Gen_vector(double x[], int n) {
    int i;
    for (i = 0; i < n; i++)
        x[i] = random() / ((double)RAND_MAX);
}  /* Gen_vector */

/*-------------------------------------------------------------------
 * Function:   Print_matrix
 * Purpose:    Print a matrix to stdout
 * In args:    title:  title for output
 *             A:      the matrix
 *             m:      number of rows
 *             n:      number of cols
 */
void Print_matrix(
                  char    title[]  /* in */,
                  double  A[]      /* in */, 
                  int     m        /* in */, 
                  int     n        /* in */) {
   int i, j;

   printf("\nThe matrix %s\n", title);
   for (i = 0; i < m; i++) {
      for (j = 0; j < n; j++)
         printf("%f ", A[i*n+j]);
      printf("\n");
   }
}  /* Print_matrix */

/*-------------------------------------------------------------------
 * Function:   Print_vector
 * Purpose:    Print the contents of a vector to stdout
 * In args:    title:  title for output
 *             y:      the vector to be printed
 *             m:      the number of elements in the vector
 */
void Print_vector(
                  char    title[]  /* in */, 
                  double  y[]      /* in */, 
                  int     m        /* in */) {
   int i;

   printf("\nThe vector %s\n", title);
   for (i = 0; i < m; i++)
      printf("%f ", y[i]);
   printf("\n");
}  /* Print_vector */


/*-------------------------------------------------------------------
 * Function:   Mat_vect_mult
 * Purpose:    Multiply a matrix by a vector
 * In args:    A: the matrix
 *             x: the vector being multiplied by A
 *             m: the number of rows in A and components in y
 *             n: the number of columns in A components in x
 * Out args:   y: the product vector Ax
 */
void Mat_vect_mult(
                   double  A[]  /* in  */, 
                   double  x[]  /* in  */, 
                   double  y[]  /* out */,
                   int     m    /* in  */, 
                   int     n    /* in  */,
                   int     thread_count) {
   int i, j;

   for (i = 0; i < m; i++) {
      y[i] = 0.0;
      for (j = 0; j < n; j++)
         y[i] += A[i*n+j]*x[j];
   }
}  /* Mat_vect_mult */
