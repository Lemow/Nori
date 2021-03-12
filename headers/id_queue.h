#pragma once
#include "common.h"

typedef struct id_node
{
    u32 id;
    struct id_node *next;
}id_node;

typedef struct id_queue
{
    id_node *back;
    id_node *front;

}id_queue;

void idq_init(id_queue *pQue);

void idq_push(id_queue *pQue, u32 id);

void idq_pop(id_queue *pQue, u32* id);

bool idq_is_empty(const id_queue *pQue);

void idq_free(id_queue* free);