#include "time.h"
#include "stdio.h"
#include "unistd.h"
#include "stdlib.h"
#include "pthread.h"
#include "semaphore.h"

#define TDOC 2 //Total de medicos
#define TNUR 2 //Total de enfermeiros
#define TPAT 10 //Total de pacientes
#define TRES 2 //Total de respiradores
#define BMED 20 //Buffer dos remedios
#define BBED 5 //Buffer dos Leitos

sem_t bed_count; //Contador de leitos
sem_t lock_medicine; //Mutex da variavel medicine_qty
sem_t lock_discharged; //Mutex da variavel alta
sem_t doctor_flag; //Semaforo flag para acordar o medico
sem_t nurse_flag; //Semaforo flag para acordar a enfermeira
sem_t respirator_flag; //Semaforo flag para acordar o respirador
sem_t free_respirator; //Semaforo flag para liberar o respirador
sem_t free_bed; //Semaforo flag para indicar leito liberado
sem_t catch_respirator; //Semaforo flag buscar o respirador
sem_t free_patient; //Semaforo flag para liberar paciente que pediu alta
sem_t free_medicine; //Semaforo flag para liberar paciente apos reposicao dos remedios

int beds_used = 0; //Contador de pacientes que pediram alta
int medicine_qty = BMED; //Quantidade de remedio
int medicine = 0;

void* Doctor(void *arg);
void* Nurse(void *arg);
void* Patient(void *arg);
void* Respirator(void *arg);

int main() {
  
	//Inicializando os semaforos
	sem_init(&bed_count, 0, BBED);
	sem_init(&lock_medicine, 0, 1);
	sem_init(&lock_discharged, 0, 1);
	sem_init(&catch_respirator, 0, 0);
	sem_init(&doctor_flag, 0, 0);
	sem_init(&free_patient, 0, 0);
	sem_init(&nurse_flag, 0, 0);
	sem_init(&respirator_flag, 0, 0);
 	sem_init(&free_respirator, 0, 0);
	sem_init(&free_bed, 0, 0);
	sem_init(&free_medicine, 0, 0);
	
	pthread_t doctor[TDOC], nurse[TNUR], patient[TPAT], ambu[TRES];

	int *id;
	
	//criando medico
	for (int i = 0; i < TDOC ; i++) {
		id = (int *) malloc(sizeof(int));
		*id = i;
		pthread_create(&doctor[i], NULL, Doctor, (void *) (id));
	}

	//criando enfermeiros
	for (int i = 0; i < TNUR ; i++) {
		id = (int *) malloc(sizeof(int));
		*id = i;
		pthread_create(&nurse[i], NULL, Nurse, (void *) (id));
	}
	
	//criando paciente
	for (int i = 0; i < TPAT ; i++) {
		id = (int *) malloc(sizeof(int));
		*id = i;
		pthread_create(&patient[i], NULL, Patient, (void *) (id));
	}
	
	//criando ambulancia
	for (int i = 0; i < TRES ; i++) {
		id = (int *) malloc(sizeof(int));
		*id = i;
		pthread_create(&ambu[i], NULL, Respirator, (void *) (id));
	}
	
	for (int i = 0; i < TPAT; i++){
	  pthread_join(patient[i],NULL);
	}
	
	return 0;
}

void* Doctor(void *arg) {
	int i = *((int *) arg);
	
	while(1) {
		printf("\n Medico %d pronto esperando instrucoes!", i);
		sem_wait(&doctor_flag);
		sem_wait(&lock_discharged);
	    while (beds_used != 0){
			printf("\n Medico %d assinando alta", i);
			sem_post(&free_patient);
			printf("\n Medico %d liberando paciente", i);
			beds_used--;
		}
		sem_post(&lock_discharged);
	}
    pthread_exit(0);
}

void* Nurse(void *arg) {
	int i = *((int *) arg);
	
	while(1) {
		printf("\n Enfermeiro %d pronto esperando instrucoes!", i);
		sem_wait(&nurse_flag);
		sem_wait(&lock_medicine);
		if (medicine_qty < medicine) {
			printf("\n Enfermeiro %d repondo soro", i);
			medicine_qty = medicine_qty + medicine + 10;
			medicine = 0;
			sem_post(&free_medicine);
		}

		sem_post(&lock_medicine);
		sem_wait(&lock_discharged);
		
		if (beds_used != 0) {
			printf("\n Enfermeiro %d chamando medico", i);
			sem_post(&doctor_flag);
		}
		sem_post(&lock_discharged);
	}
	pthread_exit(0);
}

void* Respirator(void *arg) {
	int i = *((int *) arg);
	
	while(1){
		printf("\n Respirador %d pronto esperando instrucoes!", i);
		sem_wait(&respirator_flag);
		printf("\n Respirador %d foi requisitado", i);
		printf("\n Respirador %d sendo colocado no paciente", i);
		sem_post(&catch_respirator);
		printf("\n Respirador %d colocado no paciente", i);
		sem_wait(&free_respirator);
		printf("\n Respirador %d liberado", i);
	}
	pthread_exit(0);
}

void* Patient(void *arg) {
	int i = *((int *) arg);
	int necessary_medicine, need_bed, need_respirator; 
	
	while(1){
		
		srand ((unsigned)time(NULL));
		printf("\n Paciente %d infectado, indo para o hospital!", i);

		need_respirator = (rand()% 10);	
		necessary_medicine = (rand()% 3) + 1;
		need_bed = 0;
	
		if(need_respirator < 4){
			printf("\n Paciente %d requisitou o respirador", i);
			sem_post(&respirator_flag);
			sem_wait(&catch_respirator);
			printf("\n Paciente %d liberou o respirador", i);
		}
		else{
			printf("\n Paciente %d não precisou do respirador", i);
		}
		
		while (need_bed == 0 ){
		  if(sem_trywait(&bed_count) == 0){
			if(need_respirator < 4){
				printf("\n Paciente %d liberou o respirador.", i);
			}

			sem_post(&free_respirator);
			printf("\n Paciente %d ocupou um leito.", i);
			printf("\n Paciente %d precisa de remedio.", i);
			sem_wait(&lock_medicine);

			if (medicine_qty - necessary_medicine < 0){
				printf("\n Paciente %d quantidade solicitada em falta, requisitando enfermeiro.", i);
				medicine = necessary_medicine;
				sem_post(&lock_medicine);
				sem_post(&nurse_flag);
				sem_wait(&free_medicine);
			}

			medicine_qty = medicine_qty - necessary_medicine;
			sem_post(&lock_medicine);
			printf("\n Paciente %d recebendo remedio.", i);

			while (necessary_medicine != 0){
				sleep(5);
				necessary_medicine--;
			}

			printf("\n Paciente %d pronto para receber alta.", i);
			sem_wait(&lock_discharged);
			beds_used++;
			sem_post(&lock_discharged);
			sem_post(&nurse_flag);
			sem_wait(&free_patient);
			sem_post(&bed_count);
			need_bed = 1;
			sem_post(&free_bed);
			printf("\n Paciente %d liberado!", i);
		} else {
			  printf("\n Paciente %d nao ha leitos, paciente esperando.", i);
			  if (need_respirator < 4) {
				  printf("\n Paciente %d nao liberou o respirador.", i);
			  }
			  sem_wait(&free_bed);
		  }
		}
	}	
	pthread_exit(0);
}
