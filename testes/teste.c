
/*
 * Programa exemplo de teste da biblioteca microthreads
 *
 * Vers�o 1 - 20/03/2014
 */

#include "../include/mthread.h" /*considera que este fonte est� no diret�rio �testes� (vide se��o 8)*/
#include <stdio.h>
#include <stdlib.h>

void func0(void *arg) {

     /*corpo da fun��o func0 */

     return;    /*termina a execu��o da thread via comando C */
}

void func1(void *arg) {

     /*corpo da fun��o func1 */

}    /* termina a execu��o da thread ao chegar ao final da fun��o naturalmente */

int main(int argc, char *argv[]) {
    /*int id0, id1;*/
    /*int i;*/

    printf("Test initialization was successful!\n");

	/* C�digo de execu��o */

    /*id0 = mcreate(func0, (void *)&i);*/
    /*id1 = mcreate(func1, (void *)&i);*/

	/* C�digo de execu��o */

    /*mjoin(id0);*/
    /*mjoin(id1);*/

    return 0;
}

