/*
Um pombo correio leva cartas entre os sites A e B, mas só quando o número de cartas acumuladas em sua mochila chegar a 20. Inicialmente, 
o pombo fica em A, esperando que existam 20 cartas para carregar, e dormindo enquanto não houver. 
Quando as cartas chegam a 20, o pombo deve levar todas as 20 cartas de A para B, e em seguida voltar para A, repetindo este ciclo. 
As cartas são escritas por uma quantidade qualquer de usuários. Cada usuário, quando tem uma carta pronta, 
coloca sua carta na mochila do pombo e volta a escrever uma nova carta. Caso o pombo tenha partido, ele deve esperar o seu retorno para colar a carta na mochila.
*/

#include "stdlib.h"
#include "stdio.h"
#include "pthread.h"
#include "semaphore.h"
#include "unistd.h"

#define PIGEON 1 //number of pigeons
#define USERS 10 //number of users
#define BAG 20 //number of letters in bag

int letters = 0;

sem_t mutex;
sem_t empty;
sem_t full;

void * user(void *arg);
void * pigeon(void *arg);

int main(int argc, char **argv) {
    sem_init(&mutex, 0, 1);
	sem_init(&empty, 0, BAG);
	sem_init(&full, 0, 0);

    pthread_t pigeon_t[PIGEON], user_t[USERS];
    int *id;

    for (int i = 0; i < PIGEON ; i++) {
		id = (int *) malloc(sizeof(int));
		*id = i;
		pthread_create(&pigeon_t[i], NULL, pigeon, (void *) (id));
	}

    for(int i = 0; i < USERS; i++){
        id = (int *) malloc(sizeof(int));
        *id = i;
	    pthread_create(&(user_t[i]), NULL, user,  (void *) (id));
    }

    pthread_join(pigeon_t[0],NULL);
	pthread_join(user_t[0],NULL);
	
	return 0;
}

void * pigeon(void *arg) {
    int i = *((int *) arg);
  
    while(1) {
        sem_wait(&full);
	    printf("\n Pombo %d viajando para B!", i);
        sleep( rand() % 5);

	    sem_wait(&mutex);

	    letters = 0;

	    for (int j = 0; j < BAG; j++){
	        sem_post(&empty);
	    }

	    printf("\n Pombo %d voltando para A", i);
        sleep( rand() % 5);

	    sem_post(&mutex);
    }
    pthread_exit(0);
}

void * user(void *arg) {
    int i = *((int *) arg);
    
    while(1) {
	    printf("\n Usuario %d escrevendo carta!", i);
        sleep(rand() % 5);

	    sem_wait(&empty);
	    sem_wait(&mutex);

	    letters++;

	    printf("\n Usuario %d colocou carta! Capacidade da mochila: %d", i, BAG-letters);
        sleep(rand() % 5);

	    if (letters == BAG){
	      sem_post(&full);
	    }

	    sem_post(&mutex);
    }
    pthread_exit(0);
}
