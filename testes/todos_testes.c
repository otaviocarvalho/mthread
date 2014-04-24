// Testa mlock
mmutex_t mutex;
int test = 0;
void thread1(void *arg) {
    printf("Entrou na thread1\n");
    return;
}
void thread0(void *arg) {
    printf("Entrou na thread0\n");
    /*mlock(&mutex);*/
    /*printf("bloqueou thread\n");*/
    /*test++;*/
    /*munlock(&mutex);*/
    /*printf("desbloqueou thread\n");*/
    return;
}
int main(){
    int tid = -1;
    int i;

    mmutex_init(&mutex);
    tid = mcreate(thread0, (void *)&i);
    printf("tid %d\n", tid);
    tid = mcreate(thread1, (void *)&i);
    printf("tid %d\n", tid);
    tid = mcreate(thread1, (void *)&i);

    /*mlock(&mutex);*/
    printf("bloqueou thread main\n");
    /*test++;*/
    /*printf("%d\n", test);*/
    mjoin(1);
    /*munlock(&mutex);*/
    printf("desbloqueou thread main\n");

    printf("entrou\n");
    printf("acabou\n");
    tcb_priority_queue_print(ready_queue);
    return 0;
}

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
