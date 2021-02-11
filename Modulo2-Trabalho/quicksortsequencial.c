#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include "timer.h"

//int array[] = {4, 2, 8, 1, 6, 5, 10, 9, 3, 7};
int *array;
int numElementos;
int num = 0;

int partition(int start, int end) {
    //printf("Partition ta cuidando do intervalo %d - %d\n", start, end);
    int pivot = end;
    //printf("pivot = %d (pos %d)\n", array[pivot], pivot);
    int i = start - 1; // Posição do menor elemento
    int j;
    int aux;

    for(j = start; j <= end - 1; j++) {
        if(array[j] < array[pivot]) {
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

    return i + 1;
}

void quicksort(int start, int end) {
    if(start < end) {
        int pivot = partition(start, end);

        quicksort(start, pivot - 1);
        quicksort(pivot + 1, end);
    }

    num++;
}

int main(int argc, char *argv[]) {

    double inicio, fim; 
    time_t t;
    srand((unsigned) time(&t));

    if(argc < 2) {
        fprintf(stderr, "Digite: %s <dimensao do vetor>\n", argv[0]);
        return 1;
    }

    numElementos = atoll(argv[1]);

    array = (int *) malloc(sizeof(int) * numElementos);
    if(array == NULL) {
        fprintf(stderr, "ERRO--malloc\n");
        return 2;
    }

    for(long long int i = 0; i < numElementos; i++) {
        array[i] = rand() % (numElementos + 1) + 1;
    }

    for(int i = 0; i < numElementos; i++) {
       printf("%d ", array[i]);
    }

    GET_TIME(inicio);

    quicksort(0, numElementos - 1);

    GET_TIME(fim);

    for(int i = 0; i < numElementos; i++) {
       printf("%d ", array[i]);
    }

    printf("foi pra fora do if %d vezes", num);

    printf("\nTempo: %lf\n", fim - inicio);

    free(array);

    return 0;
}