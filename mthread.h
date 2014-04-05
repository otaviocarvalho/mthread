/*
 * mthread.h: arquivo de inclus�o com os prot�tipos das fun��es a serem
 *            implementadas na realiza��o do trabalho.
 *
 * N�O MODIFIQUE ESTE ARQUIVO.
 *
 * VERS�O 1 - 20/03/2014
 */
#ifndef __mthread__
#define __mthread__ 1

#include "mdata.h"   /* arquivo de inclus�o com as estruturas de dados definidas
                     * na implementa��o do trabalho. Deve incluir a estrutura TCB e a
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
