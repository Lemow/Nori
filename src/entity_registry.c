#include "Nori.h"

void er_init(entity_registry *pEr, u32 cvecCapacity)
{
    size_t cvecsize = cvecCapacity * sizeof(cvec);
    pEr->cVectors = (cvec*)MALLOC(cvecsize);
    pEr->maxID = 0;
    pEr->cvecCount = 0;
    pEr->cvecCapacity = cvecCapacity;
    idq_init(&pEr->idQueue);
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

cvec* er_get_cvec(entity_registry* pEr, componentID_t componentID)
{
#ifdef DEBUG
    if (pEr->cvecCount <= componentID)
    {
        fprintf(stderr, "ComponentID %u does not exist\n", componentID);
        return NULL;
    }
#endif
    return &pEr->cVectors[componentID];
}

void er_free(entity_registry *pEr)
{
    for (int i = 0; i < pEr->cvecCount; i++)
    {

        cv_free(&pEr->cVectors[i]);
    }
    FREE(pEr->cVectors);

    idq_free(&pEr->idQueue);
}

void er_push_component(entity_registry *pEr, entity_t entityID, componentID_t componentID, const void *pComponent)
{

    cvec *pCv = &pEr->cVectors[componentID];
    cv_push(pCv, pComponent, entityID);
}

void *er_emplace_component(entity_registry *pEr, entity_t entityID, componentID_t componentID)
{
    cvec *pCv = &pEr->cVectors[componentID];

    return cv_emplace(pCv, entityID);
}

void er_remove_entity(entity_registry *pEr, entity_t entityID)
{
    cvec *pVecs = pEr->cVectors;
    const cvec *const pEnd = pEr->cVectors + pEr->cvecCount;

    for (; pVecs != pEnd; pVecs++)
    {
        if (cv_find(pVecs, entityID) != NULL)
            cv_remove(pVecs, entityID);
    }

    idq_push(&pEr->idQueue, entityID);
}

entity_t er_add_cvec(entity_registry *pEr, u32 componentSize, u32 InitialCount)
{
#ifdef DEBUG
    if (pEr->cvecCount == pEr->cvecCapacity)
    {
        fprintf(stderr, "ERROR IN FILE %s, LINE %d:\nNot enough reserved space for another cvector!\n", __FILE__, __LINE__);
        return -1;
    }
#endif
    printf("pEr->cvecCount: %u\n", pEr->cvecCount);
    cv_init(&pEr->cVectors[pEr->cvecCount], componentSize, InitialCount);
    return pEr->cvecCount++;
}

void *er_get_component(entity_registry *pEr, entity_t entityID, componentID_t componentID)
{
    cvec *pCv = &pEr->cVectors[componentID];

    return cv_find(pCv, entityID);
}