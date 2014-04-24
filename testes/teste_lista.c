#include <stdio.h>
#include <stdlib.h>
#include "../include/mthread.h"

int main(){
    tcb_t *aux = tcb_create(1, 0, NULL);
    tcb_t *aux2 = tcb_create(2, 0, NULL);
    tcb_t *aux3 = tcb_create(3, 0, NULL);
    tcb_t *aux4 = tcb_create(4, 0, NULL);

    tcb_list_t *list = tcb_list_create();

    // Testa inicio
    tcb_list_print(list);
    tcb_list_remove(list, aux);
    tcb_list_remove(list, aux);
    tcb_list_print(list);
    tcb_list_add(list, aux);
    tcb_list_print(list);
    tcb_list_remove(list, aux);
    tcb_list_remove(list, aux); // SegFault
    tcb_list_print(list);
    aux = tcb_create(1,0,NULL);
    tcb_list_add(list, aux);
    tcb_list_print(list);

    tcb_list_add(list, aux);
    tcb_list_add(list, aux2);
    tcb_list_add(list, aux3);
    tcb_list_remove(list, aux3); // meio
    tcb_list_remove(list, aux2); // meio
    tcb_list_remove(list, aux);
    tcb_list_add(list, aux4);
    tcb_list_remove(list, aux4);
    tcb_list_print(list);

    // Teste simples
    tcb_list_add(list, aux);
    tcb_list_add(list, aux2);
    tcb_list_add(list, aux3);
    tcb_list_add(list, aux4);
    tcb_list_print(list);
    tcb_list_remove(list, aux4); // fim
    tcb_list_print(list);
    tcb_list_print(list);
    tcb_list_remove(list, aux); // inicio
    tcb_list_print(list);
    return 0;
}
