#include <stdio.h>
#include <stdlib.h>
#include "../include/mthread.h"
#include <time.h>

#define MTHREAD_STACK 1024*1024
#define READY 0
#define RUNNING 1
#define BLOCKED 2

int initialized_mthread = 0;
int num_threads = 0;
int num_tid = 0;

tcb_t *running_thread = NULL;
tcb_priority_queue_t *ready_queue = NULL;
tcb_list_t *all_threads = NULL;
tcb_list_t *blocked = NULL;
tcb_list_t *threads_list = NULL;

ucontext_t *thread_finished; // Contexto a ser chamado quando uma thread termina

int tcb_priority_queue_add(tcb_priority_queue_t *queue, tcb_t *data){
    tcb_priority_queue_t *n = (tcb_priority_queue_t *) malloc(sizeof(tcb_priority_queue_t));

    if (n != NULL){
        if (queue->data == NULL){ // Insere primeiro elemento
            queue->front = queue;
            queue->back = queue;
            queue->data = data;
            queue->next = NULL;
            queue->prev = NULL;

            free(n);
        }
        else { // Insere demais elementos
            n->data = data;
            n->next = NULL;
            (queue->back)->next = n;
            queue->back = n;
        }

        return 1;
    }

    return 0;
}

tcb_t* tcb_priority_queue_remove(tcb_priority_queue_t *q){
    tcb_t *removed_data;
    tcb_priority_queue_t *remove;

    if (q == NULL){
        return NULL;
    }
    else if (q->next == NULL){ // Único elemento
        removed_data = q->data;
        q->data = NULL;
        q->prev = NULL;
        q->next = NULL;
        q->back = NULL;
        q->front = NULL;
    }
    else { // Mais de um elemento
        removed_data = q->data;
        remove = q->next;

        q->front = q->next;
        q->data = (q->next)->data;
        q->prev = NULL;
        q->next = (q->next)->next;

        free(remove);
    }

    return removed_data;
}

void tcb_priority_queue_print(tcb_priority_queue_t* q){
    printf("[");
    if (q != NULL){ // Verifica se existe algum elemento válido na fila
        if (q->data != NULL){
            tcb_priority_queue_t* aux = q;

            while (aux != NULL){
                printf("%d | ", aux->data->tid);
                aux = aux->next;
            }
        }
    }
    printf("]\n");
}

tcb_priority_queue_t* tcb_priority_queue_init(){
    tcb_priority_queue_t *q = (tcb_priority_queue_t *) malloc(sizeof(tcb_priority_queue_t));

    if (q != NULL){
        q->data = NULL;
        q->next = NULL;
        q->prev = NULL;
        q->front = NULL;
        q->back = NULL;
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
    }

    return 0;
}

tcb_t* tcb_list_remove(tcb_list_t *list, tcb_t *thread){
    tcb_t *removed_data;
    tcb_list_t *remove;
    tcb_list_t *aux = list;

    while (aux != NULL){
        // Teste de segurança para garantir acesso somente à area de memória alocada
        if (aux->data == NULL){
            return NULL;
        }

        if ( ((aux->data)->tid) == thread->tid ){
            if (aux->prev == NULL && aux->next == NULL){ // Remove único elemento
                remove = aux;
                removed_data = remove->data;
                aux->data = NULL;
            }
            else if (aux->prev == NULL){ // Remove o primeiro elemento de uma lista não vazia
                list->data = (aux->next)->data;
                list->prev = NULL;
                list->next = (aux->next)->next;
                remove = aux->next;
                removed_data = remove->data;
                free(remove);
            }
            else if (aux->next == NULL){ // Remove o último elemento
                (aux->prev)->next = NULL;
                remove = aux;
                removed_data = remove->data;
                free(remove);
            }
            else { // Remove no meio da lista
                (aux->prev)->next = aux->next;
                (aux->next)->prev = aux->prev;
                remove = aux;
                removed_data = remove->data;
                free(remove);
            }

            return removed_data;
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

void dispatch_next(){
    tcb_t *scheduled_thread = schedule();
    /*printf("scheduled_thread tid %d\n", scheduled_thread->tid);*/

    if (scheduled_thread != NULL){
        // Thread é colocada em execução (Dispatch)
        setcontext(scheduled_thread->context);
    }

    /*if (scheduled_thread == NULL){*/
        /*return -1;*/
    /*}*/
    /*else {*/
        /*// Thread é colocada em execução (Dispatch)*/
        /*setcontext(scheduled_thread->context);*/
        /*return 0;*/
    /*}*/
}

int add_ready(tcb_t *thread){
    if (tcb_priority_queue_add(ready_queue, thread)){
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
    /*printf("thread_finish context\n");*/
    /*exit(-1);*/
    /*murder(running_thread);*/
    tcb_list_t *waiting_list = running_thread->waiting;

    while (waiting_list != NULL){
        /*tcb_t *pending_thread = (tcb_t *) waiting_list->data;*/
        /*tcb_list_remove(running_thread->waiting, pending_thread); // Erro no free() da lista*/
        /*pending_thread->status = READY;*/
        /*add_ready(pending_thread);*/

        waiting_list = waiting_list->next;
    }

    num_threads--;
    free(running_thread->waiting);
    free(running_thread);

    dispatch_next();
}

int mthread_init(){
    int dispatched_main = 0;
    int result_main_context = 0;

    // Sinaliza que inicializou a biblioteca
    initialized_mthread = 1;

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

        makecontext(thread_finished, (void (*)(void)) thread_finish, 0); // Chama a função thread_finish() ao terminar o contexto
        /*setcontext(thread_finished);*/
    }
    else {
        return -4;
    }

    // Salva o contexto atual antes de fazer o dispatch da main
    result_main_context = getcontext(main_context);

    if (!dispatched_main){
        dispatched_main = 1;
        if (result_main_context < 0){
            return -6;
        }

        if (create_thread(main_context) < 0){
            return -8;
        }

        dispatch_next();
        /*if (dispatch_next() < 0){*/
            /*return -9;*/
        /*}*/
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


int mcreate(void (*start_routine)(void*), void *arg){
    ucontext_t *thr_context = (ucontext_t *) malloc(sizeof(ucontext_t));
    int tid;

    if (!initialized_mthread){
        mthread_init();
    }

    if (thr_context != NULL){
        getcontext(thr_context);

        thr_context->uc_stack.ss_sp = malloc(SIGSTKSZ);
        thr_context->uc_stack.ss_size = SIGSTKSZ;
        thr_context->uc_link = thread_finished;

        // Testa se alocou a stack adequadamente
        if (thr_context->uc_stack.ss_sp == NULL){
            return -2;
        }

        makecontext(thr_context, (void (*)(void)) start_routine, 1, arg);
    }
    else {
        return -1;
    }

    tid = create_thread(thr_context);

    return tid;
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

/*// Testa mthread_create*/
/*void thread0(void *arg) {*/
    /*printf("Entrou na thread0\n");*/
    /*return;*/
/*}*/
/*int main(){*/
    /*int tid = -1;*/
    /*int i;*/

    /*tid = mcreate(thread0, (void *)&i);*/
    /*tid = mcreate(thread0, (void *)&i);*/
    /*printf("tid %d\n", tid);*/
    /*dispatch_next();*/
    /*tcb_priority_queue_print(ready_queue);*/
    /*return 0;*/
/*}*/

/*// Testa a queue*/
/*int main(){*/
    /*tcb_t *aux = tcb_create(1, 0, NULL);*/
    /*tcb_t *aux2 = tcb_create(2, 0, NULL);*/
    /*tcb_t *aux3 = tcb_create(3, 0, NULL);*/
    /*tcb_t *aux4 = tcb_create(4, 0, NULL);*/

    /*tcb_priority_queue_t *q = tcb_priority_queue_init();*/

    /*// Testa inicio*/
    /*tcb_priority_queue_print(q);*/
    /*tcb_priority_queue_remove(q);*/
    /*tcb_priority_queue_remove(q);*/
    /*tcb_priority_queue_print(q);*/
    /*tcb_priority_queue_add(q,aux);*/
    /*tcb_priority_queue_add(q,aux2);*/
    /*tcb_priority_queue_add(q,aux3);*/
    /*tcb_priority_queue_print(q);*/
    /*tcb_priority_queue_remove(q);*/
    /*tcb_priority_queue_remove(q);*/
    /*tcb_priority_queue_remove(q);*/
    /*tcb_priority_queue_remove(q);*/
    /*tcb_priority_queue_print(q);*/
/*}*/

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
