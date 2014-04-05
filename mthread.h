/*
 * mthread.h: arquivo de inclusão com os protótipos das funções a serem
 *            implementadas na realização do trabalho.
 *
 * NÃO MODIFIQUE ESTE ARQUIVO.
 *
 * VERSÃO 1 - 20/03/2014
 */
#ifndef __mthread__
#define __mthread__ 1

#include "mdata.h"   /* arquivo de inclusão com as estruturas de dados definidas
                     * na implementação do trabalho. Deve incluir a estrutura TCB e a
                     * aquela empregada para o mutex. Por exemplo:
                     *     typedef mutex {
                     *         int flag;
                     *         TCB *next;
                     *     } mmutex_t;
                     */

int mcreate (void (*start_routine)(void*), void *arg);
int myield(void);
int mjoin(int);
int mmutex_init(mmutex_t *);
int mlock (mmutex_t *);
int munlock (mmutex_t *);

#endif
