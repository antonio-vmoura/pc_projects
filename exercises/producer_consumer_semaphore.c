/*
O problema do produtor e consumidor (também conhecido como o problema do buffer limitado), 
consiste em um conjunto de processos que compartilham um mesmo buffer. 
Os processos chamados produtores colocam informacoes no buffer.
Os processos chamados consumidores retiram informacoes deste buffer. 
Quando o buffer estiver cheio, os produtores devem aguardar por posições disponíveis
para inserir os dados produzidos. Da mesma forma, quando o buffer estiver vazio os 
consumidores devem aguardar por dados para serem consumidos.
*/

#include "stdio.h"
#include "stdlib.h"
#include "pthread.h"
#include "unistd.h"
#include "semaphore.h"

#define PR 1 //number of producers
#define CN 1 //number of consumers
#define N 5 //buffer size

sem_t mutex;
sem_t empty;
sem_t full;

int counter = 0;

void * producer(void * meuid);
void * consumer(void * meuid);
void * put_item();
void * get_item();

void main(argc, argv)
int argc;
char *argv[];
{
	int erro;
	int i, n, m;
	int *id;

  	sem_init(&mutex, 0, 1);
	sem_init(&empty, 0, N);
	sem_init(&full, 0, 0);

  	pthread_t tid[PR];
   
	for (i = 0; i < PR; i++) {
		id = (int *) malloc(sizeof(int));
		*id = i;
		erro = pthread_create(&tid[i], NULL, producer, (void *) (id));

		if(erro) {
			printf("erro na criacao do thread %d\n", i);
			exit(1);
		}
	}
  
  	pthread_t tCid[CN];

	for (i = 0; i < CN; i++) {
		id = (int *) malloc(sizeof(int));
		*id = i;
		erro = pthread_create(&tCid[i], NULL, consumer, (void *) (id));

		if(erro) {
			printf("erro na criacao do thread %d\n", i);
			exit(1);
		}
	}
 
  pthread_join(tid[0],NULL);
}

void * producer(void* pi) {
	while(1) {
		sem_wait(&empty);
		sem_wait(&mutex);
		printf("\nProduzindo... \n");
		put_item();
		sem_post(&mutex);
		sem_post(&full);
	}
	pthread_exit(0);
}

void * consumer(void* pi) {
	while(1) {
		sem_wait(&full);
		sem_wait(&mutex);
		printf("\nConsumindo... \n");
		get_item();	    
		sem_post(&mutex);
		sem_post(&empty);
	}
	pthread_exit(0);
}

void * put_item() {
    int item = 1;

    printf("Produtor criou %d item(s) \n", item);
    counter += item;
    if (counter >= N){
      counter = N;
      printf("O buffer está cheio \n");
    } else {
      printf("Tamanho do buffer: %d itens \n", counter);
    }

  	sleep(rand() % 5);   
}

void * get_item(){
    counter -= 1;
    printf("Consumidor consumiu 1 item \n");
    printf("Tamanho do buffer: %d itens \n", counter);
    sleep(rand() % 5);   
}
