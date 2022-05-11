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

#define PR 1 //number of producers
#define CN 2 //number of consumers
#define N 10 //buffer size

pthread_cond_t cond_pr = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_cn = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock_turn = PTHREAD_MUTEX_INITIALIZER;

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
	int *id;

	pthread_t tid[PR], tCid[CN];
	
	//creating producers
	for (int i = 0; i < PR; i++){
		id = (int *) malloc(sizeof(int));
		*id = i;
		erro = pthread_create(&tid[i], NULL, producer, (void *) (id));

		if(erro){
			printf("erro na criacao do thread %d\n", i);
			exit(1);
		}
	}

	//creating consumers
	for (int i = 0; i < CN; i++){
		id = (int *) malloc(sizeof(int));
		*id = i;
		erro = pthread_create(&tCid[i], NULL, consumer, (void *) (id));

		if(erro){
			printf("erro na criacao do thread %d\n", i);
			exit(1);
		}
	}
	
	pthread_join(tid[0],NULL);
}

void * producer (void* pi){
	while(1){
		pthread_mutex_lock(&lock_turn);
		
			while(counter == N){
				pthread_cond_wait(&cond_pr, &lock_turn);
			}

			printf("\nProduzindo... \n");
			put_item();
		pthread_mutex_unlock(&lock_turn);

		if(counter != 0){
			printf("Produtor acordando consumidor \n");
			pthread_cond_signal(&cond_cn);
		}

	}

	pthread_exit(0);
}

void * consumer (void* pi){
  while(1){
    pthread_mutex_lock(&lock_turn);
      while(counter == 0){
        pthread_cond_wait(&cond_cn, &lock_turn);
      }
      printf("\nConsumindo... \n");
      get_item();
    pthread_mutex_unlock(&lock_turn);

    if(counter != N){
      printf("Consumidor acordando produtor \n");
      pthread_cond_signal(&cond_pr);
    }
  }

  pthread_exit(0);
}

void * put_item(){
    int item = 2;

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
