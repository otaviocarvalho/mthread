#include <stdio.h>
#include <stdlib.h>
#include "../include/mthread.h"

mmutex_t mutex;
int test = 0;
void thread1(void *arg) {
    printf("Entrou na thread1\n");
    return;
}
void thread0(void *arg) {
    printf("Entrou na thread0\n");
}
int main(){
    int tid = -1;
    int i;
    
    printf("\n Teste mlock \n");

    mmutex_init(&mutex);
    tid = mcreate(thread0, (void *)&i);
    printf("tid %d\n", tid);
    tid = mcreate(thread1, (void *)&i);
    printf("tid %d\n", tid);
    tid = mcreate(thread1, (void *)&i);

    printf("bloqueou thread main\n");
    mjoin(1);
    printf("desbloqueou thread main\n");

    printf("entrou\n");
    printf("acabou\n");
    return 0;
}
