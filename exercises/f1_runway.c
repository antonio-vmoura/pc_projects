#include "stdlib.h"
#include "stdio.h"
#include "pthread.h"
#include "unistd.h"
#include "time.h"
#include "semaphore.h"

#define CARNUM 2
#define TEAMNUM 5
#define RUNWAYCAPACITY 4

int quantity = 0;

sem_t runway;
sem_t team[TEAMNUM];
pthread_t car[CARNUM*TEAMNUM];
pthread_mutex_t lock_qty = PTHREAD_MUTEX_INITIALIZER;

void * pilot(void *arg);

int main(int argc, char **argv) {
    int i;
    int *id;

    srand48(time(NULL));
    sem_init(&runway,0,RUNWAYCAPACITY);

    for(i=0; i < TEAMNUM; i++) { 
        sem_init(&team[i],0,1);
    }

    for(i = 0; i < CARNUM*TEAMNUM;i++) {
        id = (int *) malloc(sizeof(int));
        *id = i;
	    pthread_create(&(car[i]),NULL,pilot, (void*) (id));
    }

   pthread_join(car[0],NULL);
}

void * pilot(void *arg) {
    int id = *((int *) arg);
    int eq = id % TEAMNUM;

    printf("Carro %d da equipe %d criado\n", id, eq);

    while(1) {
        sleep(drand48()*3);

        sem_wait(&team[eq]);
        sem_wait(&runway);
                
        pthread_mutex_lock(&lock_qty);
            quantity++;
            printf("Carro %d treinando, quantidade: %d\n",id,quantity);
        pthread_mutex_unlock(&lock_qty);
        
        sleep(10*id);
        printf("Carro %d saindo da pista\n",id);

        pthread_mutex_lock(&lock_qty);
            quantity--;
        pthread_mutex_unlock(&lock_qty); 
        sem_post(&runway);
        sem_post(&team[eq]);
    }
}




















