#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

#define N 2000

void printTime(struct timespec t0, struct timespec t1);
void printMatrix(int matrix[N][N]);

int a[N][N], b[N][N], c[N][N], cc[N][N], ccc[N][N];

int main(){
   struct timespec t_start, t_end;
   double elapsedTime;
   int i, j, k, temp, sum;
   
   for (i = 0; i < N; i++){
        for (j = 0; j < N; j++) {
            a[i][j] = rand();
            b[i][j] = rand();
            c[i][j] = 0;
            cc[i][j] = 0;
            ccc[i][j] = 0;
        }
	}

	// Sequantial: start time
    printf("Sequential elapsedTime: ");
	clock_gettime( CLOCK_REALTIME, &t_start);  
	for (i = 0; i < N; i++){
		for (j = 0; j < N; j++) {
			for (k = 0; k < N; k++){
				c[i][j] += a[i][k]*b[k][j];
			}
		}
	}
	// stop time
    clock_gettime(CLOCK_REALTIME, &t_end);
    printTime(t_start, t_end);


    // OpenMP 1: start time
    printf("OpenMP elapsedTime: ");
    clock_gettime( CLOCK_REALTIME, &t_start);

    //Transpose
    #pragma omp parallel for private(i, j, temp)
    for (i = 0; i < N; i++){
        for (j = i + 1; j < N; j++){
            temp = b[i][j];
            b[i][j] = b[j][i];
            b[j][i] = temp;
        }
    }

    #pragma omp parallel for private(i, j, k, sum)
    for(i = 0; i < N; i++){
        for(j = 0; j < N; j++){
            sum = 0;
            for(k = 0; k < N; k++) { 
                sum += a[i][k]*b[j][k];
            }
            cc[i][j] = sum;
        }
    }
 
  // stop time
   clock_gettime( CLOCK_REALTIME, &t_end);
   printTime(t_start, t_end);

    transpose(b);

   // OpenMP 2: start time
    printf("OpenMP2 elapsedTime: ");
    clock_gettime( CLOCK_REALTIME, &t_start);

    // Transpose
    #pragma omp parallel for private(i, j, temp)
    for (i = 0; i < N; i++){
        for (j = i + 1; j < N; j++){
            temp = b[i][j];
            b[i][j] = b[j][i];
            b[j][i] = temp;
        }
    }

    #pragma omp parallel for private(i, j, k)
    for(i = 0; i < N; i++){
        for(j = 0; j < N; j++){
            for(k = 0; k < N; k++) { 
                ccc[i][j] += a[i][k]*b[j][k];
            }
        }
    }
 
    // stop time
   clock_gettime( CLOCK_REALTIME, &t_end);
   printTime(t_start, t_end);
 
    for (i = 0; i < N; i++){
		for (j = 0; j < N; j++){
			if (cc[i][j] != c[i][j])
				break;
            if (ccc[i][j] != c[i][j])
				break;
		}
	}
   
	if (i == N && j == N) printf("Test pass!!!\n"); 
	else printf("Test failed!!!\n");
    
    return 0;
}

void printTime(struct timespec t0, struct timespec t1){
    double elapsedTime;

    elapsedTime = (t1.tv_sec - t0.tv_sec) * 1000.0;
    elapsedTime += (t1.tv_nsec - t0.tv_nsec) / 1000000.0;
    printf("%lf ms\n", elapsedTime);
}

void printMatrix(int matrix[N][N]){
    int i, j;

    for (i = 0; i < N; i++){
        for (j = 0; j < N; j++){
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}