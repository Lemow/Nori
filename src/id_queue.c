#include <stdlib.h>
#include "id_queue.h"

void idq_init(id_queue *pQue)
{
    pQue->back = pQue->front = NULL;
}

void idq_push(id_queue* pQue, u32 id)
{
    id_node* newBack = malloc(sizeof(id_node));
    newBack->id = id;
    newBack->next = NULL;
    if(pQue->back)
        pQue->back->next = newBack;
    pQue->back = newBack;

    if(!pQue->front)
    {
        pQue->front = newBack;
    }
}

void idq_pop(id_queue * pQue, u32 * id)
{
    if(id)
        *id = pQue->front->id;

    id_node* toDelete = pQue->front;
    pQue->front = pQue->front->next;

    if(toDelete == pQue->back)
    {
        pQue->back = NULL;
    }

    free(toDelete);
}

void idq_free(id_queue *pQue)
{
    while(pQue->front)
    {
        id_node* toDelete = pQue->front;
        pQue->front = pQue->front->next;
        free(toDelete);
    }
    pQue->back = pQue->front = NULL;
}

bool idq_is_empty(const id_queue *pQue)
{
    return pQue->front == NULL;
}
