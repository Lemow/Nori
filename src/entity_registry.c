#include "entity_registry.h"

void er_init(entity_registry *pEr, u32 cvecCapacity)
{
    idq_init(&pEr->idQueue);
    pEr->cVectors = malloc(cvecCapacity * sizeof(cvec));
    pEr->maxID = 0;
    pEr->cvecCount = 0;
    pEr->cvecCapacity = cvecCapacity;
}

u32 er_create_entity(entity_registry *pEr)
{
    u32 retval;
    if (!idq_is_empty(&pEr->idQueue))
    {
        idq_pop(&pEr->idQueue, &retval);
    }
    else
    {
        retval = pEr->maxID;
        pEr->maxID++;
    }
    return retval;
}

void er_free(entity_registry *pEr)
{
    for(int i = 0; i < pEr->cvecCount; i++)
    {
        cv_free(&pEr->cVectors[i]);
    }
    free(pEr->cVectors);

    idq_free(&pEr->idQueue);
}

void er_push_component(entity_registry *pEr, u32 entityID, u32 componentID, const void *pComponent)
{

    cvec* pCv = &pEr->cVectors[componentID];
    cv_push(pCv, pComponent, entityID);
}

void *er_emplace_component(entity_registry *pEr, u32 entityID, u32 componentID)
{
    cvec *pCv = &pEr->cVectors[componentID];

    return cv_emplace(pCv,entityID);
}


void er_remove_entity(entity_registry *pEr, u32 entityID)
{
    cvec* pVecs = pEr->cVectors;
    const cvec* const pEnd = pEr->cVectors + pEr->cvecCount;

    for(; pVecs != pEnd; pVecs++)
    {
        if(cv_find(pVecs, entityID) != NULL)
            cv_remove(pVecs, entityID);
    }

    idq_push(&pEr->idQueue, entityID);
}

u32 er_add_cvec(entity_registry *pEr, u32 componentSize, u32 InitialCount)
{
    #ifdef DEBUG
        if(pEr->cvecCount == pEr->cvecCapacity)
        {
            fprintf(stderr, "ERROR IN FILE %s, LINE %d:\nNot enough reserved space for another cvector!\n",__FILE__,__LINE__);
            return;
        }
    #endif

    cv_init(&pEr->cVectors[pEr->cvecCount],componentSize,InitialCount);
    return pEr->cvecCount++;
}

void *er_get_component(entity_registry *pEr, u32 entityID, u32 componentID)
{
    cvec* pCv = &pEr->cVectors[componentID];

    return cv_find(pCv, entityID);
}