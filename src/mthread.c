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
            tcb_priority_queue_t *aux = queue;
            while (queue->next != NULL)
                aux = aux->next;
            /*printf("aux %d\n", aux->data->tid);*/
            aux->next = n;
            queue->back = n;
            /*(queue->back)->next = n;*/
            /*queue->back = n;*/

            /*printf("queue %d\n", (ready_queue->next)->data->tid);*/
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
        thread->waiting_flag = 0;
    }

    return thread;
}

tcb_t* tcb_list_get_thread(tcb_list_t* list, int tid){
    if (list != NULL){
        tcb_list_t* aux = list;

        while (aux != NULL){
            if ((aux->data)->tid == tid){
                return aux->data;
            }

            aux = aux->next;
        }
    }

    return NULL;
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
    return 0;
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
    printf("thread_finish context\n");
    /*exit(-1);*/
    /*murder(running_thread);*/
    tcb_list_t *waiting_list = running_thread->waiting;

    while (waiting_list != NULL){
        if (waiting_list->data != NULL){
            printf("entrou pending thread\n");
            tcb_t *pending_thread = (tcb_t *) waiting_list->data;
            tcb_list_remove(running_thread->waiting, pending_thread); // Erro no free() da lista
            pending_thread->status = READY;
            add_ready(pending_thread);

            waiting_list = waiting_list->next;
        }
        else {
            waiting_list = NULL;
        }
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
            return -1;
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
    int yield = 0;
    getcontext(running_thread->context);

    if (!yield){
        yield = 1;
        add_ready(running_thread);
        dispatch_next();
    }

    printf("voltou myield\n");
    return 0;
}

int mjoin(int tid){
    printf("mjoin thread tid %d\n", tid);
    tcb_t *thr_wait = tcb_list_get_thread(all_threads, tid);

    if (thr_wait == NULL){
        return -1;
    }

    if (thr_wait->waiting_flag == 0){
        tcb_list_add(thr_wait->waiting, running_thread);
        thr_wait->waiting_flag = 1;
    }
    else {
        printf("segunda thread querendo fazendo join() no mesmo tid\n");
        return -1;
    }

    running_thread->status = BLOCKED;
    tcb_list_add(blocked, running_thread);

    getcontext(running_thread->context);
    if (running_thread->status == BLOCKED){
        dispatch_next();
    }

    return 0;
}

int mmutex_init(mmutex_t *m){
    m->locked = 0;
    m->waiting = tcb_list_create();
    return 0;
}

int mlock (mmutex_t *m){
    if (m->locked == 0){
        m->locked = 1;
        printf("lockou\n");
    }
    else {

        printf("here locked\n");
        tcb_list_add(m->waiting, running_thread);

        running_thread->status = BLOCKED;
        tcb_list_add(blocked, running_thread);

        getcontext(running_thread->context);

        dispatch_next();
    }

    return 0;
}

int munlock (mmutex_t *m){
    m->locked = 0;

    if (m->waiting != NULL){
        tcb_t *thread_waiting = tcb_list_remove(m->waiting, (m->waiting)->data);
        if (thread_waiting != NULL){
            /*add_ready(thread_waiting);*/
            thread_waiting->status = READY;
        }
    }

    return 0;
}

/*// Testa mlock*/
/*mmutex_t mutex;*/
/*int test = 0;*/
/*void thread0(void *arg) {*/
    /*printf("Entrou na thread0\n");*/
    /*mlock(&mutex);*/
    /*printf("bloqueou thread\n");*/
    /*test++;*/
    /*munlock(&mutex);*/
    /*printf("desbloqueou thread\n");*/
    /*return;*/
/*}*/
/*int main(){*/
    /*int tid = -1;*/
    /*int i;*/

    /*mmutex_init(&mutex);*/
    /*tid = mcreate(thread0, (void *)&i);*/
    /*printf("tid %d\n", tid);*/

    /*mlock(&mutex);*/
    /*printf("bloqueou thread main\n");*/
    /*test++;*/
    /*printf("%d\n", test);*/
    /*mjoin(1);*/
    /*munlock(&mutex);*/
    /*printf("desbloqueou thread main\n");*/

    /*printf("entrou\n");*/
    /*printf("acabou\n");*/
    /*tcb_priority_queue_print(ready_queue);*/
    /*return 0;*/
/*}*/

/*// Testa myield*/
/*void thread2(void *arg) {*/
    /*printf("Entrou na thread2\n");*/
    /*myield();*/
    /*printf("Voltou na thread2\n");*/
    /*return;*/
/*}*/

/*void thread1(void *arg) {*/
    /*printf("initjoin\n");*/
    /*mjoin(3);*/
    /*printf("endjoin\n");*/
    /*printf("Entrou na thread1\n");*/
    /*return;*/
/*}*/
/*void thread0(void *arg) {*/
    /*int i;*/
    /*mcreate(thread2, (void *)&i);*/
    /*printf("Entrou na thread0\n");*/
    /*[>myield();<]*/
    /*[>printf("Voltou na thread0\n");<]*/
    /*return;*/
/*}*/
/*int main(){*/
    /*int tid = -1;*/
    /*int i;*/

    /*[>tid = mcreate(thread1, (void *)&i);<]*/
    /*[>printf("tid %d\n", tid);<]*/
    /*[>tid = mcreate(thread0, (void *)&i);<]*/
    /*[>printf("tid %d\n", tid);<]*/
    /*tid = mcreate(thread0, (void *)&i);*/
    /*printf("tid %d\n", tid);*/
    /*[>dispatch_next();<]*/

    /*printf("entrou\n");*/
    /*mjoin(1);*/
    /*mjoin(2);*/
    /*printf("acabou\n");*/
    /*tcb_priority_queue_print(ready_queue);*/
    /*return 0;*/
/*}*/

/*// Testa mthread_join e mthread_create*/
/*void thread2(void *arg) {*/
    /*printf("Entrou na thread2\n");*/
    /*return;*/
/*}*/

/*void thread1(void *arg) {*/
    /*printf("initjoin\n");*/
    /*mjoin(3);*/
    /*printf("endjoin\n");*/
    /*printf("Entrou na thread1\n");*/
    /*return;*/
/*}*/
/*void thread0(void *arg) {*/
    /*int i;*/
    /*mcreate(thread2, (void *)&i);*/
    /*printf("Entrou na thread0\n");*/
    /*return;*/
/*}*/
/*int main(){*/
    /*int tid = -1;*/
    /*int i;*/

    /*tid = mcreate(thread1, (void *)&i);*/
    /*printf("tid %d\n", tid);*/
    /*tid = mcreate(thread0, (void *)&i);*/
    /*printf("tid %d\n", tid);*/
    /*tid = mcreate(thread0, (void *)&i);*/
    /*printf("tid %d\n", tid);*/
    /*[>dispatch_next();<]*/
    /*mjoin(1);*/
    /*printf("acabou\n");*/
    /*return 0;*/
/*}*/

/*// Testa mthread_join*/
/*void thread1(void *arg) {*/
    /*printf("Entrou na thread1\n");*/
    /*return;*/
/*}*/
/*void thread0(void *arg) {*/
    /*printf("Entrou na thread0\n");*/
    /*int tid = -1;*/
    /*[>tid = mcreate(thread1, (void *)&tid);<]*/
    /*mjoin(1);*/
    /*return;*/
/*}*/
/*int main(){*/
    /*int tid = -1;*/
    /*int i;*/

    /*tid = mcreate(thread0, (void *)&i);*/
    /*printf("tid %d\n", tid);*/
    /*mjoin(1);*/
    /*mjoin(1);*/
    /*printf("acabou\n");*/
    /*return 0;*/
/*}*/

/*// Testa mthread_create*/
/*void thread1(void *arg) {*/
    /*printf("Entrou na thread1\n");*/
    /*return;*/
/*}*/
/*void thread0(void *arg) {*/
    /*printf("Entrou na thread0\n");*/
    /*int tid = -1;*/
    /*tid = mcreate(thread1, (void *)&tid);*/
    /*return;*/
/*}*/
/*int main(){*/
    /*int tid = -1;*/
    /*int i;*/

    /*tid = mcreate(thread0, (void *)&i);*/
    /*tid = mcreate(thread0, (void *)&i);*/
    /*tid = mcreate(thread0, (void *)&i);*/
    /*printf("tid %d\n", tid);*/
    /*dispatch_next();*/
    /*[>tcb_priority_queue_print(ready_queue);<]*/
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
