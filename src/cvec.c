#include "Nori.h"

void nr_cv_init(cvec *pCv, u32 componentSize, u32 initialCount)
{
    nr_ss_init(&pCv->entitySet, initialCount);
    pCv->capacity = initialCount;
    pCv->componentCount = 0;
    pCv->componentSize = componentSize;
    pCv->components = MALLOC(pCv->componentSize * initialCount);

}

size_t nr_cv_sizeof(const cvec *pCv)
{
    size_t size = sizeof(cvec);
    size += pCv->componentCount * pCv->componentSize;
    size += (pCv->entitySet.denseCount + pCv->entitySet.maxVal + 1) * sizeof(u32);
    return size;
}

void *nr_cv_find(cvec *pCv, entity_t id)
{
    u32 index = nr_ss_find(&pCv->entitySet, id);

    return (void*)((index != -1) * (u64)((char*)pCv->components + (index * pCv->componentSize)));
}

void nr_cv_free(cvec *pCv)
{
    FREE(pCv->components);
    nr_ss_free(&pCv->entitySet);
}

void nr_cv_push(cvec *pCv, const void *pComponent, entity_t id)
{
#ifdef DEBUG
    if (nr_ss_find(&pCv->entitySet, id) != -1)
    {
        fprintf(stderr, "id %u already in set\n", id);
        return;
    }

#endif

    if (pCv->capacity == pCv->componentCount)
    {
        void* newBegin = REALLOC(pCv->components, (pCv->capacity * 2) * pCv->componentSize);
        if (newBegin)
            pCv->components = newBegin;
        pCv->capacity = pCv->capacity * 2;
    }

    void *pInsert = (char*)pCv->components + (pCv->componentCount * pCv->componentSize);
    memcpy(pInsert, pComponent, pCv->componentSize);

    nr_ss_insert(&pCv->entitySet, id);

    pCv->componentCount++;
}

void *nr_cv_emplace(cvec *pCv, entity_t id)
{
#ifdef DEBUG
    if (nr_ss_find(&pCv->entitySet, id) != -1)
    {
        return NULL;
    }
#endif
    if (pCv->capacity == pCv->componentCount)
    {
        pCv->capacity *= 2;
        void* newComponents = REALLOC(pCv->components, pCv->capacity * pCv->componentSize);
        if (newComponents)
            pCv->components = newComponents;
    }

    void *pInsert = (char*)pCv->components + (pCv->componentCount * pCv->componentSize);

    nr_ss_insert(&pCv->entitySet, id);

    pCv->componentCount++;
    return pInsert;
}

void nr_cv_remove(cvec *pCv, entity_t id)
{
    void *pToRemove = (char*)pCv->components + nr_ss_find(&pCv->entitySet, id) * pCv->componentSize;
    void *pLast = (char*)pCv->components + (pCv->componentCount - 1) * pCv->componentSize;
    nr_ss_remove(&pCv->entitySet, id);

    memcpy(pToRemove, pLast, pCv->componentSize);
    pCv->componentCount--;
}