#include "sparse_set.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "allocation.h"

void ss_init(sparse_set *pSs, size_t initialCount)
{
    pSs->dense = MALLOC(initialCount * sizeof(u32));
    pSs->sparse = MALLOC(initialCount * sizeof(u32));
    pSs->denseCapacity = initialCount;
    pSs->maxVal = initialCount - 1;
    pSs->denseCount = 0;
}

u32 ss_count(const sparse_set *pSs)
{
    return pSs->denseCount;
}

u32 ss_find(sparse_set *pSs, u32 val)
{
    if (val > pSs->maxVal)
        return -1;

    u32 index = pSs->sparse[val];

    if (index >= pSs->denseCount)
        return -1;

    if (pSs->dense[index] == val)
        return index;
    else
        return -1;
}

void ss_insert(sparse_set *pSs, u32 val)
{

    if (pSs->maxVal < val)
    {
        pSs->sparse = realloc(pSs->sparse, (val + 1) * sizeof(u32));
        pSs->maxVal = val;
    }

    if (pSs->denseCount == pSs->denseCapacity)
    {
        u32 newCap = pSs->denseCapacity * 2;
        fprintf(stderr, "%u\n",pSs->dense[pSs->denseCount - 1]);
        pSs->dense = realloc(pSs->dense, newCap * sizeof(u32));
        pSs->denseCapacity = newCap;
    }

    pSs->dense[pSs->denseCount] = val;
    pSs->sparse[val] = pSs->denseCount;

    pSs->denseCount++;
}

void ss_remove(sparse_set *pSs, u32 val)
{
    u32 index = pSs->sparse[val];
    u32 lastIndex = pSs->denseCount - 1;
    u32 lastVal = pSs->sparse[pSs->dense[lastIndex]];

    pSs->dense[index] = lastVal;
    pSs->dense[lastIndex] = val;

    pSs->sparse[val] = lastIndex;
    pSs->sparse[lastVal] = index;

    pSs->denseCount--;
}

void ss_free(sparse_set *pSs)
{
    printf("%p\n", pSs->dense);
    FREE(pSs->dense);
    FREE(pSs->sparse);
}
