#include "sequtil.h"

int **alloc_2d_matrix(int r, int c)
{
    int ** a;
    int i; 
    a = (int **)malloc(sizeof(int *) * r);
    if (a == NULL) {
        perror("memory allocation failure");
        exit(0);
    } 
    for (i = 0; i < r; ++i) {
        a[i] = (int *)malloc(sizeof(int) * c);
        if (a[i] == NULL) {
            perror("memory allocation failure");
            exit(EXIT_FAILURE);
        }
    }
    return a;
}
 
void dealloc_2d_matrix(int **a, int r, int c)
{
    	int i;
   	for (i = 0; i < r; ++i)
        	free(a[i]);
    	free(a);
}

int ** read_pgm_file(char * file_name, int h, int w)
{
	FILE * file;
    	int i,j, int_tmp;
    	int** data=alloc_2d_matrix(h,w);
    	
    if ((file = fopen(file_name, "r+")) == NULL)
	{
	    printf("ERROR: file open failed\n");
	    return(NULL);
	}
	
	for (i = 0; i < (h); i++)
	{
        for (j = 0; j < (w); j++)
	    {
		    fscanf(file,"%d", &int_tmp);
		    data[i][j] = int_tmp;	
	    }
    }    
    fclose(file);
    return data;
}



void create_histogram(int * hist, int ** img, int num_rows, int num_cols){
    int i,j;
    int number;
    int power;

    //go through each cell in the image
    for (i = 0; i < num_rows; i++){
        for (j = 0; j < num_cols; j++){
            number = 0;
            power = 7;           //highest most bit is 10 000 000 = 2^7  
            if (i-1 >= 0){       //check up
                if (j-1 >= 0)    //check left
                {
                    if (img[i][j] < img[i-1][j-1]){
                        number = number + (1 << power);
                    }
                }
                power = power -1;
                if (img[i][j] < img[i-1][j]){   //check middle top
                        number = number + (1 << power);
                }
                power = power -1;
                if (j+1 < num_cols) // check right top
                {
                    if (img[i][j] < img[i-1][j+1]){
                        number = number + (1 << power);
                    }
                }
                power = power -1;
            }else {
                power = power -3;       //all upper row is missing ignore the 3 most significant bits
            }
            
            if (j+1 < num_cols) //check right most middle
            {
                if (img[i][j] < img[i][j+1]){
                    number = number + (1 << power);
                }
            }
            power = power -1;
            if (i+1 < num_rows){    //check below
                if (j+1 < num_cols) // bottom right
                {
                    if (img[i][j] < img[i+1][j+1]){
                        number = number + (1 << power);
                    }
                }
                power = power -1;
                if (img[i][j] < img[i+1][j]){ //bottom middle
                        number = number + (1 << power);
                }
                power = power -1;
                if (j-1 >= 0) //bottom left
                {
                    if (img[i][j] < img[i+1][j-1]){
                        number = number + (1 << power);
                    }
                }
                power = power -1;
            } else {
                power = power -3;
            }
            if (j-1 >= 0 ) //tcheck left most middle
            {
                if (img[i][j] < img[i][j-1]){
                    number = number + (1 << power);
                }
            }
            hist[number] = hist[number] + 1;
        }
    }
}  

double safe_divide(double a, double b)
{
    if (b == 0)
    {
        return 0; 
    }
    return (a / (double)b);
}

double distance(int * a, int *b, int size){
    int i;
    double dist = 0.0;
    double dif;

    for (i = 0; i < size; i++){
        //printf("%f %d %d",dist, a[i], b[i]);
        dif = (double)(a[i]-b[i]);
        dif *= dif;
        dist = dist + safe_divide(dif, a[i]+b[i]);
    }
    
    return (dist/(double)2.0);
}

int find_closest(int ***training_set, int num_persons, int num_training, int size,int * test_image){
    double dist = 0;
    double min = -27;
    int personId;

    int i,j;//,k;

    for (i = 0; i < num_persons; i++){
        for (j = 0; j < num_training; j++){
            if (min < 0){
                min = (double) distance(training_set[i][j],test_image,size);
                personId = i;
            } else {
                dist = (double) distance(training_set[i][j],test_image,size);
                
                if (dist < min){
                    min = dist;
                    personId = i;
                }
            }
        }
    }
    return personId+1;
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
    int i;//,j;
    if (dim1 > 0 && dim2>0){
        for (i = 0; i<dim1; i++){
            prettyPrintArray(dim2,array[i]);
           // printf("\n");
        } 
    }
}

int ***getTrainingSet(int noPeople,int k, int histoSize, int rowcount, int columncount, char* baseFolder){
    int **img;
    char inum[5];
    char jnum[5];
    char *result = malloc(10);

    int ***trainingHistograms = (int ***)malloc(sizeof(int **) * noPeople );
    int i,j,p;

    for (i = 0; i < noPeople; ++i) {
        trainingHistograms[i] = (int **)malloc(sizeof(int*) * k);
        for (j = 0; j < k; ++j) {
            trainingHistograms[i][j] = (int *)malloc(sizeof(int) * histoSize);
            for (p = 0; p < histoSize; p++){
                trainingHistograms[i][j][p] = 0;
            }
            sprintf(inum, "%d", i+1);
            sprintf(jnum, "%d", j+1);
            strcpy(result, baseFolder);
            strcat(result, inum);
            strcat(result, ".");
            strcat(result, jnum);
            strcat(result, ".txt");
            //printf("%s,  %d,%d\n", result,i,j);
            img = read_pgm_file(result,rowcount,columncount); 
            create_histogram(trainingHistograms[i][j],img,rowcount,columncount);
            dealloc_2d_matrix(img,rowcount,columncount);
        }
       
    }

    return trainingHistograms;
}

int ***getTestSets(int noPeople,int k, int histoSize, int rowcount, int columncount, char* baseFolder){
    int **img;
    char inum[5];
    char jnum[5];
    char *result = malloc(10);

    int ***testHistograms = (int ***)malloc(sizeof(int **) * noPeople );
    int i,j,p;
    
    for (i = 0; i < noPeople; ++i) {
        testHistograms[i] = (int **)malloc(sizeof(int*) * 20-k);
        for (j = k; j < 20; ++j) {
            
            testHistograms[i][j-k] = (int *)malloc(sizeof(int) * histoSize);
            for (p = 0; p < histoSize; p++){
                testHistograms[i][j-k][p] = 0;
            }
            sprintf(inum, "%d", i+1);
            sprintf(jnum, "%d", j+1);
            strcpy(result, baseFolder);
            strcat(result, inum);
            strcat(result, ".");
            strcat(result, jnum);
            strcat(result, ".txt");
            //printf("%s,  %d,%d\n", result,i,j);
            img = read_pgm_file(result,rowcount,columncount); 
            create_histogram(testHistograms[i][j-k],img,rowcount,columncount);
            dealloc_2d_matrix(img,rowcount,columncount);
        }
       
    }

    return testHistograms;
}
