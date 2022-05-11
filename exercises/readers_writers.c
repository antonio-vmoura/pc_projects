/*
Este problema é uma abstração do acesso à base de dados, onde não existe o perigo em termos diversos processos lendo concorrentemente (leitores), 
mas escrevendo ou mudando os dados (escritores) deve ser feito sob exclusão mútua para garantir consistência.
Leitores e escritores é uma família de modelos de controle de concorrência em que leitores (entes que não alteram conteúdo)
pode acessar concorrentemente os recursos (por exemplo, um banco de dados) e escritores (entes que alteram conteúdo) requerem acesso exclusivo.
*/

#include "stdio.h"
#include "unistd.h"
#include "stdlib.h"
#include "pthread.h"

#define TRUE 1

#define NE 3 //number of writers
#define NL 10 //number of readers

pthread_mutex_t lock_bd = PTHREAD_MUTEX_INITIALIZER; //lock the database
pthread_mutex_t lock_nr = PTHREAD_MUTEX_INITIALIZER; //lock the number of readers
pthread_mutex_t lock_turn = PTHREAD_MUTEX_INITIALIZER; //locks the reader's or writer's turn

int num_readers = 0;

void * reader(void *arg);
void * writer(void *arg);
void read_data_base();
void use_data_read();
void think_up_data();
void write_data_base();

int main() {
	pthread_t r[NL], w[NE];
	int *id;

	//creating readers
	for (int i = 0; i < NL ; i++) {
		id = (int *) malloc(sizeof(int));
		*id = i;
		pthread_create(&r[i], NULL, reader, (void *) (id));
	}

	//creating writers
	for (int i = 0; i< NE; i++) {
	   	id = (int *) malloc(sizeof(int));
        *id = i;
		pthread_create(&w[i], NULL, writer, (void *) (id));
	}

	pthread_join(r[0],NULL);

	return 0;
}

void * reader(void *arg) {
	int i = *((int *) arg);

	while(TRUE) {
		pthread_mutex_lock(&lock_turn);
			pthread_mutex_lock(&lock_nr);
				num_readers++;
				if(num_readers == 1){
					pthread_mutex_lock(&lock_bd);
				}
			pthread_mutex_unlock(&lock_nr);
		pthread_mutex_unlock(&lock_turn);

		read_data_base(i);

		pthread_mutex_lock(&lock_nr);
			num_readers--;
			if(num_readers == 0){  
				pthread_mutex_unlock(&lock_bd);
			}
		pthread_mutex_unlock(&lock_nr);
		use_data_read(i);
	}

    pthread_exit(0);
}

void * writer(void *arg) {
	int i = *((int *) arg);

	while(TRUE) {
		think_up_data(i);

		pthread_mutex_lock(&lock_turn);
			pthread_mutex_lock(&lock_bd);
				write_data_base(i);
			pthread_mutex_unlock(&lock_bd);
		pthread_mutex_unlock(&lock_turn);
	}

    pthread_exit(0);
}

void read_data_base(int i) {
	printf("Leitor %d está lendo os dados! Número de leitores: %d\n", i, num_readers);
	sleep(rand() % 5);
}

void use_data_read(int i) {
	printf("Leitor %d está usando os dados lidos! Número de leitores: %d\n", i, num_readers);
	sleep(rand() % 5);
}

void think_up_data(int i) {
	printf("Escritor %d está pensando no que escrever!\n", i);
	sleep(rand() % 5);
}

void write_data_base(int i) {
	printf("Escritor %d está escrevendo os dados! Número de leitores: %d\n", i, num_readers);
	sleep( rand() % 5 + 15);
}
