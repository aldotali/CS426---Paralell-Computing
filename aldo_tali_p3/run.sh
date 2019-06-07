#! /bin/bash 

kVals=( 1 2 5 7 10 )
threadVals=( 1 2 4 6 8 16 )

for k in "${kVals[@]}"
do
     for thread in "${threadVals[@]}"
     do
            ./lbp_seq $k >> aldo_tali.output  && export OMP_NUM_THREADS=$thread && ./lbp_omp $k >> aldo_tali.output
  
     done
done
