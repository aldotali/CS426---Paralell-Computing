


#ifndef UTILS_H_   /* Include guard */
#define UTILS_H_

/*
    @author: Aldo Tali
    @creds: Senior Computer Engineering Student, Bilkent Uniersity
    @ID: 21500097
    @Course: Parallel Computing - CS426
    @Project01: Spring 2019 - Due 02/06/2019
    @Section: Utils header
    @Desc: Defines all the struct and function definitions needed for the implementation of cuda proejct.
*/ 

struct MatrixStorage {
    int *row_ptr;
    int row_size;

    int *col_ind;
    int col_size;

    double *values;
    int val_size;
};

int* allocateArray(int size);
double* allocateDoubleArray(int size);

void initializeDoubleArray(double* arr, int size, double initalizer);
void initializeArray(int* arr, int size, int initalizer);
void allocateMatrixStorage(struct MatrixStorage* matrixOut, int rows,int values);
void initializeMatrixStorage(struct MatrixStorage* matrixOut, int rows,int values, int initalizer); 
void readMatrix(struct MatrixStorage* matrixOut,char *filename);
void accumulateCounts(int* arr,int size,int val);
void printArray(int* arr, int size);
void printDoubleArray(double* arr, int size);
void matrix_vector_multip(int *row_ptr, int *col_ind, 
            double *values, int rows,int columns, double **x );
#endif