#pragma once
#include "common.h"
#include "sparse_set.h"
#include <stdio.h>
#include <string.h>

typedef struct cvec
{
    void *components;
    sparse_set entitySet;
    u32 capacity;
    u32 componentCount;
    u32 componentSize;

} cvec;

void cv_init(cvec *pCv, u32 componentSize, u32 initialCount);

void cv_push(cvec *pCv, const void *pComponent, u32 id);

void* cv_emplace(cvec* pCv, u32 id);

void cv_remove(cvec *pCv, u32 id);

void cv_free(cvec* pCv);

void* cv_find(cvec* pCv, u32 id);

size_t cv_sizeof(const cvec* pCv);

#define CV_FOR(componentType, ptrName, pCv) for (componentType *ptrName = (componentType *)(pCv)->components; ptrName != (componentType *)((pCv)->components) + (pCv)->componentCount; ptrName++)
