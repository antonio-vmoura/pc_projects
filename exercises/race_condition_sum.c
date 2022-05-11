#include "stdio.h"
#include "unistd.h"
#include "stdlib.h"
#include "pthread.h"

#define N 4
#define X 100000

int counter = 0;
int unsafe_counter = 0;

pthread_mutex_t lock_counter = PTHREAD_MUTEX_INITIALIZER;

void * increment(void * arg) {
    int id = *((int *) arg); 

    printf("Criou um pthread com id = %d \n",id);

    for(int i = 0; i < X; i++){
        pthread_mutex_lock(&lock_counter); //catch the lock
        counter++;

        pthread_mutex_unlock(&lock_counter); //release the lock
        unsafe_counter++;
    }
    pthread_exit(0);    
}

int main() {
   int i;
   int * id;
   pthread_t a[N];

   for (i = 0; i < N ; i++) {
        id = (int *) malloc(sizeof(int));
        *id = i;
        pthread_create(&a[i], NULL, increment, (void *) (id));
   }

  for (i = 0; i < N ; i++) {
        pthread_join(a[i],NULL);
  }

  printf("\nValor final do counter = %d\n", counter); 
  printf("Valor final do unsafe_counter = %d\n", unsafe_counter); 
  printf("FIM!!!\n");

  return 0;
}
