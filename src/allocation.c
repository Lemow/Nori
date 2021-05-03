#include "Nori.h"

#ifdef DEBUG

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

DebugAllocateInfo debugAllocationInfo;

void av_grow(alloc_vector* pAv)
{
    size_t newCap = pAv->cap - pAv->begin;
    size_t newEnd = pAv->end - pAv->begin;
    Allocation* newBegin = realloc(pAv->begin, newCap * sizeof(Allocation));
    if (newBegin)
    {
        pAv->begin = newBegin;
        pAv->end = pAv->begin + newEnd;
    }
    pAv->cap = pAv->begin + newCap;
}

void av_init(alloc_vector* pAv)
{
    pAv->begin = malloc(sizeof(Allocation) * 16);
    pAv->end = pAv->begin;
    pAv->cap = pAv->begin + 16;
}

void av_push(alloc_vector* pAv, const Allocation* alloc)
{
    if (pAv->end == pAv->cap)
    {
        av_grow(pAv);
    }

    memcpy(pAv->end, alloc, sizeof(Allocation));
    pAv->end++;
}

bool av_remove(alloc_vector* pAv, void* pToRemove)
{
    Allocation* pToDelete = av_find(pAv, pToRemove);

    if (!pToDelete)
    {
        fprintf(stderr, "%p not found in allocation vector.\n", pToRemove);
        return false;
    }
    else
    {
        pAv->end--;
        memcpy(pToDelete, pAv->end, sizeof(Allocation));
        return true;
    }
}

void av_free(alloc_vector* pAv)
{
    free(pAv->begin);
}

size_t av_size(const alloc_vector* pAv)
{
    return pAv->end - pAv->begin;
}

Allocation* av_emplace(alloc_vector* pAv)
{
    if (pAv->end == pAv->cap)
    {
        av_grow(pAv);
    }

    Allocation* retval = pAv->end;
    pAv->end++;

    return retval;
}

Allocation* av_find(alloc_vector* pAv, const void* ptr)
{
    Allocation* begin = pAv->begin;
    Allocation* end = pAv->end;
    for (; begin != end; begin++)
    {
        if (begin->ptr == ptr)
        {
            return begin;
        }
    }

    return NULL;
}

void dai_init()
{
    av_init(&debugAllocationInfo.allocations);
}

void* dalloc(size_t size, const char* fileName,const char* functionName, int line)
{
    void* retval = malloc(size);

    Allocation* ptr = av_emplace(&debugAllocationInfo.allocations);
    ptr->ptr = retval;
    strcpy(ptr->file, fileName);
    strcpy(ptr->function, functionName);
    ptr->size = size;
    ptr->line = line;

    return retval;
}


void dfree(void* ptr)
{
    av_remove(&debugAllocationInfo.allocations, ptr);
    free(ptr);
}

void* drealloc(void* ptr, size_t size)
{
    Allocation* pAlloc = av_find(&debugAllocationInfo.allocations, ptr);
    if (!pAlloc)
        return realloc(ptr, size);

    void* retval = realloc(ptr, size);

    if (retval)
        pAlloc->ptr = retval;

    return retval;
}

int checkMemory()
{
    Allocation* iter = debugAllocationInfo.allocations.begin;
    Allocation* end = debugAllocationInfo.allocations.end;
    
    for (; iter != end; iter++)
    {
        fprintf(stderr, "%lu bytes @%p allocated in %s:%s, line %d not freed!\n", iter->size, iter->ptr, iter->file,iter->function, iter->line);
    }

    int retval;
    if(av_size(&debugAllocationInfo.allocations) != 0)
        retval = -1;
    else
        retval = 0;        

    return retval;
}

#endif