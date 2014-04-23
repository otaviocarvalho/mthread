#include <stdio.h>
#include <stdlib.h>
#include "../include/mthread.h"

void thread0(void *arg) {
    int i;
    printf("Entrou na thread0\n");
    return;
}
int main(){
    int tid = -1;
    int i;

    printf("\nTeste mlock: \n");

    tid = mcreate(thread0, (void *)&i);
    printf("tid %d\n", tid);
    tid = mcreate(thread0, (void *)&i);
    printf("tid %d\n", tid);
    tid = mcreate(thread0, (void *)&i);
    printf("tid %d\n", tid);
    dispatch_next();

    printf("entrou\n");
    mjoin(1);
    printf("acabou\n");
    //tcb_priority_queue_print(ready_queue);
    return 0;
}
