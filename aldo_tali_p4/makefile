all: main
main: serial.c cuda_parallel.cu
	cc -o serial serial.c utils.h utils.c -std=c99 -lm
	/usr/local/cuda-9.1/bin/nvcc -o cuda_parallel cuda_parallel.cu