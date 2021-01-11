#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NTHREADS 4

int x = 0; // Variavel de controle de execucao
pthread_mutex_t x_mutex;
pthread_cond_t x_cond;

/* Thread A */
void *A (void *t) {

    //printf("A: Comecei\n");
  
    printf("tudo bem?\n");

    pthread_mutex_lock(&x_mutex);
    x++;
    if (x==2) {
        //printf("A:  x = %d, vai sinalizar a condicao \n", x);
        pthread_cond_broadcast(&x_cond); // Emite sinal para todas as threads que estao esperando seguirem
    }
    pthread_mutex_unlock(&x_mutex);

    pthread_exit(NULL);
}

void *B (void *t) {

    //printf("B: Comecei\n");
  
    printf("bom dia!\n");

    pthread_mutex_lock(&x_mutex);
    x++;
    if (x==2) {
        //printf("B:  x = %d, vai sinalizar a condicao \n", x);
        pthread_cond_broadcast(&x_cond); // Emite sinal para todas as threads que estao esperando seguirem
    }
    pthread_mutex_unlock(&x_mutex);

    pthread_exit(NULL);
}

void *C (void *t) {

    //printf("C: Comecei\n");

    pthread_mutex_lock(&x_mutex);
    if (x < 2) { 
        //printf("C: x= %d, vai se bloquear...\n", x);
        pthread_cond_wait(&x_cond, &x_mutex); // Aguarda a emissao do sinal
        //printf("C: sinal recebido e mutex realocado, x = %d\n", x);
    }
  
    printf("até mais!\n");

    pthread_mutex_unlock(&x_mutex); 

    pthread_exit(NULL);
}

void *D (void *t) {
    
    //printf("D: Comecei\n");

    pthread_mutex_lock(&x_mutex);
    if (x < 2) { 
        //printf("D: x= %d, vai se bloquear...\n", x);
        pthread_cond_wait(&x_cond, &x_mutex); // Aguarda a emissao do sinal
        //printf("D: sinal recebido e mutex realocado, x = %d\n", x);
    }

    printf("boa tarde!\n");

    pthread_mutex_unlock(&x_mutex); 

    pthread_exit(NULL);
}

int main(void) {
    int i; 
    pthread_t threads[NTHREADS];

    /* Inicilaiza o mutex e a variavel de condicao */
    pthread_mutex_init(&x_mutex, NULL);
    pthread_cond_init (&x_cond, NULL);

    /* Cria as threads */
    pthread_create(&threads[3], NULL, A, NULL);
    pthread_create(&threads[2], NULL, B, NULL);
    pthread_create(&threads[1], NULL, C, NULL);
    pthread_create(&threads[0], NULL, D, NULL);

    /* Espera todas as threads terminarem */
    for (i = 0; i < NTHREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    printf ("\nFIM\n");

    /* Desaloca variaveis e termina */
    pthread_mutex_destroy(&x_mutex);
    pthread_cond_destroy(&x_cond);
}
