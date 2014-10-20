/*--------------------------------------------------------------------------*/
/*
                    UNIVERSIDADE TECNOLÓGICA DO PARANÁ
                     projeto10 - Sistemas Operacionais
    _________________________________________________________________________
    *Autor: Gustavo Yudi Bientinezi Matsuzake;                              *
    *Matrícula: 1233734;                                                    *
    *Professor: Marco Aurélio Wehrmeister                                   *
    *************************************************************************
/*--------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include "queue.h"


#define ERRO_FILANAOEXISTE printf("\n\nERRO! fila nao existe\n\n");
#define ERRO_ELEMENTONAOEXISTE printf("\n\nERRO! elemento nao existe\n\n");
#define ERRO_ELEMENTODEOUTRAFILA printf("\n\nERRO! elemento ja existe em outra fila\n\n");
#define ERRO_FILAVAZIA printf("\n\nERRO! fila vazia\n\n");

void queue_append (queue_t **queue, queue_t *elem){

    if(queue == NULL)
        ERRO_FILANAOEXISTE
    else if(elem == NULL)
        ERRO_ELEMENTONAOEXISTE
    else if(elem->next != NULL || elem->prev != NULL)
        ERRO_ELEMENTODEOUTRAFILA
    else if(*queue == NULL){
        *queue = elem;
        elem->next = elem;
        elem->prev = elem;
    }else{
        elem->next = (*queue);
        elem->prev = (*queue)->prev;

        (*queue)->prev->next = elem;
        (*queue)->prev = elem;
    }


}

//------------------------------------------------------------------------------
// Remove o elemento indicado da fila, sem o destruir.
// Condicoes a verificar, gerando msgs de erro:
// - a fila deve existir
// - a fila nao deve estar vazia
// - o elemento deve existir
// - o elemento deve pertencer a fila indicada
// Retorno: apontador para o elemento removido, ou NULL se erro

queue_t *queue_remove (queue_t **queue, queue_t *elem){
    if(queue == NULL){
        ERRO_FILANAOEXISTE
        exit(1);
    }
    if(*queue == NULL){
        ERRO_FILAVAZIA
        exit(1);
    }
    if(elem == NULL){
        ERRO_ELEMENTONAOEXISTE
        exit(1);
    }

    queue_t* i;

    for(i=(*queue); 1; i=i->next){

        if(i==elem){

            if(queue_size(*queue)==1){

                *queue = NULL;
            }
            else{
                
                if((*queue)==elem)
                    (*queue) = elem->next;
                elem->next->prev = i->prev;
                elem->prev->next = i->next;
            }
            elem->next = NULL;
            elem->prev = NULL;
            return i;
        }
        if(i->next == (*queue))
            break;
    }

    ERRO_ELEMENTONAOEXISTE

    return NULL;
}

int queue_size (queue_t *queue){
    if(queue == NULL)
        return 0;
    queue_t* i = queue;
    int tamanho = 1;

    while((i = i->next)!=queue)
        tamanho++;

    return tamanho;
}

void queue_print (char *name, queue_t *queue, void print_elem (void*) ) {


    queue_t *i = queue;
    printf("\n%s\n", name);

    printf("[");

    print_elem(i);
    printf(" ");

    while(i && (i=i->next)!=queue){
        print_elem(i);
        printf((i->next != queue)?" ":"");
    }

    printf("]\n");

}
