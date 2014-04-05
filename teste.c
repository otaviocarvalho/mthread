
/*
 * Programa exemplo de teste da biblioteca microthreads
 *
 * Versão 1 - 20/03/2014
 */

#include "./include/mthread.h" /*considera que este fonte está no diretório “testes” (vide seção 8)*/

void func0(void *arg) {

     /*corpo da função func0 */
	 
     return;    /*termina a execução da thread via comando C */
}

void func1(void *arg) {

     /*corpo da função func1 */
	 
}    /* termina a execução da thread ao chegar ao final da função naturalmente */

int main(int argc, char *argv[]) {
    int	id0, id1;
    int 	i;
    
	/* Código de execução */
	
    id0 = mcreate(func0, (void *)&i);
    id1 = mcreate(func1, (void *)&i);
	
	/* Código de execução */
	
    mjoin(id0);
    mjoin(id1);
}

