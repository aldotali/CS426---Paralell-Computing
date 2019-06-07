gcc -Wall -pg sequtil.c sequtil.h lbp_seq.c -o lbp_seq
gcc -no-pie -pg omputil.c omputil.h lbp_omp.c -fopenmp -o lbp_omp
