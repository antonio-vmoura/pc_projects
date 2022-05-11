/*
Uma tribo de N canibais selvagens está jantando os missionários capturados. 
Eles ficam ao redor de um grande caldeirão contendo X porções de ensopado de missionário cozido. 
Quando um selvagem tem fome, ele se serve no caldeirão (pega 1 porção), a menos que esteja vazio 
(neste caso, o canibal deve aguardar/dormir até que o cozinheiro forneça mais porções). Quando um canibal pegar a última porção, 
o mesmo deve acordar o cozinheiro para que ele prepare mais X porções de ensopado e encha o caldeirão. 
Após encher a tigela, o cozinheiro acorda os canibais selvagens que estavam esperando o ensopado ficar pronto e volta a dormir.
*/

#include "stdio.h"
#include "stdlib.h"
#include "pthread.h"
#include "unistd.h"

#define MAXCANNIBALS 20
#define MAXCOOKS 1

pthread_cond_t lock_ca = PTHREAD_COND_INITIALIZER;
pthread_cond_t lock_co = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock_turn = PTHREAD_MUTEX_INITIALIZER;

int portion = 0;

void *cannibal(void*meuid);
void *cooker(int m);

void main(argc, argv)
int argc;
char *argv[];
{
  int erro;
  int i, n, m;
  int *id;

  pthread_t tid[MAXCANNIBALS];

  if(argc != 3) {
    n = 5; //number of cannibals
    m = 2; //number of servings the cook can prepare at a time
  } else {
    n = atoi (argv[1]); 
    m = atoi (argv[2]); 
  }

  printf("numero de canibais: %d -- quantidade de comida: %d\n", n, m);

  if(n > MAXCANNIBALS){
    printf("o numero de canibais e' maior que o maximo permitido: %d\n", MAXCANNIBALS);
    exit(1);
  }
  
  for (i = 0; i < n; i++) {
    id = (int *) malloc(sizeof(int));
    *id = i;
    erro = pthread_create(&tid[i], NULL, cannibal, (void *) (id));

    if(erro) {
      printf("erro na criacao do thread %d\n", i);
      exit(1);
    }
  }

  cooker(m);
}

void * cannibal(void* pi) {
  
  while(1) {
    pthread_mutex_lock(&lock_turn);
      while(portion == 0) {
        pthread_cond_wait(&lock_ca, &lock_turn);
      }

      printf("Canibal: comendo a porcao\n");
      portion--;
      sleep(5);

      printf("Porções restantes: %d\n\n", portion);
      
      if (portion == 0){
        printf("Canibal: Acordando cozinheiro\n");
        pthread_cond_signal(&lock_co);
      }
    pthread_mutex_unlock(&lock_turn);
    
  }
  
}

void *cooker(int m) {
  while(1){

    pthread_mutex_lock(&lock_turn);
      if(portion != 0) { //sleep while you have food
        pthread_cond_wait(&lock_co, &lock_turn);
      }

      printf("Cozinheiro: fazendo a porção\n");
      portion += m;
      sleep(10);

      printf("Porções restantes: %d\n\n", portion);

      pthread_cond_broadcast(&lock_ca);
		pthread_mutex_unlock(&lock_turn);

   }

}
