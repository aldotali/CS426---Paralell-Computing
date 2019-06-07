#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

int main(int argc, char** argv){

     

    int noThreads,number_of_iter,print_what;
    char *filename;

    if (argc < 5){
        printf("Please enter the all the following arguments:\n");
        printf("1. The number of threads used to compute Matrix-vector product\n");
        printf("2. The number of repetitions \n");
        printf("3. An argument to print on stdout \n");
        printf("4. Test-file name\n");
        exit(1);
    }else {
        noThreads = atoi(argv[1]);
        number_of_iter = atoi(argv[2]);
        print_what =  atoi(argv[3]);
        filename = argv[4];
    } 

    //printf("%d,%d,%d,%s\n",noThreads,number_of_iter,print_what,filename);

    struct MatrixStorage matrix;
    double *x;
 
    readMatrix(&matrix,filename);
    
    accumulateCounts(matrix.row_ptr,matrix.row_size,matrix.val_size);

    x = allocateDoubleArray(matrix.row_size);
    initializeDoubleArray(x,matrix.row_size,1.0);
 
    struct timeval t1, t2;

    gettimeofday(&t1, 0);
    for(int iter=0; iter<number_of_iter; iter++) {
        matrix_vector_multip(matrix.row_ptr, matrix.col_ind, matrix.values, matrix.row_size,matrix.col_size, &x );
    }
    gettimeofday(&t2, 0);

    double time = (1000000.0*(t2.tv_sec-t1.tv_sec) + t2.tv_usec-t1.tv_usec)/1000.0;

    //printf("Time to generate:  %3.1f ms \n", time);
    if (print_what == 2){
         printDoubleArray(x,matrix.row_size);
    }
   

    return 0;
}