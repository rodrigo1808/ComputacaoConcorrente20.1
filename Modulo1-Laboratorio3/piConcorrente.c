#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include "timer.h"

long long int numElementos; // Numero de elementos a se somar
int nthreads; // Numero de threads

void * tarefa(void * arg) {
    long int id = (long int) arg;

    double *somaLocal;
    somaLocal = (double *) malloc(sizeof(double));
    if(somaLocal == NULL) {
        fprintf(stderr, "ERRO-malloc\n");
        exit(1);
    }
    *somaLocal = 0;

    for(int i = id; i < numElementos; i+=nthreads) {
        *somaLocal += pow(-1, i) * (4.0/((2*i) + 1));
    }

    pthread_exit((void *) somaLocal);
}

int main(int argc, char *argv[]) {

    double somaSerie = 0; // Soma dos elementos da serie
    double inicio, fim; 
    pthread_t *tid;
    double *retorno; // Guarda o retorno das threads

    // Recebendo parâmetros
    if(argc < 3) {
        fprintf(stderr, "Digite: %s <numero de elementos> <numero de threads>\n", argv[0]);
        return 1;
    }

    numElementos = atoll(argv[1]);
    nthreads = atoi(argv[2]);

    GET_TIME(inicio);
    tid = (pthread_t *) malloc(sizeof(pthread_t) * nthreads);
    if(tid == NULL) {
        fprintf(stderr, "ERRO--malloc\n");
        return 2;
    }

    for(long int i = 0; i < nthreads; i++) {
        if(pthread_create(tid + i, NULL, tarefa, (void *) i)) {
            fprintf(stderr, "ERRO--pthread_create\n");
            return 3;
        }
    }

    //aguardar o termino das threads
    for(long int i = 0; i < nthreads; i++) {
        if(pthread_join(*(tid+i), (void **) &retorno)) {
            fprintf(stderr, "ERRO--pthread_join\n");
            return 3;
        }
        //soma a soma de elementos da thread a soma total
        somaSerie += *retorno;
        free(retorno);
    }
    GET_TIME(fim);

    printf("Tempo concorrente: %lf\n", fim - inicio);

    printf("SOMA DA SERIE: %.15lf\n", somaSerie);
    printf("MATH_PI: %.15lf\n", M_PI);

    printf("DIFERENCA: %.15lf\n", M_PI - (somaSerie));

    free(tid);

    return 0;
}