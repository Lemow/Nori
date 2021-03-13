#pragma once
#include "id_queue.h"
#include "cvec.h"

typedef struct entity_registry
{
    id_queue idQueue;
    //component vectors
    cvec* cVectors;
    u32 maxID;
    u32 cvecCount;
    u32 cvecCapacity;
}entity_registry;


void er_init(entity_registry* pEr, u32 cvecCount);

u32 er_create_entity(entity_registry* pEr);

void er_remove_entity(entity_registry* pEr, u32 entityID);

u32 er_add_cvec(entity_registry *pEr, u32 componentSize, u32 InitialCount);

void er_push_component(entity_registry* pEr, u32 entityID, u32 componentID , const void* pComponent);

void* er_emplace_component(entity_registry *pEr, u32 entityID, u32 componentID);

void* er_get_component(entity_registry* pEr, u32 entityID, u32 componentID);

void er_free(entity_registry* pEr);