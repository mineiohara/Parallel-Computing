#include <stdio.h>
#include <stdlib.h>
#include <cuda_runtime.h>

#define N 100000000
#define Max 1024

__global__ void add1( int *a, int *b, int *c );
__global__ void add2( int *a, int *b, int *c );
__global__ void add3( int *a, int *b, int *c );

int main(){
    int *a, *b, *c;
    int *da, *db, *dc;
    int i;
    float elapsedTime;
    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);

    // Allocate the memory on the CPU
    a = (int *)malloc(Max * sizeof(int));
    b = (int *)malloc(Max * sizeof(int));
    c = (int *)malloc(Max * sizeof(int));

    // Allocate the memory one the GPU
    cudaMalloc((void **)&da, Max * sizeof(int));
    cudaMalloc((void **)&db, Max * sizeof(int));
    cudaMalloc((void **)&dc, Max * sizeof(int));

    srand(time(NULL));
    for (i = 0; i < Max; i++){
        a[i] = rand()%256;
        b[i] = rand()%256;
    }

    // Copy the arrays to GPU
    cudaMemcpy(da, a, Max * sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(db, b, Max * sizeof(int), cudaMemcpyHostToDevice);

    // One block with multiple threads
    cudaEventRecord(start, 0);
    add1<<<1, Max>>>(da, db, dc);
    cudaEventRecord(stop, 0);
    cudaEventSynchronize(stop);
    cudaEventElapsedTime(&elapsedTime, start, stop);

    cudaMemcpy(c, dc, Max * sizeof(int), cudaMemcpyDeviceToHost);
    for (i = 0; i < Max; i++) {
        if ((a[i] + b[i]) != c[i]) {
            printf( "Error:  %d + %d != %d\n", a[i], b[i], c[i] );
        }
    }
    printf("One block with multiple threads: %13f msec\n", elapsedTime);

    // Multiple blocks, each block has one thread
    cudaEventRecord(start, 0);
    add2<<<Max, 1>>>(da, db, dc);
    cudaEventRecord(stop, 0);
    cudaEventSynchronize(stop);
    cudaEventElapsedTime(&elapsedTime, start, stop);

    cudaMemcpy(c, dc, Max * sizeof(int), cudaMemcpyDeviceToHost);
    for (i = 0; i < Max; i++) {
        if ((a[i] + b[i]) != c[i]) {
            printf( "Error:  %d + %d != %d\n", a[i], b[i], c[i] );
        }
    }
    printf("Multiple blocks/one thread: %13f msec\n", elapsedTime);


    cudaFree(da);
    cudaFree(db);
    cudaFree(dc);
    free(a);
    free(b);
    free(c);

    a = (int *)malloc(N * sizeof(int));
    b = (int *)malloc(N * sizeof(int));
    c = (int *)malloc(N * sizeof(int));

    cudaMalloc((void **)&da, N * sizeof(int));
    cudaMalloc((void **)&db, N * sizeof(int));
    cudaMalloc((void **)&dc, N * sizeof(int));

    srand(time(NULL));
    for (i = 0; i < N; i++){
        a[i] = rand()%256;
        b[i] = rand()%256;
    }

    // Copy the arrays to GPU
    cudaMemcpy(da, a, N * sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(db, b, N * sizeof(int), cudaMemcpyHostToDevice);


    // 1024 threads with N+1024-1/1024 blocks
    cudaEventRecord(start, 0);
    add3<<<(N+Max-1)/Max, Max>>>(da, db, dc);
    cudaEventRecord(stop, 0);
    cudaEventSynchronize(stop);
    cudaEventElapsedTime(&elapsedTime, start, stop);

    cudaMemcpy(c, dc, N * sizeof(int), cudaMemcpyDeviceToHost);
    for (i = 0; i < N; i++) {
        if ((a[i] + b[i]) != c[i]) {
            printf( "Error:  %d + %d != %d\n", a[i], b[i], c[i] );
        }
    }
    printf("1024 threads per block: %13f msec\n", elapsedTime);


    // 544 threads with N+544-1/544 blocks
    cudaEventRecord(start, 0);
    add3<<<(N+544-1)/544, 544>>>(da, db, dc);
    cudaEventRecord(stop, 0);
    cudaEventSynchronize(stop);
    cudaEventElapsedTime(&elapsedTime, start, stop);

    cudaMemcpy(c, dc, N * sizeof(int), cudaMemcpyDeviceToHost);
    for (i = 0; i < N; i++) {
        if ((a[i] + b[i]) != c[i]) {
            printf( "Error:  %d + %d != %d\n", a[i], b[i], c[i] );
        }
    }
    printf("544 threads per block: %13f msec\n", elapsedTime);


    // 32 threads with N+32-1/32 blocks
    cudaEventRecord(start, 0);
    add3<<<(N+32-1)/32, 32>>>(da, db, dc);
    cudaEventRecord(stop, 0);
    cudaEventSynchronize(stop);
    cudaEventElapsedTime(&elapsedTime, start, stop);

    cudaMemcpy(c, dc, N * sizeof(int), cudaMemcpyDeviceToHost);
    for (i = 0; i < N; i++) {
        if ((a[i] + b[i]) != c[i]) {
            printf( "Error:  %d + %d != %d\n", a[i], b[i], c[i] );
        }
    }
    printf("32 threads per block: %13f msec\n", elapsedTime);
    

    // Free the memory allocated on the GPU
    cudaFree(da);
    cudaFree(db);
    cudaFree(dc);
    free(a);
    free(b);
    free(c);
    cudaEventDestroy(start);
    cudaEventDestroy(stop);

    return 0;
}

__global__ void add1(int *a, int *b, int *c){
    int tid = threadIdx.x;
    c[tid] = a[tid] + b[tid];
}

__global__ void add2(int *a, int *b, int *c){
    int tid = blockIdx.x;
    c[tid] = a[tid] + b[tid];
}

__global__ void add3(int *a, int *b, int *c){
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    c[tid] = a[tid] + b[tid];
}