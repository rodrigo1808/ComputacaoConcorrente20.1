#include <stdio.h>
#include <time.h> // Necessário para gerar numeros randomicos
#include <stdlib.h>

int main(int argc, char* argv[]) {

    long long int bufferSize; // Tamanho da sequência
    long long int sequencia[] = { 1, 2, 3, 1, 1, 1, 1, 2, 3, 4, 5, 5, 5, 5, 5, 5, 5, 0, 0, 0, 3, 3, 3, 0, 1, 2, 3, 4, 5, 0 };
    FILE *ptr;
    time_t t; // Variavel para geracao de numeros aleatorios
    srand((unsigned) time(&t));

    if(argc < 2) {
        fprintf(stderr, "Digite: %s <tamanho do buffer (0 para o buffer exemplo do trabalho)>\n", argv[0]);
        return 1;
    }

    bufferSize = atoll(argv[1]);

    ptr = fopen("sequencia.bin","wb");

    // Caso opte pela entrada de exemplo
    if(bufferSize == 0) {
        bufferSize = 30;
        fwrite(&bufferSize, sizeof(long long int), 1, ptr);
        for (int i = 0; i < 31; i++){
            fwrite(&sequencia[i], sizeof(long long int), 1, ptr);
        }

        fclose(ptr);

        return 0;
    }

    long long int buffer[bufferSize + 1];

    printf("Gerando o arquivo binario...\n");

    buffer[0] = bufferSize;

    for(int i = 1; i < bufferSize + 1; i++) {
        buffer[i] = rand() % 6 ; // Gerando valores aleatorios de 0 a 5
    }

    fwrite(buffer,sizeof(buffer),1,ptr);

    printf("Lendo a sequencia de numeros do arquivo...\n");

    for(int i = 0; i < bufferSize + 1; i++)
        printf("%lld ", buffer[i]); // Mostrando a sequencia randomica gerada

    fclose(ptr);

    printf("\n");

    return 0;
}