#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
/*
    @author: Aldo Tali
    @creds: Senior Computer Engineering Student, Bilkent Uniersity
    @ID: 21500097
    @Course: Parallel Computing - CS426
    @Project01: Spring 2019 - Due 10/03/2019
    @Section: Part B2
    @Desc: Writes a parallell implementation of a program that takes two
    input file, reads two matrices and computes the multiplication of them.
*/ 


int ROW_MAJOR = 0;
int COLUMN_MAJOR = 1;

void allocate2DArray(int*** array,int dim1,int dim2){
    
    int i; 
  
    *array = (int **)malloc(dim1 * sizeof(int *)); 
    for (i=0; i<dim1; i++) 
        (*array)[i] = (int *)malloc(dim2 * sizeof(int)); 
}

//Reads a file with a square matrix. Given that the line precedding the matrix
//gives the dimension of the respective matrix, it assigns the integers in 2d arrays
//accordingly.
int readMatrixFromFile(char* fileName,int*** matrix,int major){
    FILE *fileToRead = fopen(fileName,"r");
    char *line = NULL;
    char linesplitter[] = "\n";
    char spacesplitter[] = " ";
    ssize_t singleLine;
    size_t length = 0;
    int dimension = 0;
    char* numberStr;
    char* singleEntry;
    int number;
    int i,j;

    if (fileToRead == NULL){
        return -1;
    }else{
        i = -1;
        while ((singleLine = getline(&line, &length, fileToRead)) != -1) {
            
            //get the numbers one by one and put them in the array
            numberStr = strtok(line, linesplitter);
            if (numberStr == NULL){
                printf("The num: %s\n", line);
            }else {
                //printf("'%s'\n", numberStr); For debugging reasons
                if (dimension == 0){
                    number = atoi(numberStr);
                    dimension = number;
                    allocate2DArray(matrix,dimension,dimension);
                } else {
                    j = 0;
                    singleEntry = strtok(numberStr, spacesplitter);
                    while( singleEntry != NULL ) {   
                        number = atoi(singleEntry);
                        //printf(" %d",number);
                        if (major == ROW_MAJOR){
                            (*matrix)[i][j] = number;
                        } else {
                            if (major == COLUMN_MAJOR){
                                (*matrix)[j][i] = number;
                            }
                        }
                        
                        singleEntry = strtok(NULL, spacesplitter);
                        j++;
                    }
                    //printf("\n");
                }
            }
            i++;
        }
        free(line);
        fclose(fileToRead);
    }
    return dimension;
}

//prints integer array elements line by line
void prettyPrintArray(int size, int* array){
    int i;
    if (size > 0){
        for (i = 0; i<size; i++){
            printf("%d ",array[i]);
        } 
    }
}

//prints integer array elements line by line
void prettyPrint2DArray(int dim1,int dim2, int** array){
    int i,j;
    if (dim1 > 0 && dim2>0){
        for (i = 0; i<dim1; i++){
            prettyPrintArray(dim2,array[i]);
            printf("\n");
        } 
    }
}

//multplies two matrices. Note operation is in O(n3)
int** multiplySquareMatrices(int dim1,int dim2, int** mat1, int** mat2){
    int i, j, k;
    int **result;

    allocate2DArray(&result,dim1,dim2);
    for (i = 0; i < dim1;i++){
        for (j = 0; j < dim2; j++){
            result[i][j] = 0;
            for (k = 0; k < dim1; k++){
                result[i][j] = result[i][j] + mat1[i][k] * mat2[k][j];
            } 
        }
    }
    return result;
}

//does vector multiplication of two arrays
int multiplyVectors(int dim1,int dim2, int* vec1, int* vec2){
    int i, j, k;
    int result = 0;

    for (i = 0; i < dim1;i++){
        result = result + vec1[i]*vec2[i];
    }
    return result;
}

//copy array from source to destination (deep copy)
void copyArray(int* source, int** dest, int size){
    int i;
    //*dest = (int *)malloc(size * sizeof(int));
    for (i = 0; i < size; i++){
        (*dest)[i]=source[i];
    }
}

//writes a 2d square matrix to a file
void write2DArrayToFile(char* filename, int** arr, int dim){
    FILE *f = fopen(filename, "w");
    if (f == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }
    fprintf(f, "%d\n",dim);
    int i,j;
    for (i= 0; i < dim; i++){
        for (j = 0; j<dim; j++){
            fprintf(f, "%d ",arr[i][j]);
        }
        fprintf(f,"\n");
    }
    fclose(f);
    
}

int executeMPI(int argc, char** argv){
    int ROW_TAG  = 0;
    int COLUMN_TAG  = 1;
    int ARRAY_TAG  = 2;
    int DIM_TAG = 3;
    int OUTPUT_TAG = 4;
    int ROW_INDEX_TAG = 5;
    int COLUMN_INDEX_TAG = 6;
    MPI_Status status;

    int **matrix,**matrix2,**result;
    int dim1,dim2;
    int noOfProcessors;
    int processId;
    int chunks;
    int i,j,k,counter,rowIndex,columnIndex,singleEntry;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &noOfProcessors);  
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);


    if (processId == 0){
        dim1 = readMatrixFromFile(argv[1],&matrix,0);
        if (noOfProcessors == 1){
            dim2 = readMatrixFromFile(argv[2],&matrix2,0);
            result = multiplySquareMatrices(dim2,dim2,matrix,matrix2);
        }else {
            dim2 = readMatrixFromFile(argv[2],&matrix2,1);
            allocate2DArray(&result,dim1,dim2);
        if (dim1 != dim2){
            printf("The matrices given as input are not the same dimension. The behaviour cannoit be predicted\n");
        }
        int matrixrow,matrixcolum,gridIndex,processIdrow,pid;
        //assumes input is perfect square
        int procDim = sqrt(noOfProcessors);
        int noOfGrids = dim1/ procDim;

        for (processIdrow = 1; processIdrow < procDim*procDim; processIdrow++){
            MPI_Send(&dim1, 1, MPI_INT, processIdrow, DIM_TAG,MPI_COMM_WORLD);
        }
        for (gridIndex = 0; gridIndex<noOfGrids; gridIndex++){
            for (pid = 0; pid < procDim*procDim; pid++){
                matrixrow = gridIndex*procDim + pid/procDim;
                matrixcolum = gridIndex*procDim + pid%procDim;
                if (pid==0){
                }else {
                    MPI_Send(matrix[matrixrow], dim1, MPI_INT,pid , ROW_TAG,MPI_COMM_WORLD);
                    MPI_Send(matrix2[matrixcolum], dim1, MPI_INT,pid , COLUMN_TAG,MPI_COMM_WORLD);
                }

            }
        }
        int gridDim,resultx,resulty;
        for (gridDim =0 ; gridDim<noOfGrids; gridDim++){
            for (gridIndex =0 ; gridIndex < noOfGrids; gridIndex++){
                for (pid = 0; pid < procDim*procDim; pid++){    
                    if (pid == 0){
                        matrixrow = gridIndex*procDim + pid/procDim;
                        matrixcolum = gridIndex*procDim + pid%procDim;
                        singleEntry = multiplyVectors(dim1,dim1,matrix[matrixrow],matrix2[matrixcolum]);
                        resultx = gridDim* procDim;
                        resulty = gridIndex* procDim;
                        result[resultx][resulty] = singleEntry;
                    }else {
                        MPI_Recv(&rowIndex,1, MPI_INT, pid, ROW_INDEX_TAG, MPI_COMM_WORLD, &status);
                        MPI_Recv(&columnIndex,1, MPI_INT, pid, COLUMN_INDEX_TAG, MPI_COMM_WORLD, &status);
                        MPI_Recv(&singleEntry,1, MPI_INT, pid, OUTPUT_TAG, MPI_COMM_WORLD, &status);
                        resultx = gridDim* procDim + pid/procDim;
                        resulty = gridIndex* procDim + pid%procDim;
                        result[resultx][resulty] = singleEntry;
                    }
                }

            }
        }
        }
     write2DArrayToFile(argv[3],result,dim1);
    }else {
        //recieve the 2 arrays
        //get singleEntry
        //send single entry to master
        int matrixrow,gridIndex,processIdrow,dim;
        int **rows;
        int **columns;
        //assumes input is perfect square
        MPI_Recv(&dim, 1, MPI_INT, 0, DIM_TAG, MPI_COMM_WORLD, &status);
        int procDim = sqrt(noOfProcessors);
        int noOfGrids = dim/ procDim;
        allocate2DArray(&rows,noOfGrids,noOfGrids);
        allocate2DArray(&columns,noOfGrids,noOfGrids);

        for (gridIndex = 0; gridIndex<noOfGrids; gridIndex++){
            MPI_Recv(columns[gridIndex],dim, MPI_INT, 0, ROW_TAG, MPI_COMM_WORLD, &status);
            copyArray(columns[gridIndex], &rows[gridIndex],dim);
            MPI_Recv(columns[gridIndex],dim, MPI_INT, 0, COLUMN_TAG, MPI_COMM_WORLD, &status);
        }
        for (rowIndex = 0; rowIndex<noOfGrids; rowIndex++){
            for (columnIndex = 0; columnIndex<noOfGrids; columnIndex++){
                singleEntry = multiplyVectors(dim,dim,rows[rowIndex],columns[columnIndex]);
                MPI_Send(&rowIndex, 1, MPI_INT, 0, ROW_INDEX_TAG,MPI_COMM_WORLD);
                MPI_Send(&columnIndex, 1, MPI_INT, 0, COLUMN_INDEX_TAG,MPI_COMM_WORLD);
                MPI_Send(&singleEntry, 1, MPI_INT, 0, OUTPUT_TAG,MPI_COMM_WORLD);
            }
        }
    }

    MPI_Finalize();

    
}

int main(int argc, char** argv){
    return executeMPI(argc,argv);
}
