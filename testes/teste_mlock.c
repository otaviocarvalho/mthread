#include <stdio.h>
#include <stdlib.h>
#include "../include/mthread.h"

int test = 0;
mmutex_t *mutex;

void thread1(void *arg) {
    printf("Entrou na thread1\n");
    return;
}

void thread0(void *arg) {
    printf("Entrou na thread0\n");
    mlock(mutex);
    printf("bloqueou thread\n");
    test++;
    munlock(mutex);
    printf("desbloqueou thread\n");
    return;
}

int main(){
    int tid = -1;
    int i;

    printf("\n Teste da funcao mlock \n");

    mmutex_init(mutex);
    tid = mcreate(thread0, (void *)&i);
    printf("tid %d\n", tid);
    tid = mcreate(thread1, (void *)&i);
    printf("tid %d\n", tid);
    tid = mcreate(thread1, (void *)&i);

    mjoin(1);
    mjoin(2);
    mjoin(3);

    mlock(mutex);
    printf("bloqueou thread main\n");
    test++;
    printf("%d\n", test);
    munlock(mutex);
    printf("desbloqueou thread main\n");

    printf("entrou\n");
    printf("acabou\n");
    return 0;
}


