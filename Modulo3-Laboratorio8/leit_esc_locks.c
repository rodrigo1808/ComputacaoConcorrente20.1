#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>
#include <unistd.h>

#define L 4 //numero de threads leitoras
#define E 2 //numero de threads escritoras

pthread_mutex_t mutex;
pthread_cond_t cond_escr, cond_leit;
int escritores = 0, leitores = 0, fila_escritores = 0;
int num = -1;

void * leitor (void * arg) {
  int *id = (int *) arg;
  while(1) {
    printf("L[%d] quer ler\n", *id);
    pthread_mutex_lock(&mutex);
    while(escritores > 0 || fila_escritores > 0) {
      printf("L[%d] bloqueou\n", *id);
      pthread_cond_wait(&cond_leit, &mutex);
      printf("L[%d] desbloqueou\n", *id);
    }
    leitores++;
    pthread_mutex_unlock(&mutex);

    printf("L[%d] esta lendo\n", *id);
    printf("L[%d] leu %d\n", *id, num);
    sleep(1);

    pthread_mutex_lock(&mutex);
    leitores--;
    pthread_cond_signal(&cond_escr);

    pthread_mutex_unlock(&mutex);
    printf("L[%d] terminou de ler\n", *id);
    
  } 
  free(arg);
  pthread_exit(NULL);
}

void * escritor (void * arg) {
  int *id = (int *) arg;
  while(1) {
    printf("E[%d] quer escrever\n", *id);
    pthread_mutex_lock(&mutex);
    fila_escritores++;
    while(escritores > 0) {
      printf("E[%d] bloqueou\n", *id);
      pthread_cond_wait(&cond_escr, &mutex);
      printf("E[%d] desbloqueou\n", *id);
    }

    escritores++;
    pthread_mutex_unlock(&mutex);

    printf("E[%d] esta escrevendo\n", *id);
    num = *id;
    printf("E[%d] escreveu %d\n", *id, num);
    sleep(1);

    pthread_mutex_lock(&mutex);
    fila_escritores--;
    escritores--;

    if(fila_escritores > 0) {
      pthread_cond_signal(&cond_escr);
    } else {
      pthread_cond_broadcast(&cond_leit);
    }
    pthread_mutex_unlock(&mutex);

    pthread_mutex_unlock(&mutex);
    printf("E[%d] terminou de escrever\n", *id);
    sleep(2);
  } 
  free(arg);
  pthread_exit(NULL);
}

//funcao principal
int main(void) {
  //identificadores das threads
  pthread_t tid[L+E];
  int id[L+E];

  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&cond_leit, NULL);
  pthread_cond_init(&cond_escr, NULL);

  //cria as threads leitoras
  for(int i=0; i<L; i++) {
    id[i] = i+1;
    if(pthread_create(&tid[i], NULL, leitor, (void *) &id[i])) exit(-1);
  } 
  
  //cria as threads escritoras
  for(int i=0; i<E; i++) {
    id[i+L] = i+1;
    if(pthread_create(&tid[i+L], NULL, escritor, (void *) &id[i+L])) exit(-1);
  } 

  pthread_exit(NULL);
  return 0;
}