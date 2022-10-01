#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 100
#define Core 4

int A [N][N];
int B [N][N];
int C [N][N] = {0};
int goldenC [N][N] = {0};
struct v
{
   int i;
};

void *runner(void *param);
double calcTime(struct timespec, struct timespec);
void transpose(int [N][N]);

int main(int argc, char *argv[]) {
    int i, j, k;
    pthread_t tid[Core];       //Thread ID
    pthread_attr_t attr[Core]; //Set of thread attributes
    struct timespec t_start, t_end;
    double ptime, stime;
    
    for(i = 0; i < N; i++) {
        for(j = 0; j < N; j++) {
            A[i][j] = rand()%100;
            B[i][j] = rand()%100;
        }
    }
    
    // Pthread parallel
    clock_gettime( CLOCK_REALTIME, &t_start);

    transpose(B);
    
    for(i = 0; i < Core; i++) {
        struct v *data = (struct v *) malloc(sizeof(struct v));
        data->i = i;

        pthread_attr_init(&attr[i]);
        pthread_create(&tid[i],&attr[i],runner,data);
    }

    for(i = 0; i < Core; i++) {
            pthread_join(tid[i], NULL);
    }

    clock_gettime( CLOCK_REALTIME, &t_end);
    printf("Parallel elapsedTime: %lf ms\n", ptime = calcTime(t_start, t_end));


    transpose(B);


    // Sequantial
    clock_gettime( CLOCK_REALTIME, &t_start);

    for (i = 0; i < N; i++) {
        for(j = 0; j < N; j++) {
            for(k=0; k<N; k++){
                goldenC[i][j]+=A[i][k] * B[k][j];
            }
        }
    }

    clock_gettime( CLOCK_REALTIME, &t_end);
    printf("Sequential elapsedTime: %lf ms\n", stime = calcTime(t_start, t_end));


    // Check the performance and accuracy
    printf("Speed up by %.3lf times\n", stime/ptime);

    int pass = 1;
    for(i = 0; i < N; i++) {
        for(j = 0; j < N; j++) {
            if(goldenC[i][j]!=C[i][j]){
                pass = 0;
            }
        }
    }
    if(pass==1)
        printf("Test pass!\n");
    
    return 0;
}

void *runner(void *param)
{
    struct v *data = param;
    int i, j, k;
    
    for(i = data->i * N / Core; i < (data->i + 1) * N / Core; i++)
    {
        for (j = 0; j < N; j++)
        {
            for (k = 0; k < N; k++)
            {
                C[i][j] += A[i][k] * B[j][k];
            }
        }
    }

    pthread_exit(0);
}

double calcTime(struct timespec tStart, struct timespec tEnd){
    double elapsedTime;

    elapsedTime = (tEnd.tv_sec - tStart.tv_sec) * 1000.0;
    elapsedTime += (tEnd.tv_nsec - tStart.tv_nsec) / 1000000.0;

    return elapsedTime;
}

void transpose(int matrix[N][N]){
    int i, j, temp;

    for (i = 0; i < N; i++)
    {
        for (j = i + 1; j < N; j++)
        {
            temp = matrix[i][j];
            matrix[i][j] = matrix[j][i];
            matrix[j][i] = temp;
        }
    }
}
