#include "allocation.h"
#ifdef DEBUG

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

DebugAllocateInfo debugAllocationInfo;

void dai_init()
{
    v_alloc(&debugAllocationInfo.allocations, 128 * sizeof(Allocation));
}

void* dalloc(size_t size, const char* fileName,const char* functionName, int line)
{
    printf("dai@%p\n",&debugAllocationInfo);
    void* retval = malloc(size);
    printf("Allocated %lu bytes at adress %p @%s:%s:%d\n",size, retval,fileName,functionName,line);

    Allocation* ptr = v_push_back(&debugAllocationInfo.allocations, sizeof(Allocation));
    ptr->ptr = retval;
    strcpy(ptr->file, fileName);
    strcpy(ptr->function, functionName);
    ptr->size = size;
    ptr->line = line;

    return retval;
}


void dfree(void* ptr)
{
    Allocation* toDelete = NULL;
    printf("pointers:\n");
    V_FOR(Allocation*, iter, &debugAllocationInfo.allocations)
    {
        printf("%p\n",iter->ptr);
        if(iter->ptr == ptr)
        {
            toDelete = iter;
            printf("@%p\nhuh\n", iter);
        }
    }
    if(!toDelete)
    {
        fprintf(stderr, "%p not found in allocationInfo struct\n", ptr);
        free(ptr);
        return;
    }
    printf("Freeing %p\n", ptr);
    free(ptr);
    printf("Freed %p\n", ptr);
    Allocation* last = ((Allocation*)debugAllocationInfo.allocations.end) - 1;
    fprintf(stderr, "%lu bytes @%p allocated in %s:%s, line %d not freed!\n", last->size, last->ptr, last->file, last->function, last->line);
    toDelete->line = last->line;
    toDelete->ptr = last->ptr;
    toDelete->size = last->size;
    v_pop_back(&debugAllocationInfo.allocations, sizeof(Allocation));
}

int checkMemory()
{
    V_FOR(Allocation*, iter, &debugAllocationInfo.allocations)
    {
        fprintf(stderr, "%lu bytes @%p allocated in %s:%s, line %d not freed!\n", iter->size, iter->ptr, iter->file,iter->function, iter->line);
    }
    if(v_size(&debugAllocationInfo.allocations) != 0)
        return -1;
    else
        return 0;        
}

#endif