#include <mpi.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
/*
    @author: Aldo Tali
    @creds: Senior Computer Engineering Student, Bilkent Uniersity
    @ID: 21500097
    @Course: Parallel Computing - CS426
    @Project01: Spring 2019 - Due 10/03/2019
    @Section: Part A2
    @Desc: Writes a parallel implementation of a program that takes an
    input file, reads its integers and then sums them up. Master thread
    dispatches its numbers to slaves as equally as possible. The workers
    compute local sums and master aggregates totals.
*/ 

//Gets the numebr of lines in a file
int getNumberOfLines(char* fileName){
    FILE *fileToRead = fopen(fileName,"r");
    int counter =0; 
    char *line = NULL;
    ssize_t singleLine;
    size_t length = 0;


    if (fileToRead == NULL){
        return -1;
    }else{
        //printf("%s\n",fileName);

        while ((singleLine = getline(&line, &length, fileToRead)) != -1) 
        {
            //this line is purely for debugging purposes
            //printf("%s", line);
             counter = counter +1;
        }
        free(line);
        fclose(fileToRead);
    }

    return counter;
}

//This reads a file of integers line by line 
int readFileLineByLine(char* fileName, int* array){
   
    FILE *fileToRead = fopen(fileName,"r");
    char splitter[] = "\n";
    int counter =0; 
    char *line = NULL;
    ssize_t singleLine;
    size_t length = 0;
    char* numberStr;
    int number;


    if (fileToRead == NULL){
        return -1;
    }else{
       // printf("%s\n",fileName);

        while ((singleLine = getline(&line, &length, fileToRead)) != -1) 
        {
            //this line is purely for debugging purposes
            //printf("%s", line);
            
            //get the numbers one by one and put them in the array
            numberStr = strtok(line, splitter);
            if (numberStr == NULL){
                printf("The num: %s\n", line);
            }else {
                //printf("'%s'\n", numberStr); For debugging reasons
                number = atoi(numberStr);
                array[counter] = number;
                counter = counter +1;
            }
        }
        free(line);
        fclose(fileToRead);
    }

    return counter;
}

//calculates the sum of elements in an array
int computeSumOfArray(int* array,int size){
    int i;
    int sum = 0;

    for (i = 0; i < size; i++){
        sum = sum + array[i];
    }
    return sum;
}


//writes a number to a file
void writeNumToFile(char* filename,int num){
    FILE *f = fopen(filename, "w");
    if (f == NULL){
        printf("Error opening file!\n");
        exit(1);
    }
    fprintf(f, "%d\n",num);
    fclose(f);
    
}

//main code for the execution
void executeMpiV1(int argc, char** argv){
    //vars
    int* dynamicArray;
    int arraySize;
    int noOfProcessors;
    int processId;
    int sizeOfChunk;
    int processPartialSum;
    int sum;
    MPI_Status status;

    //constants
    int START_TAG  = 0;
    int END_TAG  = 1;
    int ARRAY_TAG  = 2;
    int NOLINES_TAG = 3;
    int PARTIAL_SUM_TAG = 4;
    double finish;

    MPI_Init(NULL, NULL);
    MPI_Barrier(MPI_COMM_WORLD);
    double begin = MPI_Wtime();
    MPI_Comm_size(MPI_COMM_WORLD, &noOfProcessors);  
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);

    //master process allocates the partitions accordingly
    //computes its own sum and waits for the sums of the other processes
    if (processId == 0){
        int noOfLines;
        noOfLines = getNumberOfLines(argv[1]);
        dynamicArray = (int*) malloc(noOfLines * sizeof(int));
        arraySize = readFileLineByLine(argv[1],dynamicArray);
        int chunks;
        int start;
        int end = 0;
        sizeOfChunk = arraySize/noOfProcessors;
        for (chunks=0; chunks < noOfProcessors-1; chunks++){
            start = end;
            end = start + arraySize/noOfProcessors;
            MPI_Send(&sizeOfChunk, 1, MPI_INT, chunks+1, NOLINES_TAG,MPI_COMM_WORLD);
            //MPI_Barrier(MPI_COMM_WORLD);
            MPI_Send(&dynamicArray[start], sizeOfChunk, MPI_INT, chunks+1, ARRAY_TAG,MPI_COMM_WORLD);
        }

        //master also computes its own sum
        int remainingElements = arraySize - (noOfProcessors-1)*sizeOfChunk;
        sum = computeSumOfArray(&dynamicArray[end],remainingElements);
        //printf("The sum of process [%d] is [%d]\n",processId,sum);

        //collect all sums frm other processes
        for (chunks=0; chunks<noOfProcessors-1; chunks++){
            MPI_Recv(&processPartialSum, 1, MPI_INT, chunks+1, PARTIAL_SUM_TAG, MPI_COMM_WORLD, &status);
            sum = sum + processPartialSum;
        }
        //printf("The sum of process [%d] is [%d]\n",processId,sum);
        printf("%d\n",sum);
        writeNumToFile(argv[1],sum);
    } else {
      
        MPI_Recv(&sizeOfChunk, 1, MPI_INT, 0, NOLINES_TAG, MPI_COMM_WORLD, &status);
        //MPI_Barrier(MPI_COMM_WORLD);
        MPI_Recv(dynamicArray,sizeOfChunk, MPI_INT, 0, ARRAY_TAG, MPI_COMM_WORLD, &status);
       
        //processes compute thir sums
        processPartialSum = computeSumOfArray(dynamicArray,sizeOfChunk);
        //printf("The sum of process [%d] is [%d]\n",processId,processPartialSum);

        //send this sum to master
        MPI_Send(&processPartialSum, 1, MPI_INT, 0, PARTIAL_SUM_TAG,MPI_COMM_WORLD);

    }
    
    MPI_Barrier(MPI_COMM_WORLD);
    //if (processId == 0){
    // finish = MPI_Wtime();
    //}
    
    MPI_Finalize();
}

//prints integer array elements line by line
void prettyPrintArray(int size, int* array){
    int i;
    if (size > 0){
        for (i = 0; i<size; i++){
            printf("%d\n",array[i]);
        } 
    }
}

int main(int argc, char** argv){
    if (argc > 2 || argc < 2 ){
        printf("Only 1 argument input is allowed. The input file is needed\n");
    }
 
    executeMpiV1(argc,argv);
  
    return 0;
}
