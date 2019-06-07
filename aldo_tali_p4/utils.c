


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>

/*
    @author: Aldo Tali
    @creds: Senior Computer Engineering Student, Bilkent Uniersity
    @ID: 21500097
    @Course: Parallel Computing - CS426
    @Project01: Spring 2019 - Due 07/04/2019
    @Section: Utils
    @Desc: Defines all the struct and function implementations needed for the implementation of kreduce.
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

int* allocateArray(int size){
    int *arr;
    arr = (int *)malloc(sizeof(int) * size);
    return arr;
}
  
double* allocateDoubleArray(int size){
    double *arr;
    arr = (double *)malloc(sizeof(double) * size);
    return arr;
}

void initializeArray(int* arr, int size, int initalizer){
    int i;
    for (i = 0; i < size; i++){
        arr[i] = initalizer;
    }
}
void initializeDoubleArray(double* arr, int size, double initalizer){
    int i;
    for (i = 0; i < size; i++){
        arr[i] = initalizer;
    }
}

void allocateMatrixStorage(struct MatrixStorage* matrixOut, int rows,int values){
    matrixOut->row_ptr =  allocateArray(matrixOut->row_size);
    matrixOut->col_ind =  allocateArray(matrixOut->val_size);
    matrixOut->values =  allocateDoubleArray(matrixOut->val_size);
}

void initializeMatrixStorage(struct MatrixStorage* matrixOut, int rows,int values, int initalizer){
    initializeArray(matrixOut->row_ptr, matrixOut->row_size, 0);
    initializeArray(matrixOut->col_ind, matrixOut->val_size, matrixOut->val_size);
    initializeDoubleArray(matrixOut->values, matrixOut->val_size, (double) matrixOut->val_size);
}

void readMatrix(struct MatrixStorage* matrixOut,char *filename){
    FILE * fileToRead;
    int  i;
    int row_index,col_index;
    double val;

    fileToRead = fopen(filename,"r");             //opens the file

    if (fileToRead == NULL){
         return ;
    }else{
    
        fscanf(fileToRead,"%d %d %d", &(matrixOut->row_size), &(matrixOut->col_size), &(matrixOut->val_size));
        allocateMatrixStorage(matrixOut, matrixOut->row_size,matrixOut->val_size);
        initializeMatrixStorage(matrixOut,matrixOut->row_size,matrixOut->val_size, matrixOut->val_size);

        for (i = 0; i < matrixOut->val_size; i++)
        {   
            fscanf(fileToRead,"%d %d %lg", &row_index, &col_index, &val);
            //if (row_index == 5)
            //printf("%d,%d\n", matrixOut->row_ptr[row_index-1],row_index);
            matrixOut->row_ptr[row_index-1] =  matrixOut->row_ptr[row_index-1] + 1;	
            matrixOut->values[i] = val;
            matrixOut->col_ind[i] = col_index -1;
        } 
        //printf("\n");
    }
    fclose(fileToRead);
}

void accumulateCounts(int* arr,int size,int val){
    int i;
    //printf("%d\n", size);
    if (arr[size-1] == 0){
        arr[size-1] = val;
    }
    else{
         arr[size-1] = val - arr[size-1];
    }
    
    for (i = size-2; i >=0; i--){
        if (arr[i] == 0)
            arr[i] = arr[i+1];
        else {
            arr[i] = arr[i+1] - arr[i];
        }
    }
}

void printArray(int* arr, int size){
     
    int i;
    for (i = 0; i < size; i++){
        printf("%d\n", arr[i]);
    }
    printf("\n");
}

void printDoubleArray(double* arr, int size){
    int i;
    for (i = 0; i < size; i++){
        printf("%lf\n", arr[i]);
    }
    printf("\n");
}

void matrix_vector_multip(int *row_ptr, int *col_ind, 
            double *values, int rows,int columns, double **x ){
    int i,j;
    double val = 0.0;
    
    double *newArr = allocateDoubleArray(rows);
    initializeDoubleArray(newArr,rows,0.0);
    
    double *tmpX = *x;

    for (i = 0; i < rows-1; i++){
        if (row_ptr[i] != row_ptr[i+1]){
            for (j = row_ptr[i]; j < row_ptr[i+1];j++){
                val = val + tmpX[col_ind[j]]*values[j];
            }
            newArr[i] = val;
        } else {
            newArr[i] = 0.0;
        }
        val = 0.0;
    }
    
    //printArray(col_ind,columns);
    i = rows -1;
    if (row_ptr[i] < rows){
        for (j = row_ptr[i]; j < columns;j++){
            
            val = val + tmpX[col_ind[j]]*values[j];
        }
        newArr[i] = val;
        
    }else {
        newArr[i] = 0.0;
    }
    val = 0.0;
    
    *x = newArr;
    newArr = NULL;
    free(tmpX);
}

