#include "Nori.h"

void idq_init(id_queue *pQue)
{
    pQue->back = pQue->front = NULL;
}

void idq_push(id_queue *pQue, entity_t id)
{
    id_node *newBack = MALLOC(sizeof(id_node));
    newBack->id = id;
    newBack->next = NULL;
    if (pQue->back)
        pQue->back->next = newBack;
    pQue->back = newBack;

    if (!pQue->front)
    {
        pQue->front = newBack;
    }

}

void idq_pop(id_queue *pQue, entity_t*id)
{
    if (id)
        *id = pQue->front->id;

    id_node *toDelete = pQue->front;
    if (toDelete == pQue->back)
    {
        pQue->front = pQue->back = NULL;
    } else
        pQue->front = pQue->front->next;


    FREE(toDelete);
}

void idq_free(id_queue *pQue)
{

    while (pQue->front)
    {
        id_node *toDelete = pQue->front;
        pQue->front = pQue->front->next;
        FREE(toDelete);
    }
    pQue->back = pQue->front = NULL;

}

bool idq_is_empty(const id_queue *pQue)
{
    return pQue->front == NULL;
}
