#pragma once
#include "vector.h"

typedef struct Allocation
{
    void* ptr;
    char file[256];
    char function[256];
    size_t size;
    int line;
}Allocation;

typedef struct DebugAllocateInfo
{
    vector allocations;

}DebugAllocateInfo;


#ifdef DEBUG

    void dai_init();
    void *dalloc(size_t size, const char *fileName, const char* functionName, int line);
    void dfree(void* ptr);
    int checkMemory();

#define MALLOC(size) dalloc(size, __FILE__, __FUNCTION__,__LINE__)
#define FREE(ptr) dfree(ptr)
#define CHECK_MEMORY() checkMemory() 
#else

    #define MALLOC(size) malloc(size)
    #define FREE(ptr) free(ptr)
    #define CHECK_MEMORY() 0
#endif