/*
Suponha que há macacos em dois morros A e B, ligados por uma ponte de corda. 
De tempos em tempos os macacos decidem passar para o outro morro à procura de comida.
Esta passagem é feita através da ponte de corda. Mais de um macaco pode atravessar a ponte ao mesmo tempo, 
mas isso só é possível se eles estiverem indo na mesma direção. Implemente um programa que faça o controle da passagem de macacos pela ponte usando locks.
*/

#include "pthread.h"
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"

pthread_mutex_t lock_ma = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_mb = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_string = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_turn = PTHREAD_MUTEX_INITIALIZER;

#define MA 10 // monkeys that walk from A to B
#define MB 10 // monkeys that walk from B to A

int monkey_a = 0;
int monkey_b = 0;

void *monkeyAB(void *a) {
	int i = *((int *)a);

	while (1) {
		// procedures for accessing the rope
		pthread_mutex_lock(&lock_turn);
			pthread_mutex_lock(&lock_ma);
				monkey_a++;
				if (monkey_a == 1) {
					pthread_mutex_lock(&lock_string);
				}
			pthread_mutex_unlock(&lock_ma);
		pthread_mutex_unlock(&lock_turn);

		printf("Macaco %d passado de A para B \n", i);
		sleep(5);

		// procedures for when getting off the rope
		pthread_mutex_lock(&lock_ma);
		monkey_a--;
		if (monkey_a == 0) {
			pthread_mutex_unlock(&lock_string);
		}

		pthread_mutex_unlock(&lock_ma);
	}

	pthread_exit(0);
}

void *monkeyBA(void *a) {
	int i = *((int *)a);

	while (1) {
		// procedures for accessing the rope
		pthread_mutex_lock(&lock_turn);
			pthread_mutex_lock(&lock_mb);
				monkey_b++;
				if (monkey_b == 1) {
					pthread_mutex_lock(&lock_string);
				}
			pthread_mutex_unlock(&lock_mb);
		pthread_mutex_unlock(&lock_turn);

		printf("Macaco %d passado de B para A \n", i);
		sleep(1);

		// procedures for when getting off the rope
		pthread_mutex_lock(&lock_mb);
		monkey_b--;
		if (monkey_b == 0) {
			pthread_mutex_unlock(&lock_string);
		}
		pthread_mutex_unlock(&lock_mb);
	}

	pthread_exit(0);
}

int main(int argc, char *argv[]) {
	pthread_t monkeys[MA + MB];
	int *id;

	for (int i = 0; i < MA + MB; i++) {
		id = (int *)malloc(sizeof(int));
		*id = i;
		if (i % 2 == 0) {
			if (pthread_create(&monkeys[i], NULL, &monkeyAB, (void *)id)) {
				printf("Não pode criar a thread %d\n", i);
				return -1;
			}
		} else {
			if (pthread_create(&monkeys[i], NULL, &monkeyBA, (void *)id)) {
				printf("Não pode criar a thread %d\n", i);
				return -1;
			}
		}
	}

	pthread_join(monkeys[0], NULL);
	return 0;
}
