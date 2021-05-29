#include "Nori.h"

void idq_init(id_queue *pQue, u32 initialSize)
{
    pQue->queue = NULL;
    nr_v_reserve(pQue->queue, initialSize);
}

void idq_push(id_queue *pQue, entity_t id)
{
    nr_v_push(pQue->queue, id);
}

void idq_pop(id_queue *pQue, entity_t*id)
{
    *id = v_last(pQue->queue);
    nr_v_pop(pQue->queue);
}

void idq_free(id_queue *pQue)
{
    nr_v_free(pQue->queue);
}

bool idq_is_empty(const id_queue *pQue)
{
    return nr_v_size(pQue->queue) == 0;
}
