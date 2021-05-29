#include "Nori.h"


nori_vector* nori_vector_init(u64 elementSize, u32 capacity)
{
    nori_vector* pVec = MALLOC(sizeof(nori_vector) * elementSize * capacity);
    pVec->elementSize = elementSize;
    pVec->size = 0;
    pVec->cap = capacity;

    return pVec;
}

static void reserve(nori_vector** pVec, u32 newCap)
{
    nori_vector* pNew = REALLOC(*pVec, newCap);
    if (pNew)
    {
        *pVec = pNew;
        pNew->cap = newCap;
    }
}

void nori_vector_push_back(nori_vector** pVec, void* pSrc, u32 elementCount)
{
    if ((*pVec)->size + elementCount >= (*pVec)->cap)
    {
        u32 newCap = max((*pVec)->cap * 2, (*pVec)->size + elementCount);
        reserve(pVec, newCap);
    }

    
}

void* nori_vector_emplace_back(nori_vector** pVec, u32 elementCount)
{
    if ((*pVec)->size + elementCount >= (*pVec)->cap)
    {
        u32 newCap = max((*pVec)->cap * 2, (*pVec)->size + elementCount);
        reserve(pVec,newCap);
    }

    void* retval = (*pVec)->vec + (*pVec)->size * (*pVec)->elementSize;
    (*pVec)->size += elementCount * (*pVec)->elementSize;
    return retval;
}
