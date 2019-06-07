#include "omputil.h"
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
    double serialRunningTime = 0.0;
    double parallelTime = 0.0;
    

    int *** trainingHistograms = getTrainingSet(noPeople,k,histoSize, rowcount,columncount,"images/", &serialRunningTime,&parallelTime);
    int *** testSets = getTestSets(noPeople,k, histoSize, rowcount, columncount, "images/", &serialRunningTime, &parallelTime);

    char inum[5];
    char jnum[5];
    char *result = malloc(10);
    
    int i, j , closest_id;
    int accurate = 0;
    start = clock();
    for (i = 0; i < noPeople; i++){
        for (j = k; j < 20; j++){

            closest_id = find_closest(trainingHistograms, noPeople, k ,histoSize,testSets[i][j-k]);
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
    parallelTime = parallelTime;
    printf("Accuracy: %d correct answers for %d tests\n",accurate, noPeople*(20-k));
    printf("Parallel time: %.2f ms\n",parallelTime*1000);
    printf("Sequential time: %.2f ms\n",serialRunningTime);

    return 0;
    
}