#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

#define L 4 //numero de threads leitoras
#define E 2 //numero de threads escritoras
#define SIZE 10

//variaveis do problema
int leit=0; //contador de threads lendo
int escr=0; //contador de threads escrevendo
int filaEscr = 0; // contador de escritores na fila
int vetor[SIZE]; // Vetor de inteiros

//variaveis para sincronizacao
pthread_mutex_t mutex;
pthread_cond_t cond_leit, cond_escr, cond_contLeit;

//entrada leitura
void InicLeit (int id) {
   pthread_mutex_lock(&mutex);
   printf("L[%d] quer ler\n", id);
   while((escr > 0) || (filaEscr > 0)) { // Se tiver escritor escrevendo ou escritores na fila bloqueia
     printf("L[%d] bloqueou\n", id);
     pthread_cond_wait(&cond_leit, &mutex);
     printf("L[%d] desbloqueou\n", id);
   }
   leit++;
   pthread_mutex_unlock(&mutex);
}

//saida leitura
void FimLeit (int id) {
   pthread_mutex_lock(&mutex);
   printf("L[%d] terminou de ler\n", id);
   leit--;
   if(leit==0) pthread_cond_signal(&cond_escr);
   pthread_mutex_unlock(&mutex);
}

//entrada escrita
void InicEscr (int id) {
   pthread_mutex_lock(&mutex);
   filaEscr++;
   printf("E[%d] quer escrever - Nenhum leitor vai comecar a leitura:\n", id);
   while(escr > 0) { // Se tiver escritor escrevendo
     printf("E[%d] bloqueou\n", id);
     pthread_cond_wait(&cond_escr, &mutex);
     printf("E[%d] desbloqueou\n", id);
   }
   escr++;
   pthread_mutex_unlock(&mutex);
}

//saida escrita
void FimEscr (int id) {
   pthread_mutex_lock(&mutex);
   pthread_cond_broadcast(&cond_contLeit);
   printf("E[%d] terminou de escrever\n", id);
   filaEscr--;
   escr--;
   if(filaEscr == 0) printf("Leitores podem ler\n"); // Se não tiver escritores na fila a leitura é liberada
   pthread_cond_signal(&cond_escr);
   pthread_cond_broadcast(&cond_leit);
   pthread_mutex_unlock(&mutex);
}

//thread leitora
void * leitor (void * arg) {
  int *id = (int *) arg;
  while(1) {
    InicLeit(*id);
    pthread_mutex_lock(&mutex);
    while(filaEscr>0) { pthread_cond_wait(&cond_contLeit, &mutex); } // Para bloquear a leitura enquanto existir escritor esperando 
    int soma = 0;
    printf("L[%d] leu o vetor: [ ", *id);
    for(int i = 0; i < SIZE; i++) {
        printf("%d ", vetor[i]);
        soma += vetor[i];
    }
    printf("] Média: %f\n", soma/(SIZE * 1.0));
    pthread_mutex_unlock(&mutex);
    FimLeit(*id);
    sleep(1);
  } 
  free(arg);
  pthread_exit(NULL);
}

//thread leitora
void * escritor (void * arg) {
  int *id = (int *) arg;
  while(1) {
    InicEscr(*id);
    pthread_mutex_lock(&mutex);
    printf("Escritora %d esta escrevendo\n", *id);
    vetor[0] = *id;
    for(int i = 1; i < SIZE - 1; i++) {
        vetor[i] = *id * 2;
    }
    vetor[SIZE - 1] = *id;
    pthread_mutex_unlock(&mutex);
    FimEscr(*id);
    sleep(1);
  } 
  free(arg);
  pthread_exit(NULL);
}

//funcao principal
int main(void) {
  //identificadores das threads
  pthread_t tid[L+E];
  int id[L+E], i;

  //inicializa as variaveis de sincronizacao
  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&cond_leit, NULL);
  pthread_cond_init(&cond_escr, NULL);
  pthread_cond_init(&cond_contLeit, NULL);

  // Preenche o vetor com zeros
  for(i = 0; i < SIZE; i++) {
      vetor[i] = 0;
  }

  //cria as threads leitoras
  for(i=0; i<L; i++) {
    id[i] = i+1;
    if(pthread_create(&tid[i], NULL, leitor, (void *) &id[i])) exit(-1);
  } 
  
  //cria as threads escritoras
  for(i=0; i<E; i++) {
    id[i+L] = i+1;
    if(pthread_create(&tid[i+L], NULL, escritor, (void *) &id[i+L])) exit(-1);
  } 

  pthread_exit(NULL);
  return 0;
}
