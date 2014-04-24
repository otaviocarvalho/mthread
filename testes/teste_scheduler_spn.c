#include <stdio.h>
#include <stdlib.h>
#include "../include/mthread.h"

mmutex_t mutex;

void thread1(void *arg) {
    printf("Entrou na thread1\n");
    int i = 0;
    int j = 0;
    for (i = 0; i < 1000000; i++) {
        j++;
    }
    myield();
    printf("Entrou na thread1 again\n");
    return;
}

void thread0(void *arg) {
    printf("Entrou na thread0\n");
    myield();
    printf("Entrou na thread0 again\n");
    return;
}

int main(){
    int tid = -1;
    int i;

    printf("\n Teste scheduler SPN: \n");

    mmutex_init(&mutex);
    tid = mcreate(thread1, (void *)&i);
    printf("tid %d\n", tid);
    tid = mcreate(thread0, (void *)&i);
    printf("tid %d\n", tid);
    tid = mcreate(thread0, (void *)&i);
    printf("tid %d\n", tid);

    mjoin(1);
    mjoin(2);
    mjoin(3);

    return 0;
}
