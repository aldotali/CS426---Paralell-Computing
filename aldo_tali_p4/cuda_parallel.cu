//#include "utils.cuh"
#include <stdio.h>
#include <stdlib.h>
#include <cuda.h>
#include <cuda_runtime.h>
#include <sys/time.h>
#include <string.h>
#include <math.h>
#include <limits.h>

#define BLOCKSIZE 1


struct MatrixStorage {
    int *row_ptr;
    int row_size;

    int *col_ind;
    int col_size;

    double *values;
    int val_size;
};

__host__ int* allocateArray(int size);
__host__ double* allocateDoubleArray(int size);

__host__ void initializeDoubleArray(double* arr, int size, double initalizer);
__host__ void initializeArray(int* arr, int size, int initalizer);
//void allocateMatrixStorage(struct MatrixStorage* matrixOut, int rows,int values);
__host__ void allocateMatrixStorage(int** row_ptr, int **col_ind,double **values, int row_size, int col_size, int val_size);
__host__  void initializeMatrixStorage(int **row_ptr, int **col_ind,double **values, int row_size, int col_size, int val_size);
__host__ void readMatrix(int **row_ptr, int **col_ind,double **values,
            int &row_size, int &col_size, int &val_size, char *filename);
__host__ void accumulateCounts(int* arr,int size,int val);
__host__ void printArray(int* arr, int size);
__host__ void printDoubleArray(double* arr, int size);
__host__ void matrix_vector_multip(int *row_ptr, int *col_ind, 
            double *values, int rows,int columns, double **x );

__host__ int* allocateArray(int size){
    int *arr;
    arr = (int *)malloc(sizeof(int) * size);
    return arr;
}
  
__host__ double* allocateDoubleArray(int size){
    double *arr;
    arr = (double *)malloc(sizeof(double) * size);
    return arr;
}

__host__ void initializeArray(int* arr, int size, int initalizer){
    int i;
    for (i = 0; i < size; i++){
        arr[i] = initalizer;
    }
}
__host__ void initializeDoubleArray(double* arr, int size, double initalizer){
    int i;
    for (i = 0; i < size; i++){
        arr[i] = initalizer;
    }
}

__host__ void allocateMatrixStorage(int** row_ptr, int **col_ind,double **values, int row_size,int col_size,int val_size){
    *row_ptr =  allocateArray(row_size);
    *col_ind =  allocateArray(val_size);
    *values =  allocateDoubleArray(val_size);
}

__host__ void initializeMatrixStorage(int** row_ptr, int **col_ind,double **values, int row_size,int col_size,int val_size){ 
    initializeArray(*row_ptr, row_size, 0);
    initializeArray(*col_ind, val_size, val_size);
    initializeDoubleArray(*values, val_size, (double) val_size);
}

__host__  void readMatrix(int **row_ptr, int **col_ind,double **values,
            int &row_size, int &col_size, int &val_size, char *filename){
   
    FILE * fileToRead;
    int  i;
    int row_index,col_index;
    double val;

    
    fileToRead = fopen(filename,"r");             //opens the file

    if (fileToRead == NULL){
         printf("file cant be found\n");
         return ;
    }else{
    
        fscanf(fileToRead,"%d %d %d", &(row_size), &(col_size), &(val_size));
        allocateMatrixStorage( row_ptr, col_ind, values, row_size,col_size,val_size);
        initializeMatrixStorage(row_ptr, col_ind, values,row_size,col_size, val_size);

        for (i = 0; i < val_size; i++)
        {   
            fscanf(fileToRead,"%d %d %lg", &row_index, &col_index, &val);
            //if (row_index == 5)
            //printf("%d,%d\n", row_ptr[row_index-1],row_index);
            (*row_ptr)[row_index-1] =  (*row_ptr)[row_index-1] + 1;	
            (*values)[i] = val;
            (*col_ind)[i] = col_index -1;
        } 
        //printf("\n");
    }
    fclose(fileToRead);
}

__host__ void accumulateCounts(int* arr,int size,int val){
    int i;
   
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

__host__ void printArray(int* arr, int size){
     
    int i;
    for (i = 0; i < size; i++){
        printf("%d\n", arr[i]);
    }
    printf("\n");
}

__host__ void printDoubleArray(double* arr, int size){
    int i;
    for (i = 0; i < size; i++){
        printf("%lf\n", arr[i]);
    }
    printf("\n");
}

__host__ void matrix_vector_multip(int *row_ptr, int *col_ind, 
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
__global__ void initializeDoubleArrayDev(double* arr, int size, double initalizer){
    int i;
    for (i = 0; i < size; i++){
        arr[i] = initalizer;
    }
}

__global__ void matrix_vector_multip_dev(int *row_ptr, int *col_ind, 
            double *values, int *rowsArr,int *columnsArr, double *x, double *newArr,int *single_block_size ){
    int block_size = single_block_size[0];
    int x_tid = blockIdx.x*blockDim.x + threadIdx.x;
    int y_tid = blockIdx.y*blockDim.y + threadIdx.y;
    int thread_index = x_tid + gridDim.x*block_size*y_tid;       
    

    int j;
    int rows = rowsArr[0];
    int columns = columnsArr[0];
    double val = 0.0;
    int ofset = rows * thread_index; 

    if (thread_index < rows){
        newArr[thread_index] = 0.0;
    }
    if (thread_index < rows -1 ){
        if (row_ptr[thread_index] != row_ptr[thread_index+1]){
            for (j = row_ptr[thread_index]; j < row_ptr[thread_index+1];j++){
                    val = val + x[col_ind[j]] *values[j];
            }
            newArr[thread_index] += val;
        }else {
            newArr[thread_index] += 0.0;
        }
    } else {
        if (thread_index == rows -1 && row_ptr[thread_index] < rows){
            for (j = row_ptr[thread_index]; j < columns;j++){
                val = val + x[col_ind[j]] *values[ofset + j];
            }
            newArr[thread_index] += val;
        }else {
            newArr[thread_index] += 0.0;
        }
    }

   
   __syncthreads();
    
}


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

    int *d_rowptr, *d_col_ind;
    double *d_values;
    int *d_rows, *d_columns;
    int *row_ptr, *col_ind, *single_block_size;
    double *values;
    int row_size, col_size, val_size;

    double * man_x, *man_x_result;
    double * d_man_x, *d_man_x_result;
 
    readMatrix(&row_ptr,&col_ind,&values,row_size,col_size,val_size ,filename);
    accumulateCounts(row_ptr,row_size,val_size);
    
    
    cudaMalloc((void**)&d_rowptr, sizeof(int) * row_size);
    cudaMalloc((void**)&d_col_ind, sizeof(int) * val_size);
    cudaMalloc((void**)&d_values, sizeof(double) * val_size);
    cudaMalloc((void**)&d_rows, sizeof(int));
    cudaMalloc((void**)&d_columns, sizeof(int));
    cudaMalloc((void**)&single_block_size, sizeof(int));
   
    cudaMalloc((void**)&man_x, sizeof(double) * row_size);
    cudaMalloc((void**)&man_x_result, sizeof(double) * row_size);
    cudaMalloc((void**)&d_man_x, sizeof(double) * row_size);
    cudaMalloc((void**)&d_man_x_result, sizeof(double) * row_size);


    man_x = allocateDoubleArray(row_size);
    man_x_result = allocateDoubleArray(row_size);
    initializeDoubleArray(man_x,row_size,1.0);
 
   
    cudaMemcpy(d_rowptr, row_ptr, sizeof(int) * row_size, cudaMemcpyHostToDevice);
    cudaMemcpy(d_col_ind, col_ind, sizeof(int) * val_size, cudaMemcpyHostToDevice);
    cudaMemcpy(d_values, values, sizeof(double) * val_size, cudaMemcpyHostToDevice);
    cudaMemcpy(d_rows, &row_size, sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(d_columns, &col_size, sizeof(int), cudaMemcpyHostToDevice);
    
    int block_size = BLOCKSIZE;
    int a_block_can_hold =block_size*block_size;
    int blocks_in_a_grid;
    if (row_size % a_block_can_hold != 0){
        blocks_in_a_grid=row_size/a_block_can_hold+1;
    }else {
        blocks_in_a_grid=(row_size/a_block_can_hold+1)-1;
    }
    
    int grids = noThreads /(blocks_in_a_grid*a_block_can_hold);

    if (noThreads%(blocks_in_a_grid*a_block_can_hold)>0){
        grids++;
    }

    dim3 dimBlock(block_size,block_size);
    dim3 dimGrid(grids,blocks_in_a_grid);

    double *pt;
    cudaMemcpy(single_block_size , &block_size, sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(d_man_x, man_x, sizeof(double) * row_size, cudaMemcpyHostToDevice);
    
    struct timeval t1, t2;

    gettimeofday(&t1, 0);
    for(int iter=0; iter<number_of_iter; iter++) {
        matrix_vector_multip_dev<<<dimGrid,dimBlock>>>(d_rowptr, d_col_ind,d_values, d_rows,d_columns, d_man_x,d_man_x_result,single_block_size );
        cudaDeviceSynchronize(); 
        pt = d_man_x;
        d_man_x = d_man_x_result;
        d_man_x_result = pt;
    }
    gettimeofday(&t2, 0);

    double time = (1000000.0*(t2.tv_sec-t1.tv_sec) + t2.tv_usec-t1.tv_usec)/1000.0;

    //printf("Time to generate:  %3.11f ms \n", time);
  
    cudaMemcpy(man_x, d_man_x, sizeof(double) * row_size, cudaMemcpyDeviceToHost);

    
    if (print_what == 1){
        printDoubleArray(values,val_size);
        printArray(col_ind,val_size);
        printArray(row_ptr,row_size);
    }
    if (print_what == 2){
        printDoubleArray(man_x,row_size);
    }
    
    return 0;
}