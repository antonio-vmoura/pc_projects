#include "stdio.h"
#include "stdlib.h"
#include "pthread.h"
#include "unistd.h"

#define N 10

int counter = 0;

void * pthread_func(void * arg) {
    int id = *((int *) arg);
    printf("CRIOU UM PTHREAD COM ID = %d \n", id);
    
    for (int j = 0; j < 100000 ; j++){
        counter++;
    } 

    pthread_exit(0);    
}

int main() {
    int * id;
    pthread_t a[N];

    printf("\nMEU ID: %lu\n\n", pthread_self());
    
    for (int i = 0; i < N ; i++) {
        id = (int *) malloc(sizeof(int));
        *id = i;
        pthread_create(&a[i], NULL, pthread_func, (void *) (id));
    }

    for (int i = 0; i < N ; i++) {
        pthread_join(a[i], NULL);
    }

    printf("\nCONTADOR = %d\n\n", counter);
    
    return 0;
}