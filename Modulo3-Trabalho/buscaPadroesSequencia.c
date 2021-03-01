#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include "timer.h"

#define N 10 // Tamanho dos blocos
#define M 3 // Quantidade de blocos
// M x N deve ser igual ao tamanho da sequência

FILE *ptr; // Ponteiro para leitura do arquivo

// Struct de argumentos para as threads
typedef struct {
    int id;
    pthread_t *tid;
} threadStruct;

threadStruct *threadsArgs;

// Struct que representa os blocos
typedef struct {
    long long int *array;
    int status; // 0 - Não preenchido, 1 - Preenchido, 2 - Processado
    int buscaA; // 0 - Busca ainda nao feita, 1 - Busca do padrao A feita
    int buscaB; // 0 - Busca ainda nao feita, 1 - Busca do padrao B feita
    int buscaC; // 0 - Busca ainda nao feita, 1 - Busca do padrao C feita
    sem_t em;
    sem_t preenchido;
} itemBuffer;

// Array de tamanho M que contera os blocos
itemBuffer *buffer;

// Variavel que guardara o primeiro valor do arquivo binario
long long int numElementosArquivo = -1;

// Variaveis de resposta do padrao A
long long int padraoAPosInicial = -1; 
long long int padraoATamanho = -1; 
long long int padraoAValor = -1;

// Variaveis de resposta do padrao B
long long int padraoBQuantidade = 0; 

// Variaveis de resposta do padrao C
long long int padraoCQuantidade = 0; 


// Metodo de leitura da sequencia do arquivo binario e separacao em blocos
void * leituraArquivo(void * arg) {
    threadStruct *thread = (threadStruct *) arg;
    printf("T[%d] iniciou a leitura do arquivo\n", thread->id);
    long long int numerosArquivo[N];
    
    ptr = fopen("sequencia.bin","rb");

    // Caso seja o primeiro numero a ser lido sabemos que eh o valor informante do tamanho da sequencia
    if(numElementosArquivo == -1)
        fread(&numElementosArquivo, sizeof(numElementosArquivo), 1, ptr);

    // Para cada bloco
    for(int i = 0; i < M; i++) {
        fread(numerosArquivo, sizeof(numerosArquivo), 1, ptr);

        // Para cada numero do bloco
        for(int j = 0; j < N; j++) {
            (buffer+i)->array[j] =  numerosArquivo[j];
        }

        // Sinaliza para as threads de busca que podem utilizar o bloco preenchido
        (buffer+i)->status = 1;
        printf("T[%d] liberou o bloco %d\n", thread->id, i + 1);
        sem_post(&((buffer+i)->preenchido));
    }

    printf("T[%d] finalizou leitura\n", thread->id);
    
    pthread_exit(NULL);
}

// Metodo de busca do padrao A - Maior sequencia de valores identicos
void * buscaPadraoA(void * arg) {
    threadStruct *thread = (threadStruct *) arg;
    long long int buscaPosInicial = 0;
    long long int buscaTamanho = 1;
    int sequenciaAtiva = 0;
    long long int valorAtual;
    long long int valorAnterior = -1;
    
    for(int i = 0; i < M; i++) {
        printf("T[%d] quer buscar bloco %d\n", thread->id, i+1);
        sem_wait(&((buffer+i)->preenchido));
        sem_post(&((buffer+i)->preenchido));

        for(int j = 0; j < N; j++) {
            valorAtual = (buffer+i)->array[j];

            if(valorAnterior == valorAtual) {

                // Caso ja esteja em uma sequencia
                if(sequenciaAtiva) {
                    buscaTamanho++;
                } else { // Caso nao esteja, guarda o inicio e define como uma sequencia ativa
                    buscaTamanho = 2;
                    buscaPosInicial = (i * N) + j;
                    sequenciaAtiva = 1;
                }
            } else {
                if(buscaTamanho > padraoATamanho) {
                    padraoATamanho = buscaTamanho;
                    padraoAPosInicial = buscaPosInicial;
                    padraoAValor = valorAnterior;
                }

                buscaTamanho = 0;
                sequenciaAtiva = 0;
            }

            valorAnterior = (buffer+i)->array[j];
        }

        // Caso chegue no ultimo valor dos numeros e esteja em uma sequencia de iguais maior do que todas
        if(buscaTamanho > padraoATamanho) {
            padraoATamanho = buscaTamanho;
            padraoAPosInicial = buscaPosInicial;
            padraoAValor = valorAnterior;
        }

        // Seção crítica protegida pela exclusão mútua
        sem_wait(&((buffer+i)->em));

        (buffer+i)->buscaA = 1;
        // Checa se as outras threads terminaram
        if((buffer+i)->buscaB == 1 && (buffer+i)->buscaC == 1)
            (buffer+i)->status = 2;

        sem_post(&((buffer+i)->em));

    }

    printf("T[%d] finalizou busca por padrao A\n", thread->id);

    pthread_exit(NULL);
}

// Metodo de busca do padrao B - Quantidade de triplas
void * buscaPadraoB(void * arg) {
    threadStruct *thread = (threadStruct *) arg;
    long long int valorAtual = -1, valorAnterior = -1, buscaVezesIguais = 1;
    
    for(int i = 0; i < M; i++) {
        printf("T[%d] quer buscar bloco %d\n", thread->id, i+1);
        sem_wait(&((buffer+i)->preenchido));
        sem_post(&((buffer+i)->preenchido));

        for(int j = 0; j < N; j++) {
            valorAtual = (buffer+i)->array[j];
            if(valorAnterior == valorAtual) {
                buscaVezesIguais++;   
            } else {
                buscaVezesIguais = 1;
            }

            // Caso encontre uma tripla, soma em 1 a quantidade de triplas
            if(buscaVezesIguais == 3) {
                padraoBQuantidade++;
                buscaVezesIguais = 1;
                valorAnterior = -1;
            } else {
                valorAnterior = (buffer+i)->array[j];
            }
        }

        // Seção crítica protegida pela exclusão mútua
        sem_wait(&((buffer+i)->em));

        (buffer+i)->buscaB = 1;
        // Checa se as outras threads terminaram
        if((buffer+i)->buscaA == 1 && (buffer+i)->buscaC == 1)
            (buffer+i)->status = 2;

        sem_post(&((buffer+i)->em));
    }

    printf("T[%d] finalizou busca por padrao B\n", thread->id);

    pthread_exit(NULL);
}

// Metodo de busca do padrao C - Quantidade de ocorrencias da sequencia <012345>
void * buscaPadraoC(void * arg) {
    threadStruct *thread = (threadStruct *) arg;
    long long int valorAtual = -1, valorAnterior = -1, buscaNumSequencia = 0;
    
    for(int i = 0; i < M; i++) {
        printf("T[%d] quer buscar bloco %d\n", thread->id, i+1);
        sem_wait(&((buffer+i)->preenchido));
        sem_post(&((buffer+i)->preenchido));

        for(int j = 0; j < N; j++) {
            valorAtual = (buffer+i)->array[j];

            // Caso o valor anterior seja uma sequencia crescente do atual
            if(valorAnterior + 1 == valorAtual) {
                buscaNumSequencia++;   
            } else {
                buscaNumSequencia = 0;
            }

            // Caso tenhamos chegado em uma sequencia de 5 valores
            if(buscaNumSequencia == 5) {
                padraoCQuantidade++;
                buscaNumSequencia = 0;
            }

            valorAnterior = (buffer+i)->array[j];
        }

        // Seção crítica protegida pela exclusão mútua
        sem_wait(&((buffer+i)->em));

        (buffer+i)->buscaC = 1;
        // Checa se as outras threads terminaram
        if((buffer+i)->buscaA == 1 && (buffer+i)->buscaB == 1)
            (buffer+i)->status = 2;

        sem_post(&((buffer+i)->em));
    }

    printf("T[%d] finalizou busca por padrao C\n", thread->id);

    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {

    double startTime, endTime; // Variaveis de tempo

    GET_TIME(startTime); // Inicia a contagem

    // Alocando espaço da estrutura auxiliar
    threadsArgs = (threadStruct*) malloc(sizeof(threadStruct)*4);
    if(threadsArgs == NULL) {
        puts("ERRO--malloc");
        return 2;
    }

    // Preenchendo a estrutura auxiliar
    for(int i = 0; i < 4; i++) {
        (threadsArgs + i)->id = i;
        (threadsArgs +i)->tid = (pthread_t*) malloc(sizeof(pthread_t));
    }

    // Alocando o espaco do buffer e inicializando com seus valores iniciais
    buffer = (itemBuffer*) malloc(sizeof(itemBuffer)*M);
    for(int i = 0; i < M; i++) {
        (buffer + i)->status = 0;
        (buffer + i)->buscaA = 0;
        (buffer + i)->buscaB = 0;
        (buffer + i)->buscaC = 0;
        (buffer + i)->array = (long long int*) malloc(sizeof(long long int)*N);
        sem_init(&((buffer + i)->preenchido), 0, 0);
        sem_init(&((buffer + i)->em), 0, 1);
    }

    // Criando as threads de leitura e de buscas
    pthread_create(threadsArgs->tid, NULL, leituraArquivo, (void *) threadsArgs);
    pthread_create((threadsArgs + 1)->tid, NULL, buscaPadraoA, (void *) (threadsArgs + 1));
    pthread_create((threadsArgs + 2)->tid, NULL, buscaPadraoB, (void *) (threadsArgs + 2));
    pthread_create((threadsArgs + 3)->tid, NULL, buscaPadraoC, (void *) (threadsArgs + 3));

    // Aguardar o termino das threads
    for(long int i = 0; i < 4; i++) {
        if(pthread_join(*(threadsArgs+i)->tid, NULL)) {
            fprintf(stderr, "ERRO--pthread_join\n");
            return 3;
        }
    }

    GET_TIME(endTime); // Finaliza a contagem
    printf("\nTempo: %lf\n", endTime - startTime);

    // Saída do problema com os resultados finais
    printf("\nMaior sequencia de valores identicos: %lld %lld %lld\n", padraoAPosInicial, padraoATamanho, padraoAValor);
    printf("Quantidade de triplas: %lld\n", padraoBQuantidade);
    printf("Quantidade de ocorrencias da sequencia <012345>: %lld\n", padraoCQuantidade);

    printf("\n");

    return 0;
}