#include "utils.h"
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

//readQuery reads a query file and puts it in the query array
int* readQuery(char* filename, int size){
    int* array = (int*) malloc(size * sizeof(int));
    FILE *fileToRead = fopen(filename,"r");
    char splitter[] = "\n";             //serves to chunk at each line
    char spacesplitter[] = " "; 
    int index =0;                       //serves to refer to 1 query
    char *line = NULL; 
    ssize_t singleLine;
    size_t length = 0;
    char* numberStr;                    //refers to the number in the files as a string
    int number;                         //the actual query number


    if (fileToRead == NULL){
        return NULL;
    }else{
        while ((singleLine = getline(&line, &length, fileToRead)) != -1) 
        {   
            //get the numbers one by one and put them in the array
            numberStr = strtok(line, splitter);
            if (numberStr == NULL){
                printf("The num: %s\n", line);
            }else {
                numberStr = strtok(numberStr, spacesplitter);
                while( numberStr != NULL ) {   
                    //printf("'%s'\n", numberStr); //For debugging reasons
                    number = atoi(numberStr);
                    numberStr = strtok(NULL, spacesplitter);
                    array[index] = number;
                    index = index +1;
                }
            }
        }
        free(line);
        //free(numberStr);
        fclose(fileToRead);
    }
    //free(fileToRead);
    return array;
}

//getNumberOfLines counts the number of lines there are in a file
int getNumberOfLines(char* fileName){
    FILE *fileToRead = fopen(fileName,"r");             //opens the file
    int counter =0; 
    char *line = NULL;
    ssize_t singleLine;
    size_t length = 0;

    if (fileToRead == NULL){
        return -1;
    }else{
        while ((singleLine = getline(&line, &length, fileToRead)) != -1) { //gets each line
             counter = counter +1;
        }
        free(line);
        fclose(fileToRead);
    }
    //free(fileToRead);                       //frees memory
    return counter;
}
//prints integer array elements line by line
void prettyPrintArray(int size, int* array){
    int i;
    if (size > 0){
        for (i = 0; i<size; i++){
            printf("%d\t",array[i]);
        } 
        printf("\n");
    }

}

//prints integer array elements line by line
void prettyPrint2DArray(int dim1,int dim2, int** array){
    int i,j;
    if (dim1 > 0 && dim2>0){
        for (i = 0; i<dim1; i++){
            prettyPrintArray(dim2,array[i]);
           // printf("\n");
        } 
    }
}
//alloactes a 2D array of size dimmensions dim1 and dim 2
void allocate2DArray(int dim1,int dim2, int*** array){
    int i;
    *(array) = (int **)malloc(dim1 * sizeof(int *)); 
    for (i=0; i<dim1; i++) 
        (*array)[i] = (int *)malloc(dim2 * sizeof(int)); 
}

//computeSimilarity computes the similarity betweeen a documment and a query
int computeSimilarity(int* doc, int* query, int dictSize){
    int i, sim = 0;

    for (i = 0; i < dictSize; i++){
        sim = sim + pow(doc[i],query[i]);
    }

    return sim;
}

void deallocate2D(int** array, int size){
    int i;
    for (i=0; i < size; i++){
        free(array[i]);
    }
    free(array);
}

//readDocuments reads all the documeeents and puts them into an array
void readDocuments(char* filename, int dictSize,struct Docs* p, int noProc){

    FILE *fileToRead = fopen(filename,"r");
    char lineSplitter[] = "\n";
    char indexSplitter[] = ":";
    char spacesplitter[] = " ";
    int index =0 , docIndex = 0,dicIndex = 0;
    char *line = NULL;
    ssize_t singleLine;
    size_t length = 0;
    char* numberStr;
    int number;  
    
    int currentDoc=0;

    if (fileToRead == NULL){
        return ;
    }else{
        while ((singleLine = getline(&line, &length, fileToRead)) != -1) 
        {   
            //get the numbers one by one and put them in the array
            numberStr = strtok(line, lineSplitter);
            if (numberStr == NULL){
                printf("The num: %s\n", line);
            }else {
                dicIndex = 0;
                numberStr = strtok(numberStr, indexSplitter);
                index = atoi(numberStr);    
                p->documents[currentDoc].ids[docIndex] = index;
                numberStr = strtok(NULL, indexSplitter);
                numberStr = strtok(numberStr, spacesplitter);

                while( numberStr != NULL ) {   
                    number = atoi(numberStr);
                    numberStr = strtok(NULL, spacesplitter);
                    p->documents[currentDoc].docs[docIndex][dicIndex] = number;
                    dicIndex = dicIndex +1;
                }

                docIndex = docIndex + 1;
                if (docIndex >= p->documents[currentDoc].noDocs){
                    currentDoc = currentDoc +1;
                    docIndex = 0;
                }
            }
        }
        free(line);
        fclose(fileToRead);
    }
}

//initDocs initializes  the docs struct
void initDocs(int dictSize,char* filename,struct Docs* tmp, int noProc, int* arrquery){
  
    int no = getNumberOfLines(filename);
    int i;

    int chunkSize = no/noProc;
    tmp->documents = (struct ProcValues*) malloc(noProc * sizeof (struct ProcValues) );
    tmp->noProc = noProc;
    
    for (i = 0; i < tmp->noProc; i++){
        tmp->documents[i].noDocs = chunkSize;
        tmp->documents[i].procId = i;
        tmp->documents[i].dictSize = dictSize;
        tmp->documents[i].currentHeap =tmp->documents[i].noDocs;
        if (i == 0){
            tmp->documents[i].noDocs = chunkSize + no%noProc;
        }
        tmp->documents[i].ids = (int*) malloc(tmp->documents[i].noDocs  * sizeof(int));
        //tmp->documents[i].vals = (int*) malloc(tmp->documents[i].noDocs  * sizeof(int));
        tmp->documents[i].query = arrquery;
        allocate2DArray(tmp->documents[i].noDocs,tmp->documents[i].dictSize,&(tmp->documents[i].docs));
    }
}

void deallocateDocs(struct Docs* tmp){
    int i ;
    for (i = 0; i < tmp->noProc; i++){
        DeallocateProcVal(&(tmp->documents[i]));
        //free(tmp->documents[i].ids);
    }
    free(tmp->documents);
}

//deallocates the struct info
void DeallocateProcVal(struct ProcValues* tmp){
    deallocate2D(tmp->docs,tmp->noDocs);
    free(tmp->ids);
    free(tmp->vals);
    //free(tmp->query);
}

// swaps two docs at the given indeces
void swapDoc(struct ProcValues* pv, int parent, int child){
    int* arr;
    int tmp;

    tmp = pv->vals[parent];
    pv->vals[parent] = pv->vals[child];
    pv->vals[child] = tmp;

    tmp = pv->ids[parent];
    pv->ids[parent] = pv->ids[child];
    pv->ids[child] = tmp;

    arr = pv->docs[parent];
    pv->docs[parent] = pv->docs[child];
    pv->docs[child] = arr;

}

//computes similarities arranged in a min heap
//NOTE: this doesnt change much from the original implementation, so you might like
// to swich to the normal one after all
void computeSimilaritiesSorted(struct ProcValues* pv,int allocate){
    if (allocate == 1)
        pv->vals = (int*) malloc(pv->noDocs * sizeof(int));

    int i, j,tmp;

    for (i = 0; i < pv->noDocs; i++){
        tmp =  computeSimilarity(pv->docs[i], pv->query, pv->dictSize);
        insertToHeap(pv,i,tmp);
    }
}

//insertToHeap inserts the docs to the heap
void insertToHeap(struct ProcValues* pv, int index, int elem){
    if (elem < 0){
        elem = INT_MAX;
    }
    pv->vals[index] = elem;
    heapify(pv,index);  
}

//heapifies up upon new insertions on the heap
void heapify(struct ProcValues* pv, int currentIndex){
    int parent,tmp;
    int* arr;
    if (currentIndex!=0){
        parent = (currentIndex-1)/2;
        if ( pv->vals[parent] > pv->vals[currentIndex]){
            swapDoc(pv,parent,currentIndex);
            heapify(pv,parent);
        }   
    }

}


//upon extracts re structures the heap to keep the heap properties 
void heapifydown(struct ProcValues* pv, int parent){
    int childleft,childright,tmp;
    int* arr;
    childleft = 2*parent +1;
    childright = 2*parent +2;
    if (childleft < pv->currentHeap){
        if (childright < pv->currentHeap){
            if (pv->vals[parent] > pv->vals[childleft] && pv->vals[childleft] < pv->vals[childright]){
                swapDoc(pv,parent,childleft);  
                heapifydown(pv,childleft);
            } else {
                swapDoc(pv,parent,childright);  
                heapifydown(pv,childright);
            }
        }else {
            if (pv->vals[parent] > pv->vals[childleft]){
                swapDoc(pv,parent,childleft);  
                heapifydown(pv,childleft);
            }
        }
    }

}

//extractMin moves the min value to the end of the array
void extractMin(struct ProcValues* pv){
    pv->currentHeap = pv->currentHeap -1;
    swapDoc(pv, 0, pv->currentHeap);
    heapifydown(pv,0);
}


//findleastkDocs returns an array of least similar docs. the lest k docs will be
// at the end of the array in the end. use with care.
int *findleastkDocs(struct ProcValues* pv,int** myids, int k){
    int* result = (int*) malloc(k * sizeof(int));
    (*myids) = (int*) malloc(k * sizeof(int));
    int i,index =0;
    if (pv->noDocs >= k){
        for (i = pv->noDocs -1 ; i >= pv->noDocs - k; i--){
            extractMin(pv);
            result[index] = pv->vals[i];
            (*myids)[index] = pv->ids[i];
            index = index + 1;
        }
    }else {
        for (i = pv->noDocs -1 ; i >= 0; i--){
            extractMin(pv);
            result[index] = pv->vals[i];
            (*myids)[index] = pv->ids[i];
            index = index + 1;
        }
        for (i = index; i < k; i++){
            result[index] = -27;
        }
    }
   
    return result;
}


//merges the result of 2k batches
void merge2KResults(int** leastk, int* second, int k, int** leaskids, int *ids){
    int i;
    int firstIndex = 0;
    int secondIndex = 0;
    int tmp;
    int *merged = (int*) malloc(k * sizeof(int));
    int *mergedids = (int*) malloc(k * sizeof(int));
    for (int i = 0; i < k; i++){
        if ((*leastk)[firstIndex] <= second[secondIndex] || second[secondIndex] < 0 ){
            merged[i] = (*leastk)[firstIndex];
            mergedids[i] = (*leaskids)[firstIndex];
            firstIndex = firstIndex +1;
        } else {
            if ( second[secondIndex] <= (*leastk)[firstIndex] || (*leastk)[firstIndex] < 0 ){
                merged[i] = second[secondIndex];
                mergedids[i] = ids[secondIndex];
                secondIndex = secondIndex +1;
            }
        }
    }
    free((*leastk));
    free((*leaskids));
    (*leastk) = merged;
    (*leaskids) = mergedids;
}

//prints integer array elements line by line
void resultPrint(int size, int* array){
    int i;
    printf("Least k = %d ids:\n",size);
    if (size > 0){
        for (i = 0; i<size; i++){
            if (array[i] == 0){
                continue;
            }
            printf("%d\n",array[i]);
        } 
     //   printf("\n");
    }

}