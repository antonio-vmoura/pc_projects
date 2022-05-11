/*
Um outro problema clássico de IPC acontece em uma barbearia. Na barbearia há um barbeiro, 
uma cadeira de barbeiro e N cadeiras para eventuais clientes esperarem a vez.
Quando não há clientes, o barbeiro senta-se e cai no sono (fica bloqueado). 
Quando chega um cliente, ele precisa acordar o barbeiro. 
Se outros clientes chegarem enquanto o barbeiro estiver cortando o cabelo de um cliente, 
eles se sentarão (se houver cadeiras vazias) ou sairão da barbearia (se todas as cadeiras estiverem ocupadas). 
O problema é programar o barbeiro e os M clientes sem cair em condições de disputa. 
Esse problema é semelhante a situações com várias filas, 
como uma mesa de atendimento de telemarketing com diversos atendentes e com um sistema computadorizado de chamadas em espera, 
atendendo a um número limitado de chamadas que chegam.
*/

#include "pthread.h"
#include "semaphore.h"
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"

#define CUSTOMERS_NUM 50
#define CHAIRS_NUM 5

sem_t sem_barber_chair;
sem_t sem_custumer_chairs;
sem_t sem_customer_served;
sem_t sem_customer_in_chair;

void * barber(void *v) {

  while(1) {
    sem_wait(&sem_customer_in_chair);
    sleep(1);
    printf("Barbeiro cortou o cabelo de um cliente\n"); 
    sem_post(&sem_customer_served);
  }

  pthread_exit(0);
}

void* customer(void* v) {
  int id = *(int*) v;

  sleep(rand() % 3);

  if(sem_trywait(&sem_custumer_chairs) == 0) { //conseguiu pegar uma cadeira de espera
    printf("Cliente %d entrou na barbearia\n", id);
    sem_wait(&sem_barber_chair); //espera o barbeiro
    printf("Cliente %d sentou na cadeira do barbeiro!\n", id);
    sem_post(&sem_custumer_chairs); //libera a cadeira de espera
    sem_post(&sem_customer_in_chair); //notifica o barbeiro
    printf("Cliente %d aguardando barbeiro cortar\n", id);
    sem_wait(&sem_customer_served); //espera o barbeiro cortar o cabelo
    sem_post(&sem_barber_chair); //libera a cadeira do barbeiro
    printf("Cliente %d liberou cadeira do barbeiro!\n", id);
  }else{ //barbearia cheia
	  printf("Barbearia cheia, cliente %d indo embora\n",id);
  }

  pthread_exit(0);
}

int main() {
  pthread_t thr_customers[CUSTOMERS_NUM], thr_barber;
  int i, id[CUSTOMERS_NUM];

  sem_init(&sem_customer_in_chair, 0, 0);
	sem_init(&sem_barber_chair, 0, 1);
	sem_init(&sem_customer_served, 0, 0);
  sem_init(&sem_custumer_chairs, 0, CHAIRS_NUM);

  pthread_create(&thr_barber, NULL, barber, NULL);
  
  for (i = 0; i < CUSTOMERS_NUM; i++) {
    id[i] = i;
    pthread_create(&thr_customers[i], NULL, customer, (void*) &id[i]);
  }
  
  for (i = 0; i < CUSTOMERS_NUM; i++) {
    pthread_join(thr_customers[i], NULL);
  }
  
  return 0;
}