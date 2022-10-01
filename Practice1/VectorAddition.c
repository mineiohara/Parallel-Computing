#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define N 1000000
#define Core 8

int A[N];
int B[N];
int C[N];
int goldenC[N];
struct v{
    int pos;
};

void *addition(void *param);
void *generate(void *param);
void printTime(struct timespec tStart, struct timespec tEnd);

int main(int argc, char* argv[])
{
    int i;
    pthread_t tid[Core];
    pthread_attr_t attr[Core];
    struct timespec tStart, tEnd;

    // for (i = 0; i < Core; i++)
    // {
    //     struct v *data = (struct v *)malloc(sizeof(struct v));
    //     data->pos = i;
        
    //     pthread_attr_init(&attr[i]);
    //     pthread_create(&tid[i], &attr[i], generate, data);
    // }

    // for (i = 0; i < Core; i++) pthread_join(tid[i], NULL);

    for (i = 0; i < N; i++)
    {
        A[i] = rand()%100;
        B[i] = rand()%100;
    }

    printf("Generated A, B vector.\n");


    // Sequential
    clock_gettime(CLOCK_REALTIME, &tStart);

    for (i = 0; i < N; i++) goldenC[i] = A[i] + B[i];

    clock_gettime(CLOCK_REALTIME, &tEnd);
    printf("Sequential");
    printTime(tStart, tEnd);


    // Parallel
    clock_gettime(CLOCK_REALTIME, &tStart);

    for (i = 0; i < Core; i++)
    {
        struct v *data = (struct v *)malloc(sizeof(struct v));
        data->pos = i;

        pthread_attr_init(&attr[i]);
        pthread_create(&tid[i], &attr[i], addition, data);
    }

    for (i = 0; i < Core; i++) pthread_join(tid[i], NULL);

    clock_gettime(CLOCK_REALTIME, &tEnd);
    printf("Parallel");
    printTime(tStart, tEnd);


    // Check
    for (i = 0; i < N; i++)
    {
        if (C[i] != goldenC[i])
        {
            printf("Failed!\n");
            return -1;
        }
    }

    printf("Success!\n");
    return 0;
}

void *addition(void *param)
{
    int i;
    struct v *data = param;

    for (i = (N / Core) * data->pos; i < (N / Core) * (data->pos + 1); i++)
    {
        C[i] = A[i] + B[i];
    }

    pthread_exit(0);
}

void *generate(void *param){
    int i;
    struct v *data = param;

    for (i = (N / Core) * data->pos; i < (N / Core) * (data->pos + 1); i++)
    {
        A[i] = rand()%100;
        B[i] = rand()%100;
    }

    pthread_exit(0);
}

void printTime(struct timespec tStart, struct timespec tEnd){
    double elapsedTime;

    elapsedTime = (tEnd.tv_sec - tStart.tv_sec) * 1000.0;
    elapsedTime += (tEnd.tv_nsec - tStart.tv_nsec) / 1000000.0;
    printf(" ElapsedTime: %lf ms\n", elapsedTime);
}
