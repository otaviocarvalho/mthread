#include <stdio.h>
#include <stdlib.h>
#include "../include/mthread.h"

int id0, id1;

void thread0(void *arg) {
    printf("Entrou na thread0\n");
    return;
}

void thread1(void *arg) {
    printf("Entrou na thread\n");
    mjoin(id0);
    return;
}

int main(int argc, char *argv[]) {
    int i;

    printf("\nTeste 2: Testa a mjoin com duas chamadas para o mesmo tid\n");

	/* Código de execução */

    id0 = mcreate(thread0, (void *)&i);
    id1 = mcreate(thread1, (void *)&i);

    printf("Teste id0: %d\n", id0);
    printf("Teste id1: %d\n", id1);

	/* Código de execução */

    mjoin(id0);
    mjoin(id1);

    printf("\n");

    return 0;
}

