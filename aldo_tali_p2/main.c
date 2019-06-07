#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"  
#include <mpi.h>
#include <time.h>

struct ProcValues pv;
int K_TAG  = 0;
int SIZE_TAG  = 1;
int IDS_TAG  = 2;
int DICTSIZE_TAG  = 3;
int DOCS_TAG  = 3;
int LEAST_TAG  = 5;
int IDS2_TAG  = 6;
double start;
double seqstart;
double end;
double sequential;

/*
    @author: Aldo Tali
    @creds: Senior Computer Engineering Student, Bilkent Uniersity
    @ID: 21500097
    @Course: Parallel Computing - CS426
    @Project01: Spring 2019 - Due 07/04/2019
    @Section: Main
    @Desc: Implements the kreduce function. Reads all the documents and the given query and by calling
            kreduce main will be able compute the k least simialr items in the whole docs.
*/ 

int main(int argc, char** argv){
    struct Docs docs;
    int* myids,*myvals,*arr;
    int** docus;
    int k, dictSize, noDocs,noOfProcessors,processId;
    int *leastk ;

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&noOfProcessors);
    MPI_Comm_rank(MPI_COMM_WORLD,&processId);
    start = MPI_Wtime();
    
    if (processId == 0){
        clock_t begin = clock();

        //check for correct input
        if (argc < 5) {
            printf("Please insert the arguments in the coorect form\n");
            printf("Acceptable format: \n");
            printf("mpirun –n X ./documentSearch dictionarySize kValue documents.txt query.txt\n\n");
            printf("[Debugg] Depending on your platform the - character may be encoded a little bit different. Plase write the command on your own dont straight copy it from this example. \n\n\n");
            exit(1);
        }

        //get the k and dictionary size as inputs
        k = atoi(argv[2]);
        dictSize = atoi(argv[1]);

        //reads the document and the query
        arr = readQuery(argv[4],dictSize);
        initDocs(dictSize,argv[3],&docs,noOfProcessors,arr);
        readDocuments(argv[3], dictSize,&docs,noOfProcessors);

        //sequential time finishes here      
        clock_t end = clock();
        sequential = (double)(end - begin)/ CLOCKS_PER_SEC;

        int i,j ;
        //send to each processor its normal share of the documents and 
        //correspondding doc ids with the query :)
        for (i = 1; i < noOfProcessors; i++){
            MPI_Send(&k, 1, MPI_INT, i, K_TAG,MPI_COMM_WORLD);
            MPI_Send(&(docs.documents[i].noDocs), 1, MPI_INT, i, SIZE_TAG,MPI_COMM_WORLD);
            MPI_Send(& (docs.documents[i].ids[0]), docs.documents[i].noDocs, MPI_INT, i, IDS_TAG,MPI_COMM_WORLD);
            MPI_Send(&(docs.documents[i].dictSize), 1, MPI_INT, i, DICTSIZE_TAG,MPI_COMM_WORLD);
            MPI_Send(&(docs.documents[i].query[0]), docs.documents[i].dictSize, MPI_INT, i, DICTSIZE_TAG,MPI_COMM_WORLD);

            for (j = 0; j < docs.documents[i].noDocs; j++){
                MPI_Send(&(docs.documents[i].docs[j][0]), docs.documents[i].dictSize, MPI_INT, i, DOCS_TAG,MPI_COMM_WORLD);
            }
        } 

        //make sure that the master also has a its own structure to have its share of computing
        //the similarities of size/noProc documents 
        pv.noDocs = docs.documents[0].noDocs;
        pv.ids = docs.documents[0].ids;
        pv.docs = docs.documents[0].docs;
        pv.query = docs.documents[0].query;
        pv.dictSize = docs.documents[0].dictSize;
        pv.currentHeap = pv.noDocs;
        pv.procId = processId;
        leastk = (int*) malloc(k * sizeof(int));
    }else {
        int ** tmp;
        int *query;

        //receive the neccessary data from the master node for the initial data 
        //distribution
        MPI_Recv(&k, 1, MPI_INT, 0, K_TAG, MPI_COMM_WORLD, NULL);
        MPI_Recv(&noDocs, 1, MPI_INT, 0, SIZE_TAG, MPI_COMM_WORLD, NULL);    
        myids = (int*) malloc(noDocs * sizeof(int));
        MPI_Recv(myids, noDocs , MPI_INT, 0, IDS_TAG, MPI_COMM_WORLD, NULL);
        MPI_Recv(&dictSize, 1, MPI_INT, 0, DICTSIZE_TAG, MPI_COMM_WORLD, NULL);
        query = (int*) malloc(dictSize * sizeof(int));
        MPI_Recv(&query[0], dictSize, MPI_INT, 0, DICTSIZE_TAG, MPI_COMM_WORLD, NULL);

        allocate2DArray(noDocs,dictSize,&tmp);
        int j;
        for (j = 0; j < noDocs; j++){
            MPI_Recv(&(tmp[j][0]), dictSize , MPI_INT, 0, DOCS_TAG, MPI_COMM_WORLD, NULL);
        }
        //initialize the ProcValues struct that will be used for the actual computation of the 
        //similarities and the local k similar docs.
        pv.noDocs = noDocs;
        pv.ids = myids;
        pv.docs = tmp;
        pv.query = query;
        pv.dictSize = dictSize;
        pv.currentHeap = pv.noDocs;
        pv.procId = processId;
    }  

    //each proccess calls kreduce with the according input
    //note: for each process pv holds different docs, ids and values
    // those being of that processor only
    kreduce(leastk,pv.ids,pv.vals,k,noOfProcessors,processId);
    if (processId == 0){
        leastk = pv.ids;
        end = MPI_Wtime();
        double parall = end-start;
        printf("Sequential Part: %f ms\n",sequential*1000);
        printf("Parallel Part: %f ms\n",(parall - sequential)*1000);
        printf("Total Time: %f ms\n",parall*1000);
        resultPrint(k,leastk); //I was not sure if least should be printed outside
                                // or inside the k reduce. If it was to be printed outside then
                                // the function signature should have been int** leastk
                                // Also the ints have quite some overflows with power function
                                // and changing them to floats means that we need to change the 
                                // given function's signature  
                                // Sad face :/
        
    }
    DeallocateProcVal(&pv);
    MPI_Finalize();
    return 0;
}


//implements the kreduce
void kreduce(int * leastk, int * myids, int * myvals, int k, int world_size, int my_rank){
    int  *myleast;
    int *ids;
    int *idReceiver = (int*) malloc(k * sizeof(int));
    double parall;

    leastk = (int*) malloc(k * sizeof(int));
    //compute partial result for this processor
    computeSimilaritiesSorted(&pv,1);       //use a min heap to compute the similarities
    myleast = findleastkDocs(&pv,&ids,k);   //extract the min from heap k times

    //1 proc means serialized version
    if (world_size == 1){
       /* end = MPI_Wtime();
        parall = end-start;
        printf("Sequential Part: %f ms\n",sequential*1000);
        printf("Parallel Part: %f ms\n",(parall - sequential)*1000);
        printf("Total Time: %f ms\n",parall*1000);
        if (pv.noDocs < k){
            resultPrint(pv.noDocs,ids);
        } else {
            resultPrint(k,ids);
        }*/
        pv.ids = ids;
    } else{
        if (my_rank == 0){
            
            int i;
            //accumulate partial results
            for (i = 1; i<world_size; i++){
                MPI_Recv(leastk, k , MPI_INT, i, LEAST_TAG, MPI_COMM_WORLD, NULL);      //get the least k from a proc
                MPI_Recv(idReceiver, k , MPI_INT, i, IDS2_TAG, MPI_COMM_WORLD, NULL);      //get ids ids   
                merge2KResults(&myleast, leastk,k, &ids,idReceiver);                    //merge with your own info
            }
           /* end = MPI_Wtime();
            parall = end-start;
            printf("Sequential Part: %f ms\n",sequential*1000);
            printf("Parallel Part: %f ms\n",(parall - sequential)*1000);
            printf("Total Time: %f ms\n",parall*1000);
            resultPrint(k,ids);*/
            pv.ids = ids;

        }else {
            //send partial results
            MPI_Send(myleast, k, MPI_INT, 0, LEAST_TAG,MPI_COMM_WORLD);
            MPI_Send(ids, k, MPI_INT, 0, IDS2_TAG,MPI_COMM_WORLD);
        }
    }
    
}
