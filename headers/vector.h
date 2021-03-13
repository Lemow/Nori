#pragma once

#include <stdlib.h>
#include <assert.h>

typedef struct vector
{
    void *begin;
    void *end;
    void *capacity;
} vector;

typedef struct svector
{
    vector vec;
    size_t elementSize;
}svector;

void v_init(vector* pVec);

void v_alloc(vector *pVec, size_t size);

void v_realloc(vector *pVec, size_t size);

size_t v_size(const vector *pVec);

size_t v_capacity(const vector* pVec);

//does not set pointers to NULL, only frees memory
void v_free(vector* pVec);

void v_reserve(vector* pVec, size_t newCapacity);

//returns pointer with size bytes of usable memory
void* v_push_back(vector* pVec, size_t size);

void v_pop_back(vector* pVec, size_t elementSize);

#define V_FOR(ptrType, ptrName, pVec) for(ptrType ptrName = (ptrType)(pVec)->begin; ptrName != (pVec)->end; ptrName++)