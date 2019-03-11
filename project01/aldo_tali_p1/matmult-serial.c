#include <stdio.h>
#include <string.h>
#include <stdlib.h>
/*
    @author: Aldo Tali
    @creds: Senior Computer Engineering Student, Bilkent Uniersity
    @ID: 21500097
    @Course: Parallel Computing - CS426
    @Project01: Spring 2019 - Due 10/03/2019
    @Section: Part B1
    @Desc: Writes a serial implementation of a program that takes two
    input file, reads two matrices and computes the multiplication of them.
*/ 



int ROW_MAJOR = 0;
int COLUMN_MAJOR = 1;

//allocated a 2D arrary with the given dimensions
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

//writes a 2d square matrix to a file
void write2DArrayToFile(char* filename, int** arr, int dim){
    FILE *f = fopen(filename, "w");
    if (f == NULL){
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

//perfom the serial execution of this project
void executeSerialPart(int argc, char** argv){
    if (argc > 4 || argc < 4 ){
        printf("Two argument inputs are allowed. The input matrices are needed\n");
    }
    int **matrix,**matrix2, **result;
    int dim1,dim2;
    dim1 = readMatrixFromFile(argv[1],&matrix,0);
    //prettyPrint2DArray(dim1,dim1,matrix);
    //printf("\n");
    dim2 = readMatrixFromFile(argv[2],&matrix2,0);
    //prettyPrint2DArray(dim2,dim2,matrix2);
    //printf("\n");
    result = multiplySquareMatrices(dim2,dim2,matrix,matrix2);
    //prettyPrint2DArray(dim1,dim1,result);
    //printf("\n");
    write2DArrayToFile(argv[3],result,dim2);
}

int main(int argc, char** argv){
    executeSerialPart(argc,argv);
    return 0;
}