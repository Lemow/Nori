#include "sparse_set.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void ss_init(sparse_set *pSs, size_t initialCount)
{
    fprintf(stderr,"dense: %p\n",pSs->dense);
    pSs->dense = malloc(initialCount * sizeof(u32));
    fprintf(stderr, "dense: %p\n\n", pSs->dense);

    //free(pSs->dense);
    pSs->sparse = malloc(initialCount * sizeof(u32));
    pSs->denseCapacity = initialCount;
    pSs->maxVal = initialCount - 1;
    pSs->denseCount = 0;
}

u32 ss_count(const sparse_set* pSs)
{
    return pSs->denseCount;
}

u32 ss_find(sparse_set* pSs, u32 val)
{
    if(val > pSs->maxVal)
        return -1;

    u32 index = pSs->sparse[val];

    if(index >= pSs->denseCount)
        return -1;

    
    if(pSs->dense[index] == val)
        return index; 
    else
        return -1;
}


void ss_insert(sparse_set *pSs, u32 val)
{

    if(pSs->maxVal < val)
    {
        pSs->sparse = realloc(pSs->sparse, (val +1) * sizeof(u32));
        pSs->maxVal = val;
    }

    if(pSs->denseCount == pSs->denseCapacity)
    {
        u32 newCap = pSs->denseCapacity * 2; 
        u32* newDense = malloc(newCap * sizeof(u32));
        memcpy(newDense, pSs->dense, pSs->denseCapacity * sizeof(u32));
        fprintf(stderr, "%s:%d:\n", __FILE__, __LINE__);
        fprintf(stderr, "%p\n",pSs->dense);
        for(int i = 0; i < pSs->denseCount;i++)
        {
            if(i)
                fprintf(stderr, "\n");
            fprintf(stderr, "new[%d] = %u, old[%d] = %u", i,newDense[i], i, pSs->dense[i]);
        }
        fprintf(stderr,"\n\n");
        free(pSs->dense);

        pSs->dense = newDense;

        pSs->denseCapacity = newCap;
    }


    pSs->dense[pSs->denseCount] = val;
    pSs->sparse[val] = pSs->denseCount;

    pSs->denseCount++;

}


void ss_remove(sparse_set* pSs, u32 val)
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

void ss_free(sparse_set* pSs)
{
    free(pSs->dense);
    free(pSs->sparse);
}
