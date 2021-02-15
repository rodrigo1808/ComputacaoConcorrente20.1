#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include "timer.h"

int *array; // Ponteiro para nosso array de números que serão ordenados
int size; // Tamanho do array
int nthreads; // Numero de threads
int freeThreads = 0; // Número de threads livres
void * quicksort(void * arg); // Pré declaração da função que as threads irão executar

pthread_mutex_t mutex;
pthread_cond_t cond;

// Estrutura auxiliar para cada thread
// Sera usada para apontar para a tarefa da thread e passar os parametros 
typedef struct {
    pthread_t *tid;
    int id;
    int start;
    int end;
    int busy;
} tArgs;

tArgs *args;

// Checa se tem alguma thread livre usando o parametro busy das threads
// Caso tenha retorna o index da thread livre, caso não tenha retorna -1
int freeThread() {
    int freeThread = -1;
    for(int i = 0; i < nthreads; i++) {
        if( !((args + i)->busy) ) {
            freeThread = i;
            break;
        }
    }

    return freeThread;
}


// Metodo que aloca uma thread para executar o quicksort
int EnterQuicksort() {
    pthread_mutex_lock(&mutex);
    //printf("Monitor quer usar quicksort\n");

    // Verifica se tem thread livre caso nao tenha fica preso no wait esperando um signal
    int positionFreeThread = freeThread();
    while(positionFreeThread == -1) {
        //printf("Monitor bloqueou\n");
        pthread_cond_wait(&cond, &mutex);
        //printf("Monitor desbloqueou\n");
        positionFreeThread = freeThread();
    }

    // Informa que a thread esta ocupada e retorna o index da thread que ocupou
    freeThreads--;
    (args + positionFreeThread)->busy = 1;
    pthread_mutex_unlock(&mutex);
    return positionFreeThread;
}

// Metodo que faz o looping de comparacao do quicksort, retornando seu pivot no final
int partition(int start, int end) {
    pthread_mutex_lock(&mutex);
    int pivot = end;
    int i = start - 1;
    int j;
    int aux;

    for(j = start; j <= end - 1; j++) {
        if(array[j] <= array[pivot]) {
            i++;
            if(array[i] != array[j]) {
                aux = array[i];
                array[i] = array[j];
                array[j] = aux; 
            }         
        }
    }

    aux = array[i + 1];
    array[i + 1] = array[end];
    array[end] = aux;
    pthread_mutex_unlock(&mutex);

    return i + 1;
}

// Metodo para fazer o controle de quais threads serao usadas, chama-las e passar seus parametros
void monitor(int start, int end) {
    // Checa se tem thread livre para ser usada
    int freeThreadId = EnterQuicksort();

    pthread_mutex_lock(&mutex);

    // Atribui os argumentos do quicksort na estrura auxiliar da thread
    (args + freeThreadId)->start = start;
    (args + freeThreadId)->end = end;

    printf("MONITOR USOU UM QUICK SORT NA T[%d] INTERVALO %d - %d\n", freeThreadId, start, end);

    // Cria a thread passando a estrura auxiliar como parâmetro
    pthread_create((args + freeThreadId)->tid, NULL, quicksort, (void*)(args + freeThreadId));

    pthread_mutex_unlock(&mutex);
}

// Metodo executado pela thread
void * quicksort(void * arg) {
    tArgs *thread = (tArgs *) arg;

    // Aloca os valores dos parametros em variaveis locais
    int start = thread->start;
    int end = thread->end;
    printf("T[%d] ENTROU NO QUICKSORT COM INTEVALO %d - %d\n", thread->id, start, end);

    if(start < end) {

        int pivot = partition(start, end);
        
        // Sinaliza que a thread foi liberada, para ser acessada novamente e emite um signal
        thread->busy = 0;
        printf("T[%d] liberada\n", thread->id);
        freeThreads++;
        pthread_cond_signal(&cond);
        monitor(start, pivot - 1);
        monitor(pivot + 1, end);
        pthread_exit(NULL);
    }

    // Sinaliza que a thread foi liberada, para ser acessada novamente e emite um signal
    thread->busy = 0;
    printf("T[%d] liberada\n", thread->id);
    freeThreads++;
    pthread_cond_signal(&cond);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {

    double inicio, fim; 
    int i;
    time_t t;
    srand((unsigned) time(&t));

    if(argc < 3) {
        fprintf(stderr, "Digite: %s <dimensao do vetor> <numero de threads>\n", argv[0]);
        return 1;
    }

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    // Pegando os parametros de entrada
    size = atoll(argv[1]);
    nthreads = atoll(argv[2]);
    freeThreads = nthreads;

    // Alocando espaço no array
    array = (int *) malloc(sizeof(int) * size);
    if(array == NULL) {
        fprintf(stderr, "ERRO--malloc\n");
        return 2;
    }

    // Inicializando o array com valores randômicos de 1 a numElementos + 1
    for(i = 0; i < size; i++) {
        array[i] = rand() % (size + 1) + 1;
    }

    GET_TIME(inicio);

    // Alocando espaço da estrutura auxiliar
    args = (tArgs*) malloc(sizeof(tArgs)*nthreads);
    if(args==NULL) {
        puts("ERRO--malloc");
        return 2;
    }

    // Inicializando com os valores default da estrutura auxiliar
    for(i = 0; i < nthreads; i++) {
        (args + i)->busy = 0;
        (args + i)->id = i;
        (args + i)->tid = (pthread_t*) malloc(sizeof(pthread_t));
    }

    // A primeira chamada do quicksort vai ser feita na main
    // as proximas serao controladas pelo monitor
    int pivot = partition(0, size - 1);

    monitor(0, pivot - 1);
    monitor(pivot + 1, size - 1);
    
    GET_TIME(fim);

    // Sleep para contornar o problema de nao ter controle sobre a condiçao de parada
    sleep(5);

    // Print do array ordenado
    for(int i = 0; i < size; i++) {
       printf("%d ", array[i]);
    }

    printf("\nTempo: %lf\n", fim - inicio);

    // Liberando os espaços alocados
    free(array);
    for(i = 0; i < nthreads; i++) {
        free((args + i)->tid);
    }
    free(args);

    return 0;
}