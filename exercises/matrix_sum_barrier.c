#include "stdio.h"
#include "stdlib.h"
#include "pthread.h"

#define MAXSIZE 10000  /* maximum matrix size */

pthread_barrier_t barrier;
pthread_mutex_t lock_turn = PTHREAD_MUTEX_INITIALIZER;
int size, final_result = 0;

int matrixR[MAXSIZE][MAXSIZE];

void *Worker(void *);

int main(int argc, char *argv[]) {
	size = atoi(argv[1]);
	
	if(size > MAXSIZE){
		printf("Tamanho muito grande!\n");
		return 0;	
	}

	pthread_t workerid[size];
	
	pthread_barrier_init(&barrier, NULL, size);

	for (int i = 0; i < size; i++)
		for (int j = 0; j < size; j++)
			matrixR[i][j] = 1;

	int * id;

	for (int i = 0; i < size; i++) {
		id = (int *) malloc(sizeof(int));
		*id = i;
		pthread_create(&workerid[i], NULL, Worker, (void *) (id));
	}

	for (int i = 0; i < size; i++) {
		if(pthread_join(workerid[i], NULL)) {
			printf("\n ERROR joining thread");
			exit(1);
		}
	}

	printf("\nBye!\n");
}


void *Worker(void *arg) {
	int myid = *(int *)(arg);
	int self = pthread_self();
	int result = 0;
	
	printf("worker %d (pthread id %d) has started\n", myid, self);

	for(int h = 0; h < size; h++){
		result += matrixR[myid][h];
	}

	pthread_barrier_wait(&barrier);

	pthread_mutex_lock(&lock_turn);
		final_result += result;
	pthread_mutex_unlock(&lock_turn);

	pthread_barrier_wait(&barrier);

	if (myid == 0) {
		printf("\nResultado final: %d\n", final_result);
	}
}
