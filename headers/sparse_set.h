#pragma once
#include "vector.h"
#include "common.h"

typedef struct sparse_set
{
    u32* dense;
    u32* sparse;
    u32 denseCapacity;
    u32 denseCount;
    u32 maxVal;

}sparse_set;


void ss_init(sparse_set* pSs, size_t initialCount);

void ss_insert(sparse_set* pSs, u32 val);

u32 ss_find(sparse_set* pSs, u32 val);

void ss_remove(sparse_set* pSs, u32 val);

void ss_free(sparse_set* pSs);

u32 ss_count(const sparse_set* pSs);