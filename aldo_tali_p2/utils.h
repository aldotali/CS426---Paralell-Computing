
#ifndef UTILS_H_   /* Include guard */
#define UTILS_H_

/*
    @author: Aldo Tali
    @creds: Senior Computer Engineering Student, Bilkent Uniersity
    @ID: 21500097
    @Course: Parallel Computing - CS426
    @Project01: Spring 2019 - Due 07/04/2019
    @Section: Utils header
    @Desc: Defines all the struct and function definitions needed for the implementation of kreduce.
*/ 


struct ProcValues{
  int noDocs;
  int *ids;
  int **docs;
  int *vals;
  int *query;
  int dictSize;
  int currentHeap;
  int procId;
};

struct Docs{
  int noProc;
  struct ProcValues* documents;
};

void prettyPrintArray(int size, int* array);
void prettyPrint2DArray(int dim1,int dim2, int** array);
void allocate2DArray(int dim1,int dim2,int*** array);
void deallocate2D(int** array, int size);
void readDocuments(char* filename, int dictSize,struct Docs* p, int noProc);
void initDocs(int dictSize,char* filename,struct Docs* tmp, int noProc,int* arrquery);
void DeallocateProcVal(struct ProcValues* tmp);
void deallocateDocs(struct Docs* tmp);
void swapDoc(struct ProcValues* pv, int parent, int child);
void swapDoc(struct ProcValues* pv, int parent, int child);
void computeSimilaritiesSorted(struct ProcValues* pv,int allocate);
void insertToHeap(struct ProcValues* pv, int index, int elem);
void heapify(struct ProcValues* pv, int currentIndex);
void heapifydown(struct ProcValues* pv, int parent);
void lookMostSimilarK(struct ProcValues* pv);
void extractMin(struct ProcValues* pv);
void kreduce(int * leastk, int * myids, int * myvals, int k, int world_size, int my_rank);
void merge2KResults(int** leastk, int* second, int k,int** leaskids, int *ids);
void resultPrint(int size, int* array);

int* readQuery(char* filename, int size);
int getNumberOfLines(char* fileName);
int computeSimilarity(int* doc, int* query, int dictSize);
int *findleastkDocs(struct ProcValues* pv,int** vals, int k);

#endif