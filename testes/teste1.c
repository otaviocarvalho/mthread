/*
 * Teste da função diff, que usa a biblioteca lrt para chamar a função clock_gettime() e calcular o tempo de execução das threads
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "../include/mthread.h"

int main(int argc, char *argv[]) {
    printf("\nTeste 1: Testa a função que calcula o tempo de execução de uma thread\n");
    timespec_test();
    printf("\n");
    return 0;
}

