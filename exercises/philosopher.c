#include "stdlib.h"
#include "stdio.h"
#include "pthread.h"
#include "time.h"
#include "unistd.h"
#include "semaphore.h"

#define N 5
#define LEFT (n + N - 1) % N
#define RIGHT (n + 1) % N

#define THINKING 0
#define HUNGRY 1
#define EATING 2

sem_t s[N];
sem_t mutex;
int status[N];

void *philosophers(void *arg);

void get_cutlery(int n);
void return_cutlery(int n);
void check_availability(int n);

int main () {
	int *id;
	pthread_t r[N]; 

	for (int i=0; i < N; i++) {
		sem_init(&s[i], 0, 0);
	}

	sem_init(&mutex, 0, 1);

	//criacao das threads dos filosofos
    for (int i = 0; i < N; i++) {
		id = (int *) malloc(sizeof(int));
		*id = i;
		pthread_create(&r[i], NULL, philosophers, (void*) (id));
	}

	pthread_join(r[0], NULL);

	return 0;
}

void *philosophers(void *arg) {
	int n = *((int *) arg);

	while(1) {
		printf("Filosofo %d pensando\n", n);
		sleep(3);

		get_cutlery(n);
		printf("\tFilosofo %d comendo ...\n", n);
		sleep(3);

        printf("\tFilosofo %d acabou de comer ...\n", n);
		return_cutlery(n);
	} 
}

void get_cutlery(int n) {
	sem_wait(&mutex);
	status[n] = HUNGRY;
	check_availability(n);
	sem_post(&mutex);
	sem_wait(&s[n]);
}

void return_cutlery (int n) {
	sem_wait(&mutex);
    status[n] = THINKING;
    check_availability(RIGHT);
    check_availability(LEFT);
    sem_post(&mutex);
}

void check_availability(int n){
	if (status[n] == HUNGRY && status[RIGHT] != EATING && status[LEFT] != EATING) {
		status[n] = EATING;
		sem_post(&s[n]); 
	}
}