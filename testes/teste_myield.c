#include <stdio.h>
#include <stdlib.h>
#include "../include/mthread.h"

void thread2(void *arg) {
    printf("Entrou na thread2\n");
    myield();
    printf("Voltou na thread2\n");
    return;
}

void thread1(void *arg) {
    printf("initjoin\n");
    mjoin(3);
    printf("endjoin\n");
    printf("Entrou na thread1\n");
    return;
}
void thread0(void *arg) {
    int i;
    mcreate(thread2, (void *)&i);
    printf("Entrou na thread0\n");
    myield();
    printf("Voltou na thread0\n");
    return;
}
int main(){
    int tid = -1;
    int i;

    printf("\n Teste myield \n");

    tid = mcreate(thread1, (void *)&i);
    printf("tid %d\n", tid);
    tid = mcreate(thread0, (void *)&i);
    printf("tid %d\n", tid);
    tid = mcreate(thread0, (void *)&i);
    printf("tid %d\n", tid);

    printf("entrou\n");
    mjoin(1);
    mjoin(2);
    printf("acabou\n");
    return 0;
}


