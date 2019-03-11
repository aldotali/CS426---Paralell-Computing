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
    @Section: Part A1
    @Desc: Writes a serial implementation of a program that takes an
    input file, reads its integers and then sums them up.
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
        //printf("%s\n",fileName);

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

//Given an array and its size it returns the sum of the array elements.
//The caller takes responsibility in handling possible errors on the array itself or 
// the size of the array. In case error corection is needed pass by reference an
//integer in the function parameters and make the signature of the function a boolean.
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

//executes the serial part of the project
void executeSerialPart(int argc, char** argv){
    int i;
    int arraySize;
    int noOfLines;

    if (argc > 2 || argc < 2 ){
        printf("Only 1 argument input is allowed. The input file is needed\n");
    }

    noOfLines = getNumberOfLines(argv[1]);
    int* dynamicArray = (int*) malloc(noOfLines * sizeof(int));
    arraySize = readFileLineByLine(argv[1],dynamicArray);
    //debugging purposses only
    //printf("The num of lines is:%d \n",noOfLines);
    //prettyPrintArray(arraySize,dynamicArray);
    int sum;
    sum = computeSumOfArray(dynamicArray,arraySize);
    //printf("The sum of elements is:%d \n",sum);
    
    //printf("%d\n",sum);
    writeNumToFile(argv[1],sum);
    free(dynamicArray);
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
    clock_t start, end;
    double timeused;
    start = clock();
    executeSerialPart(argc,argv);
    end = clock();
    timeused = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Ellapsed [%f]\n",timeused);
    return 0;
}
