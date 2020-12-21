#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include "timer.h"

int main(int argc, char *argv[]) {

    double somaSerie = 0; // Guarda a soma dos elementos da serie
    double fator; // Guarda o elemento a se somar a cada serie
    int sinal = 1; // Guarda o sinal do elemento
    long long int numElementos; // Numero de elementos a se somar
    double inicio, fim; 

    if(argc < 2) {
        fprintf(stderr, "Digite: %s <numero de elementos>\n", argv[0]);
        return 1;
    }

    numElementos = atoll(argv[1]);

    GET_TIME(inicio);
    // Soma os elementos
    for(long long int i = 0; i < numElementos; i++) {
        sinal = pow(-1, i);
        fator = sinal * (4.0/((2*i) + 1));
        somaSerie += fator;
    }
    GET_TIME(fim);
    printf("Tempo sequencial: %lf\n", fim - inicio);

    printf("SOMA DA SERIE: %.15lf\n", somaSerie);
    printf("MATH_PI: %.15lf\n", M_PI);

    printf("DIFERENCA: %.15lf\n", M_PI - (somaSerie));

    return 0;
}