/*
 * mdata.h: arquivo de inclusão onde os grupos devem colocar as definições
 *          de suas estruturas de dados
 *
 * VERSÃO 1 - 20/03/2014
 */

/*
 * Exemplo de estrutura de dados "mutex"
 * Os grupos devem alterar essa estrutura de acordo com sua necessidade
 */
#include <ucontext.h>
#include <time.h>

typedef struct tcb_list {
    struct tcb *data;
    struct tcb_list *prev;
    struct tcb_list *next;
} tcb_list_t;

typedef struct tcb {
    int tid;
    int status;
    double forecast;
    int waiting_flag;
    ucontext_t *context;
    tcb_list_t *waiting;
} tcb_t;

typedef struct tcb_priority_queue {
    tcb_t *data;
    struct tcb_priority_queue *prev;
    struct tcb_priority_queue *next;
} tcb_priority_queue_t;

typedef struct mmutex_t {
    int locked;
    tcb_list_t *waiting;
} mmutex_t;

