#include <stdio.h>
#include <stdlib.h>
#include "../include/mthread.h"

void thread1(void *arg) {
    printf("Entrou na thread1\n");
    return;
}
void thread0(void *arg) {
    printf("Entrou na thread0\n");
    int tid = -1;
    tid = mcreate(thread1, (void *)&tid);
    mjoin(1);
    return;
}
int main(){
    int tid = -1;
    int i;

    printf("\n Teste mjoin: \n");

    tid = mcreate(thread0, (void *)&i);
    printf("tid %d\n", tid);
    mjoin(1);
    mjoin(1);
    printf("acabou\n");
    return 0;
}
