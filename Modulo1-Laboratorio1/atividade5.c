#include <stdio.h>
#include <pthread.h>

#define NTHREADS 2
#define VETORSIZE 50

int vetor[VETORSIZE];


// Função que será executada pelas threads
void * incrementar (void * arg) {
    int valorInicialIteracao = * (int *) arg;

    // A solução está em icrementar os valores pares em uma thread e os ímpares na outra, por isso a iteração de 2 em 2
    for(int i = valorInicialIteracao; i < VETORSIZE; i = i + 2) {
        vetor[i] = vetor[i] + 1;
    }
    pthread_exit(NULL);
}


int main(void) {

    pthread_t tid[NTHREADS];
    int ident[NTHREADS];
    int i;

    printf("Valores iniciais do vetor:\n");

    // Preenchendo o vetor e printando-o
    for (i = 0; i < VETORSIZE; i++) {
       vetor[i] = i;
       printf("%d ", vetor[i]);
    }

    printf("\n");

    printf("Incrementação sendo feita ...\n");
    // Criando as threads
    for (i = 0; i < NTHREADS; i++) {
        ident[i] = i;
        if(pthread_create(&tid[i], NULL, incrementar, (void *)&ident[i]) ) { 
            printf("ERRO -- pthread_create\n");
        }
    }

    // Esperando as threads terminarem
    for (int i = 0; i < NTHREADS; i++) {
        if(pthread_join(tid[i], NULL) ) { 
            printf("ERRO -- pthread_join\n");
        }
    }
    
    printf("Valores finais do vetor:\n");
    for (i = 0; i < VETORSIZE; i++) {
       printf("%d ", vetor[i]);
    }

    printf("\n");
    
    pthread_exit(NULL);
    return 0;
}