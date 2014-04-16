/*
 * mdata.h: arquivo de inclus�o onde os grupos devem colocar as defini��es
 *          de suas estruturas de dados
 *
 * VERS�O 1 - 20/03/2014
 */

/*
 * Exemplo de estrutura de dados "mutex"
 * Os grupos devem alterar essa estrutura de acordo com sua necessidade
 */
typedef struct mutex {
	int flag;
    //TCB *next;
} mmutex_t;

typedef struct {
    int tid;
    int status;
    ucontext_t *context;
    tcb_list_t *waiting;
} tcb_t;
