#include "sequtil.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <time.h>

int main(int argc, char** argv)
{   

    
    int rowcount = 200;
    int columncount = 180;
    int noPeople = 18;
    int histoSize = (1<<8) ;

    int k = atoi(argv[1]);

    clock_t start, end;
    double serialRunningTime;
     
    start = clock();
     
    int *** trainingHistograms = getTrainingSet(noPeople,k,histoSize, rowcount,columncount,"images/");
    int *** testSets = getTestSets(noPeople,k, histoSize, rowcount, columncount, "images/");

    char inum[5];
    char jnum[5];
    char *result = malloc(10);

    int i, j , closest_id;
    int accurate = 0;
    for (i = 0; i < noPeople; i++){
        for (j = k+1; j <= 20; j++){
            closest_id = find_closest(trainingHistograms, noPeople, k ,histoSize,testSets[i][j-k-1]);
            if (closest_id == i+1){
                accurate = accurate +1;
            } 

            sprintf(inum, "%d", i+1);
            sprintf(jnum, "%d", j);
            strcpy(result, "");
            strcat(result, inum);
            strcat(result, ".");
            strcat(result, jnum);
            strcat(result, ".txt");
            printf("%s %d %d\n", result,closest_id, i+1 );
        }
    }
    end = clock();
    serialRunningTime = ((double) (end - start))*1000/CLOCKS_PER_SEC;
    printf("Accuracy: %d correct answers for %d tests\n",accurate, noPeople*(20-k));
    printf("Parallel time: %.2f ms\n",0.00);
    printf("Sequential time: %.2f ms\n",serialRunningTime);

    //printf("We had [%d] erros from [%d] Sets\n",noPeople*(20-k)-accurate, noPeople*(20-k));
    
    return 0;
    
}