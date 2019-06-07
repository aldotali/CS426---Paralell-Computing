#include <stdlib.h>
#include <string.h>
#include <stdio.h>


/*
*	Allocates an rxc integer matrix
*/
int ** alloc_2d_matrix(int r, int c);

/*
*	Deallocates an rxc integer matrix
*/
void dealloc_2d_matrix(int ** a, int r, int c);

/*@params: 
*		file_name: name of the file
*		h: number of rows in the file
*		w: number of columns in the file
*		reads a matrix file
*		note that this function can not read pgm files, only use with given dataset
**/ 
int ** read_pgm_file(char * file_name, int h, int w);


/*
 * Creates a histogram for image given by int **img and returns histogram
 * as int * hist
*/
void create_histogram(int * hist, int ** img, int num_rows, int num_cols);
double distance(int * a, int *b, int size);
int find_closest(int ***training_set, int num_persons, int num_training, int size,int * test_image);
int ***getTrainingSet(int noPeople,int k, int histoSize, int rowcount, int columncount,char* baseFolder);
int ***getTestSets(int noPeople,int k, int histoSize, int rowcount, int columncount, char* baseFolder);

void prettyPrint2DArray(int dim1,int dim2, int** array);
void prettyPrintArray(int size, int* array);