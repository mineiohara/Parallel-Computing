#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <omp.h>

#define N 400000

struct position{
    int low;
    int high;
};

void swap(int *a, int *b);
int partition(int low, int high);
void quickSort(int low, int high);
void printArray();
void printTime(struct timespec t0, struct timespec t1);
void *runner(void *param);
void ompQuickSort(int low, int high);

int array[N];

int main() {
    int i;
    int temp[N];
    struct timespec t0, t1;

    for (i = 0; i < N; i ++) {
        array[i] = rand() % 10;
        temp[i] = array[i];
    }
  
    // Sequantial
    // printf("Unsorted Array\n ");
    // printArray();
    printf("Sequantial: ");
    
    clock_gettime(CLOCK_REALTIME, &t0);

    quickSort(0, N - 1);

    clock_gettime(CLOCK_REALTIME, &t1);
    printTime(t0, t1);


    // Pthread
    // printf("Sorted array in ascending order: \n");
    // printArray();

    for (i = 0; i < N; i++) array[i] = temp[i];

    // printf("Unsorted Array\n");
    // printArray();
    printf("Pthread: ");

    pthread_t tid;
    pthread_attr_t attr;
    
    clock_gettime(CLOCK_REALTIME, &t0);

    struct position *data = (struct position *)malloc(sizeof(struct position));
    data->high = N - 1;
    data->low = 0;
    pthread_attr_init(&attr);
    pthread_create(&tid, &attr, runner, data);
    pthread_join(tid, NULL);

    clock_gettime(CLOCK_REALTIME, &t1);
    printTime(t0, t1);
    
    // printf("Sorted array in ascending order: \n");
    // printArray();

    for (i = 0; i < N; i++) array[i] = temp[i];

    // OMP
    // printf("Unsorted Array\n");
    // printArray();
    printf("OpenMP: ");
    
    clock_gettime(CLOCK_REALTIME, &t0);

    ompQuickSort(0, N - 1);

    clock_gettime(CLOCK_REALTIME, &t1);
    printTime(t0, t1);
    
    // printf("Sorted array in ascending order: \n");
    // printArray();

    return 0;
}

void swap(int *a, int *b) {
    int t = *a;
    *a = *b;
    *b = t;
}

int partition(int low, int high) {
    int i, j, pivot;

    pivot = array[high];
    i = (low - 1);

    for (j = low; j < high; j++) {
        if (array[j] <= pivot) {
            i++;
            swap(&array[i], &array[j]);
        }
    }

    swap(&array[i+1], &array[high]);
    
    return (i + 1);
}

void quickSort(int low, int high) {
    if (low < high) {
        int i, pi;

        pi = partition(low, high);

        quickSort(low, pi - 1);
        quickSort(pi + 1, high);
    }
}

void printArray() {
    int i;
    for (i = 0; i < N; i++) printf("%d  ", array[i]);
    printf("\n");
}

void printTime(struct timespec t0, struct timespec t1){
    double elapsedTime;

    elapsedTime = (t1.tv_sec - t0.tv_sec) * 1000;
    elapsedTime += (t1.tv_nsec - t0.tv_nsec) / 1000000;
    printf("%lf ms\n", elapsedTime);
}

void *runner(void *param) {
    struct position *pos = param;

    if (pos->low < pos->high) {
        int i, pi;
        pthread_t tid[2];
        pthread_attr_t attr[2];

        pi = partition(pos->low, pos->high);

        struct position *data = (struct position *)malloc(sizeof(struct position));
        data->high = pi -1;
        data->low = pos->low;
        pthread_attr_init(&attr[0]);
        pthread_create(&tid[0], &attr[0], runner, data);

        struct position *data1 = (struct position *)malloc(sizeof(struct position));
        data1->high = pos->high;
        data1->low = pi + 1;
        pthread_attr_init(&attr[1]);
        pthread_create(&tid[1], &attr[1], runner, data1);

        for (i = 0; i < 2; i++) pthread_join(tid[i], NULL);
    }

    pthread_exit(0);
}

void ompQuickSort(int low, int high) {
    if (low < high) {
        int i, pi;

        pi = partition(low, high);

        #pragma omp parallel sections
        {
            #pragma omp section
            {
                ompQuickSort(low, pi - 1);
            }
            #pragma omp section
            {
                ompQuickSort(pi + 1, high);    
            }
        }
    }
}