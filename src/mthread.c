#include <stdio.h>
#include <stdlib.h>
#include "../include/mthread.h"
#include <time.h>

#define MTHREAD_STACK 1024*1024
#define READY 0
#define RUNNING 1
#define BLOCKED 2

int mthread_initialized = 0;
int num_threads = 0;
int num_tid = 1;

tcb_t *running_thread = NULL;
tcb_priority_queue_t *ready_queue = NULL;
tcb_list_t *all_threads = NULL;
tcb_list_t *blocked = NULL;
tcb_list_t *threads_list = NULL;

ucontext_t *thread_finished; // Contexto a ser chamado quando uma thread termina

int tcb_priority_queue_add(tcb_t *data, tcb_priority_queue_t *queue){
    tcb_priority_queue_t *n = (tcb_priority_queue_t *) malloc(sizeof(tcb_priority_queue_t));

    if (n != NULL){
        n->data = data;
        n->next = NULL;

        if (queue == NULL){
            queue = n;
        } else {
            queue->next = n;
        }

        return 1;
    }

    return 0;
}

tcb_t* tcb_priority_queue_remove(tcb_priority_queue_t *q){
    tcb_t *result;

    if (q == NULL){
        return NULL;
    }
    else if (q->next == NULL){
        result = q->data;
        q = NULL;
    }
    else {
        result = q->data;
        q = q->next;
    }

    return result;
}

tcb_priority_queue_t* tcb_priority_queue_init(){
    tcb_priority_queue_t *q = (tcb_priority_queue_t *) malloc(sizeof(tcb_priority_queue_t));

    if (q != NULL){
        q->data = NULL;
        q->next = NULL;
    }

    return q;
}

int tcb_list_add(tcb_list_t *list, tcb_t *thread){
    tcb_list_t *node = (tcb_list_t *) malloc(sizeof(tcb_list_t));

    if (node == NULL){
        return -1;
    }
    else if (list->data == NULL){ // Adiciona primeiro elemento
        list->data = thread;
        list->prev = NULL;
        list->next = NULL;

        free(node);
        return 0;
    }
    else {
        node->data = thread;
        tcb_list_t *aux = list;

        // Encontra o último elemento da lista
        while (aux->next != NULL){
            aux = aux->next;
        }

        aux->next = node;
        node->prev = aux;
        node->next = NULL;

        return 1;
    }
}

tcb_t* tcb_list_remove(tcb_list_t *list, tcb_t *thread){
    tcb_t *remove;
    tcb_list_t *aux = list;

    while (aux != NULL){
        // Teste de segurança para garantir acesso somente à area de memória alocada
        if (aux->data == NULL){
            return NULL;
        }

        if ( ((aux->data)->tid) == thread->tid ){
            if (aux->prev == NULL && aux->next == NULL){ // Remove único elemento
                aux->data = NULL;
           }
            else if (aux->prev == NULL){ // Remove o primeiro elemento de uma lista não vazia

                list->data = (aux->next)->data;
                list->prev = NULL;
                list->next = (aux->next)->next;
            }
            else if (aux->next == NULL){ // Remove o último elemento
                (aux->prev)->next = NULL;
            }
            else { // Remove no meio da lista
                (aux->prev)->next = aux->next;
                (aux->next)->prev = aux->prev;
            }

            remove = aux->data;
            free(remove);
            /*free(aux);*/

            return remove;
        }

        aux = aux->next;
    }

    return NULL;
}

void tcb_list_print(tcb_list_t* list){
    printf("[");
    if (list != NULL){ // Verifica se existe algum elemento válido na lista
        if (list->data != NULL){
            tcb_list_t* aux = list;

            while (aux != NULL){
                printf("%d | ", aux->data->tid);
                aux = aux->next;
            }
        }
    }
    printf("]\n");
}

tcb_list_t* tcb_list_create(){
    tcb_list_t *list = (tcb_list_t *) malloc(sizeof(tcb_list_t));

    if (list != NULL){
        list->data = NULL;
        list->prev = NULL;
        list->next = NULL;
    }

    return list;
}

tcb_t* tcb_create(int tid, int status, ucontext_t* context){
    tcb_t *thread = (tcb_t *) malloc(sizeof(tcb_t));

    if (thread != NULL){
        thread->tid = tid;
        thread->status = status;
        thread->context = context;
        thread->waiting = tcb_list_create();
    }

    return thread;
}

tcb_t* schedule(){
    tcb_t *scheduled = tcb_priority_queue_remove(ready_queue);

    if (scheduled == NULL){
        return NULL;
    }

    running_thread = scheduled;
    scheduled->status = RUNNING;

    return scheduled;
}

int dispatch_next(){
    tcb_t *scheduled_thread = schedule();

    if (scheduled_thread == NULL){
        return -1;
    }
    else {
        // Thread é colocada em execução (Dispatch)
        setcontext(scheduled_thread->context);
        return 0;
    }
}

int add_ready(tcb_t *thread){
    if (tcb_priority_queue_add(thread, ready_queue)){
        if (thread->status == BLOCKED){
            tcb_list_remove(blocked, thread);
        }

        thread->status = READY;
        return 0;
    }

    return -1;
}

int create_thread(ucontext_t* context){
    int new_tid = num_tid;
    tcb_t *thread = tcb_create(new_tid, READY, context);

    if (thread == NULL){
        return -1;
    }

    if (add_ready(thread) < 0){
        return -2;
    }

    if (tcb_list_add(all_threads, thread) < 0){
        return -3;
    }

    num_threads++;
    num_tid++;

    return new_tid;
}

void thread_finish(){
    /*murder(running_thread);*/
    tcb_list_t *waiting_list = running_thread->waiting;

    while (waiting_list != NULL){
        tcb_t *pending_thread = (tcb_t *) waiting_list->data;
        tcb_list_remove(running_thread->waiting, pending_thread);
        pending_thread->status = READY;
        add_ready(pending_thread);

        waiting_list = waiting_list->next;
    }

    num_threads--;
    free(running_thread->waiting);
    free(running_thread);
    dispatch_next();
}

int mthread_init(){
    int dispatched_main = 0;

    // Aloca o contexto principal
    ucontext_t *main_context = (ucontext_t*) malloc( sizeof(ucontext_t) );
    if (main_context == NULL) {
        return 1;
    }

    // Inicializa o escalonador
    ready_queue = tcb_priority_queue_init();
    if(ready_queue == NULL) {
        return -1;
    }

    blocked = tcb_list_create();
    if(blocked == NULL) {
       return -2;
    }

    all_threads = tcb_list_create();
    if(all_threads == NULL) {
        return -3;
    }

    num_threads = 0;
    num_tid = 0;

    // Cria o contexto que trata o fim de uma thread
    thread_finished = (ucontext_t *) malloc(sizeof(ucontext_t));

    if (thread_finished != NULL){
        getcontext(thread_finished);
        /*thread_finished->uc_stack.ss_sp = mmap(NULL,STACKSIZE,PROT_WRITE|PROT_READ,MAP_PRIVATE|MAP_GROWSDOWN|MAP_ANONYMOUS,-1,0);*/
        thread_finished->uc_stack.ss_sp = malloc(SIGSTKSZ);
        thread_finished->uc_stack.ss_size = SIGSTKSZ;
        thread_finished->uc_link = NULL;

        // Testa se alocou a stack adequadamente
        if (thread_finished->uc_stack.ss_sp == NULL){
            return -5;
        }

        makecontext(thread_finished, thread_finish, 0); // Chama a função thread_finish() ao terminar o contexto
    }
    else {
        return -4;
    }

    if (!dispatched_main){
        dispatched_main = 1;

        if (getcontext(main_context) < 0){
            return -6;
        }

        if (create_thread(main_context) < 0){
            return -7;
        }

        if (dispatch_next() < 0){
            return -8;
        }
    }

    return 0;
}

struct timespec time_diff(struct timespec start, struct timespec end){
	struct timespec temp;

	if ((end.tv_nsec-start.tv_nsec)<0) {
		temp.tv_sec = end.tv_sec-start.tv_sec-1;
		temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
	}
    else {
		temp.tv_sec = end.tv_sec-start.tv_sec;
		temp.tv_nsec = end.tv_nsec-start.tv_nsec;
	}

	return temp;
}

void timespec_test(){
    struct timespec time1, time2;
    int i, temp;

	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &time1);
	for (i = 0; i< 242000000; i++)
		temp+=temp;
	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &time2);

    printf("Thread execution time in seconds: %lf\n", (double) time_diff(time1,time2).tv_sec);
    printf("Thread execution time in nanoseconds: %lf\n", (double) time_diff(time1,time2).tv_nsec);
}


int mcreate (void (*start_routine)(void*), void *arg){
    return 0;
}
int myield(void){
    return 0;
}

int mjoin(int tid){
    return 0;
}

int mmutex_init(mmutex_t *m){
    return 0;
}

int mlock (mmutex_t *m){
    return 0;
}

int munlock (mmutex_t *m){
    return 0;
}

/*// Testa a lista*/
/*int main(){*/
    /*tcb_t *aux = tcb_create(1, 0, NULL);*/
    /*tcb_t *aux2 = tcb_create(2, 0, NULL);*/
    /*tcb_t *aux3 = tcb_create(3, 0, NULL);*/
    /*tcb_t *aux4 = tcb_create(4, 0, NULL);*/

    /*tcb_list_t *list = tcb_list_create();*/

    /*[>// Testa inicio<]*/
    /*[>tcb_list_print(list);<]*/
    /*[>tcb_list_remove(list, aux);<]*/
    /*[>tcb_list_remove(list, aux);<]*/
    /*[>tcb_list_print(list);<]*/
    /*[>tcb_list_add(list, aux);<]*/
    /*[>tcb_list_print(list);<]*/
    /*[>tcb_list_remove(list, aux);<]*/
    /*[>tcb_list_remove(list, aux); // SegFault<]*/
    /*[>tcb_list_print(list);<]*/
    /*[>aux = tcb_create(1,0,NULL);<]*/
    /*[>tcb_list_add(list, aux);<]*/
    /*[>tcb_list_print(list);<]*/

    /*[>tcb_list_add(list, aux);<]*/
    /*[>tcb_list_add(list, aux2);<]*/
    /*[>tcb_list_add(list, aux3);<]*/
    /*[>tcb_list_remove(list, aux3); // meio<]*/
    /*[>tcb_list_remove(list, aux2); // meio<]*/
    /*[>tcb_list_remove(list, aux);<]*/
    /*[>tcb_list_add(list, aux4);<]*/
    /*[>tcb_list_remove(list, aux4);<]*/
    /*[>tcb_list_print(list);<]*/

    /*[>// Teste simples<]*/
    /*[>tcb_list_add(list, aux);<]*/
    /*[>tcb_list_add(list, aux2);<]*/
    /*[>tcb_list_add(list, aux3);<]*/
    /*[>tcb_list_add(list, aux4);<]*/
    /*[>tcb_list_print(list);<]*/
    /*[>tcb_list_remove(list, aux4); // fim<]*/
    /*[>tcb_list_print(list);<]*/
    /*[>tcb_list_print(list);<]*/
    /*[>tcb_list_remove(list, aux); // inicio<]*/
    /*[>tcb_list_print(list);<]*/
    /*return 0;*/
/*}*/
