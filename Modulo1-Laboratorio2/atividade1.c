#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer.h"

float *matriz; // Matriz que será multiplicada
float *matriz2; // Matriz que multiplicará
float *resultado; // Matriz de resultado da multiplicação
int nthreads; // Número de threads
int dim; // Dimensão da matriz


typedef struct {
    int id; 
} tArgs;

// funcao que as threads executarao
void * tarefa(void *arg) {
    tArgs *args = (tArgs*) arg;
    for(int i = args->id; i < dim; i+=nthreads) {
        for(int j = 0; j < dim; j++) {
            for(int k = 0; k < dim; k++) {
                resultado[i * dim + j] += matriz[i * dim + k] * matriz2[k * dim + i];
            }
        }
    }

    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {

    pthread_t *tid; //identificadores das threads no sistema
    tArgs *args; //identificadores locais das threads e dimensao
    double inicio, fim, delta;

    GET_TIME(inicio);

    //leitura e avaliacao dos parametros de entrada
    if(argc < 3) {
        printf("Digite: %s <dimensao da matriz> <numero de threads>\n", argv[0]);
        return 1;
    }

    dim = atoi(argv[1]);
    nthreads = atoi(argv[2]);

    if(nthreads>dim) {
        nthreads = dim;
    }

    //alocacao de memoria para as estruturas de dados
    matriz = (float *) malloc(sizeof(float) * dim * dim);
    if(matriz == NULL) {
        printf("Erro--malloc\n");
        return 2;
    }

    matriz2 = (float *) malloc(sizeof(float) * dim * dim);
    if(matriz2 == NULL) {
        printf("Erro--malloc\n");
        return 2;
    }

    resultado = (float *) malloc(sizeof(float) * dim * dim);
    if(resultado == NULL) {
        printf("Erro--malloc\n");
        return 2;
    }

    //inicializacao das estruturas de dados
    for(int i = 0; i < dim; i++) {
        for(int j = 0; j < dim; j++) {
            matriz[i * dim + j] = i+1;
            matriz2[i * dim + j] = 1;
            resultado[i * dim + j] = 0;
        }
    }

    GET_TIME(fim);

    delta = fim - inicio;
    printf("Tempo inicializacao: %lf\n", fim - inicio);

    //multiplicacao da matriz pelo vetor
    GET_TIME(inicio);
    //alocacao das estruturas
    tid = (pthread_t*) malloc(sizeof(pthread_t)*nthreads);
    if(tid==NULL) {
        puts("ERRO--malloc");
        return 2;
    }

    args = (tArgs*) malloc(sizeof(tArgs)*nthreads);
    if(args==NULL) {
        puts("ERRO--malloc");
        return 2;
    }

    //criacao das threads
    for(int i = 0; i < nthreads; i++) {
        (args+i)->id = i;
        if(pthread_create(tid+i, NULL, tarefa, (void*)(args + i))) {
            puts("ERRO--pthread_create");
            return 3;
        }
    }

    //espera pelo termino das threads
    for(int i = 0; i < nthreads; i++) {
        pthread_join(*(tid+i), NULL);
    }

    GET_TIME(fim);
    delta += fim - inicio;
    printf("Tempo multiplicacao: %lf\n", fim - inicio);

    GET_TIME(inicio);
    //exibicao dos resultados
    /*puts("Matriz de entrada");
    for(int i = 0; i < dim; i++) {
        for(int j = 0; j < dim; j++) {
            printf("%.lf ", matriz[i*dim+j]);
        }
        puts("");
    }

    puts("Matriz de entrada");
    for(int i = 0; i < dim; i++) {
        for(int j = 0; j < dim; j++) {
            printf("%.lf ", matriz2[i*dim+j]);
        }
        puts("");
    }

    puts("Matriz de saída");
    for(int i = 0; i < dim; i++) {
        for(int j = 0; j < dim; j++) {
            printf("%.lf ", resultado[i*dim+j]);
        }
        puts("");
    }*/

    //liberacao da memoria
    free(matriz);
    free(matriz2);
    free(resultado);
    free(args);
    free(tid);

    GET_TIME(fim);
    delta += fim - inicio;
    printf("Tempo finalizacao: %lf\n", fim - inicio);
    printf("Tempo total: %lf\n", delta);

    return 0;
}