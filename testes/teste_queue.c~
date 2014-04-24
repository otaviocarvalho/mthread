#include <stdio.h>
#include <stdlib.h>
#include "../include/mthread.h"

int main(){
    tcb_t *aux = tcb_create(1, 0, NULL);
    tcb_t *aux2 = tcb_create(2, 0, NULL);
    tcb_t *aux3 = tcb_create(3, 0, NULL);
    tcb_t *aux4 = tcb_create(4, 0, NULL);

    tcb_priority_queue_t *q = tcb_priority_queue_init();

    printf("\n Testa inicio \n");

    tcb_priority_queue_print(q);
    tcb_priority_queue_remove(q);
    tcb_priority_queue_remove(q);
    tcb_priority_queue_print(q);
    tcb_priority_queue_add(q,aux);
    tcb_priority_queue_add(q,aux2);
    tcb_priority_queue_add(q,aux3);
    tcb_priority_queue_print(q);
    tcb_priority_queue_remove(q);
    tcb_priority_queue_remove(q);
    tcb_priority_queue_remove(q);
    tcb_priority_queue_remove(q);
    tcb_priority_queue_print(q);
}

