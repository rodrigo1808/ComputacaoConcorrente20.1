/* 
Modulo 1 - Trabalho de Implementacao
Programa: busca.c
Descricao: Implementa o problema da busca de um número em um vetor de inteiros
Autor: Rodrigo Araujo
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h> // Necessário para gerar numeros randomicos
#include "timer.h"

long long int numElementos; // Numero de elementos do vetor
int nthreads; // Numero de threads
unsigned int *vetor; // Vetor onde o número será buscado
unsigned int numToFind; // Numero que sera procurado no vetor
long long int numIndex; // Resultado do index em que o numero procurado se encontra
short int found = 0; // Variavel de controle que sinaliza quando o numero foi achado para outras threads pararem

void *find (void * arg) {
    long long int id = (long int) arg;
    
    long long int start, end;
    unsigned int i;
    long long int tamBloco = numElementos/nthreads; // Divisao para separacao dos blocos entre as threads
    
    // Calcula o intervalo de elementos para a busca na thread
    /* 
        A divisão dos blocos foi feita de forma balanceada, quando possui resto, esta sendo dividida igualmente para as primeiras threads
        Exemplo:    4 threads e vetor = [1, 4, 8, 9, 3, 2, 11, 16, 7, 6] (tamanho 10)
                    Thread 1 procura em [1, 4, 8] (tamanho 3)
                    Thread 2 procura em [9, 3, 2] (tamanho 3)
                    Thread 3 procura em [11, 16] (tamanho 2)
                    Thread 4 procura em [7, 6] (tamanho 2)
    */

    start = id * tamBloco;

    // Caso a divisao tenha resto divide igualmente para as primeiras threads, fazendo esse controle a partir do id da thread
    if(id < (numElementos%nthreads)) {
        tamBloco++;
    }

    // Caso a thread anterior tenha pego um dos elementos do resto, avanca o inicio da procura no valor de id que diz quantas posicoes foram puladas 
    if(id != 0 && (id-1) < (numElementos%nthreads)) {
        start+=id;
    }

    end = start + tamBloco; 

    // Busca de forma simpĺes iterando crescentemente pelo array
    for(i = start; i < end; i++) {
        if(found) break; // Caso outra thread tenha encontrado, sai do loop
        
        // Caso tenha encontrado o valor buscado
        if(vetor[i] == numToFind) {
            numIndex = i; // Altera o resultado pra posicao em que encontrou
            found = 1; // Atribui 1 a variavel de controle para sinalizar para outras threads terminarem
            break;
        }
    }

    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {

    pthread_t *tid;
    double startTime, endTime; // Variaveis de tempo
    time_t t; // Variavel para geracao de numeros aleatorios
    srand((unsigned) time(&t));

    if(argc < 4) {
        fprintf(stderr, "Digite: %s <numero para procurar no vetor> <dimensao do vetor> <numero de threads>\n", argv[0]);
        return 1;
    }

    numToFind = atoll(argv[1]);
    numElementos = atoll(argv[2]);
    nthreads = atoi(argv[3]);

    // Inicializa o vetor que guarda os numeros buscados
    vetor = (unsigned int *) malloc(sizeof(unsigned int)*numElementos);
    if(vetor == NULL) {
        fprintf(stderr, "ERRO--malloc\n");
        return 2;
    }

    // Preenche o vetor com valores aleatórios ou com o caso de teste
    for(long long int i = 0; i < numElementos; i++) {

        // Trecho de código sem forçar o pior caso, dessa forma não dá pra ter um controle do ganho de desempenho pois o valor pode estar qualquer posição ou em nenhuma.
        vetor[i] = rand() % (numElementos + 1) + 1;
        
        // Trecho de código para forçar o pior caso, colocando o número para ser encontrado apenas na última posição. 
        /*if(i == numElementos - 1) {
            vetor[i] = numToFind;
            printf("Entrou na ultima iteração de alocação de valores\n");
        } else {
            vetor[i] = rand() % (numElementos + 1) + 1;
            if(vetor[i] == numToFind) vetor[i] = numToFind + 1; // Se o rand gerar o numero procurado trocamos por outro
        }*/

        
    }

    // Inicio da concorrencia e da contagem de tempo
    GET_TIME(startTime);

    tid = (pthread_t *) malloc(sizeof(pthread_t) * nthreads);
    if(tid == NULL) {
        fprintf(stderr, "ERRO--malloc\n");
        return 2;
    }

    // Cria as threads passando o indexador como parametro
    for(long int i = 0; i < nthreads; i++) {
        if(pthread_create(tid + i, NULL, find, (void *) i)) {
            fprintf(stderr, "ERRO--pthread_create\n");
            return 3;
        }
    }

    // Aguardar o termino das threads
    for(long int i = 0; i < nthreads; i++) {
        if(pthread_join(*(tid+i), NULL)) {
            fprintf(stderr, "ERRO--pthread_join\n");
            return 3;
        }
    }

    GET_TIME(endTime); // Fim da contagem

    printf("Tempo: %lf\n", endTime - startTime);
    
    // Exibe o resultado dependendo se foi encontrado ou nao
    if(found) {
        printf("O número %d foi encontrado na posição %lld do vetor\n", numToFind, numIndex);
    } else {
        printf("O número não foi encontrado\n");
    }
    
    // Libera a memoria alocada
    free(vetor);
    free(tid);

    return 0;
}