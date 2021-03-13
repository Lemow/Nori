#include "vector.h"
#include "common.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef DEFAULT_SIZEV
void v_init(vector *pVec)
{
    pVec->begin = NULL;
    pVec->end = NULL;
    pVec->capacity = NULL;
}
#else
void v_init(vector *pVec)
{
    v_alloc(pVec, DEFAULT_SIZEV);
}
#endif

void v_pop_back(vector* pVec, size_t elementSize)
{
    pVec->end -= elementSize;
}

void v_reserve(vector *pVec, size_t newCapacity)
{
    if(v_capacity(pVec) == newCapacity)
        return;

    v_realloc(pVec,newCapacity);
}

void* v_push_back(vector* pVec, size_t size)
{
    if(pVec->end + size > pVec->capacity)
    {
        v_realloc(pVec, max(size, v_size(pVec) * 2));
    }
    void* retval = pVec->end;
    pVec->end += size;
    return retval;
}


void v_realloc(vector* pVec, size_t newSize)
{
    const size_t oldSize = v_size(pVec);
    const size_t minSize = min(oldSize,newSize);
    void* newBegin = malloc(newSize);
    memcpy(newBegin, pVec->begin, minSize);
    free(pVec->begin);
    pVec->begin = newBegin;
    pVec->end = newBegin + minSize;
    pVec->capacity = newBegin + newSize;
}

void v_alloc(vector* pVec, size_t size)
{
    pVec->begin = pVec->end = malloc(size);
    pVec->capacity = pVec->begin + size;
}

size_t v_size(const vector* pVec)
{
    return pVec->end - pVec->begin;
}

size_t v_capacity(const vector* pVec)
{
    return pVec->capacity - pVec->begin;
}

void v_free(vector* pVec)
{
    free(pVec->begin);
    pVec->begin = NULL;
    pVec->end = NULL;
    pVec->capacity = NULL;
}