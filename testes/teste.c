
/*
 * Programa exemplo de teste da biblioteca microthreads
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "../include/mthread.h"

void func0(void *arg) {

     /*corpo da fun��o func0 */

     return;    /*termina a execu��o da thread via comando C */
}

void func1(void *arg) {

     /*corpo da fun��o func1 */

}    /* termina a execu��o da thread ao chegar ao final da fun��o naturalmente */

int main(int argc, char *argv[]) {
    int id0, id1;
    int i;

    printf("\nTeste 0: Teste padr�o fornecido para o trabalho\n");

	/* C�digo de execu��o */

    id0 = mcreate(func0, (void *)&i);
    id1 = mcreate(func1, (void *)&i);

    printf("Teste id0: %d\n", id0);
    printf("Teste id1: %d\n", id1);

	/* C�digo de execu��o */

    mjoin(id0);
    mjoin(id1);

    printf("\n");

    return 0;
}

