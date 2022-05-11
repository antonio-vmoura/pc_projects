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

pthread_mutex_t lock_turn = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t user_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t pigeon_cond = PTHREAD_COND_INITIALIZER;

void * user(void *arg);
void * pigeon(void *arg);

int main(int argc, char **argv) {
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
        pthread_mutex_lock(&lock_turn);
            pthread_cond_wait(&pigeon_cond, &lock_turn);

            printf("\n Pombo %d viajando para B!", i);
            sleep(rand() % 5);

            letters = 0;

            pthread_cond_broadcast(&user_cond); //wake up users

            printf("\n Pombo %d voltando para A", i);
            sleep(rand() % 5);

	    pthread_mutex_unlock(&lock_turn);
    }
    pthread_exit(0);
}

void * user(void *arg) {
    int i = *((int *) arg);
    
    while(1) {
        
	    printf("\n Usuario %d escrevendo carta!", i);
        sleep(rand() % 5);

	    while(letters == BAG) {
		    pthread_cond_wait(&user_cond, &lock_turn);
	    }

	    letters++; //Put your letter in bag
	    printf("\n Usuario %d colocou carta! Capacidade da mochila: %d", i, BAG-letters);
        sleep(rand() % 5);

	    if (letters == BAG) { //If the bag is full, wake up the pigeon
	      pthread_cond_signal(&pigeon_cond);
	    }

	    pthread_mutex_unlock(&lock_turn);
    }
    pthread_exit(0);
}
