#include "cvec.h"
#include "allocation.h"

void cv_init(cvec *pCv, u32 componentSize, u32 initialCount)
{
    ss_init(&pCv->entitySet, initialCount);
    pCv->capacity = initialCount;
    pCv->componentCount = 0;
    pCv->componentSize = componentSize;
    pCv->components = MALLOC(pCv->componentSize * initialCount);

}

size_t cv_sizeof(const cvec *pCv)
{
    size_t size = sizeof(cvec);
    size += pCv->capacity * pCv->componentSize;
    size += (pCv->entitySet.denseCapacity + pCv->entitySet.maxVal + 1) * sizeof(u32);
    return size;
}

void *cv_find(cvec *pCv, u32 id)
{
    u32 index = ss_find(&pCv->entitySet, id);

    return (void*)((index != -1) * (u64)(pCv->components + (index * pCv->componentSize)));
}

void cv_free(cvec *pCv)
{

    FREE(pCv->components);
    ss_free(&pCv->entitySet);
}

void cv_push(cvec *pCv, const void *pComponent, u32 id)
{
#ifdef DEBUG
    if (ss_find(&pCv->entitySet, id) != -1)
    {
        fprintf(stderr, "id %u already in set\n", id);
        return;
    }

#endif

    if (pCv->capacity == pCv->componentCount)
    {
        pCv->components = realloc(pCv->components, (pCv->capacity * 2) * pCv->componentSize);
    }

    void *pInsert = pCv->components + (pCv->componentCount * pCv->componentSize);

    memcpy(pInsert, pComponent, pCv->componentSize);

    ss_insert(&pCv->entitySet, id);

    pCv->componentCount++;
}

void *cv_emplace(cvec *pCv, u32 id)
{
#ifdef DEBUG
    if (ss_find(&pCv->entitySet, id) != -1)
    {
        fprintf(stderr, "id %u already in set\n", id);
        return NULL;
    }
#endif
    if (pCv->capacity == pCv->componentCount)
    {
        pCv->components = realloc(pCv->components, (pCv->capacity * 2) * pCv->componentSize);
    }

    void *pInsert = pCv->components + (pCv->componentCount * pCv->componentSize);

    ss_insert(&pCv->entitySet, id);

    pCv->componentCount++;
    return pInsert;
}

void cv_remove(cvec *pCv, u32 id)
{
    void *pToRemove = pCv->components + ss_find(&pCv->entitySet, id) * pCv->componentSize;
    void *pLast = pCv->components + (pCv->componentCount - 1) * pCv->componentSize;
    ss_remove(&pCv->entitySet, id);

    memcpy(pToRemove, pLast, pCv->componentSize);
    pCv->componentCount--;
}