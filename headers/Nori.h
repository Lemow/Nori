#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>


#define min(a,b) (((a) <= (b)) ? (a) : (b))

#define max(a, b) (((a) >= (b)) ? (a) : (b))

#define CV_FOR(componentType, ptrName, pCv) for (componentType *ptrName = (componentType *)(pCv)->components; ptrName != (componentType *)((pCv)->components) + (pCv)->componentCount; ptrName++)

#define true 1
#define false 0

typedef char bool;

typedef char i8;
typedef short i16;
typedef int i32;
typedef long long i64;

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef float f32;
typedef double f64;
typedef long double f80;

typedef char* string;

/// DEBUG ALLOCATION INFO

#ifdef DEBUG

typedef struct Allocation
{
    void* ptr;
    char file[256];
    char function[256];
    size_t size;
    int line;
}Allocation;

typedef struct alloc_vector
{
    Allocation* begin;
    Allocation* end;
    Allocation* cap;
}alloc_vector;

typedef struct DebugAllocateInfo
{
    alloc_vector allocations;

}DebugAllocateInfo;




void av_init(alloc_vector * pAv);
void av_push(alloc_vector * pAv, const Allocation * alloc);
void av_remove(alloc_vector * pAv, void* pToRemove);
void av_free(alloc_vector * pAv);
size_t av_size(const alloc_vector * pAv);
Allocation* av_emplace(alloc_vector * pAv);
Allocation* av_find(alloc_vector * pAv, const void* ptr);


void dai_init();
void* dalloc(size_t size, const char* fileName, const char* functionName, int line);
void dfree(void* ptr);
void* drealloc(void* ptr, size_t size);
int checkMemory();

#define DAI_INIT() dai_init()
#define MALLOC(size) dalloc(size, __FILE__, __FUNCTION__,__LINE__)
#define FREE(ptr) dfree(ptr)
#define REALLOC(ptr, size) drealloc(ptr,size)
#define CHECK_MEMORY() checkMemory() 
#else
#define DAI_INIT() 
#define MALLOC(size) malloc(size)
#define REALLOC(ptr, size) realloc(ptr, size)
#define FREE(ptr) free(ptr)
#define CHECK_MEMORY() 0
#endif


/// <summary>
/// SPARSE SET
/// </summary>

typedef struct sparse_set
{
    u32* dense;
    u32* sparse;
    u32 denseCapacity;
    u32 denseCount;
    u32 maxVal;

}sparse_set;


void ss_init(sparse_set* const pSs, size_t initialCount);

void ss_insert(sparse_set* const pSs, u32 val);

u32 ss_find(const sparse_set* const pSs, u32 val);

void ss_remove(sparse_set* const pSs, u32 val);

void ss_free(sparse_set* const pSs);

u32 ss_count(const sparse_set* pSs);

/// <summary>
///  ID QUEUE
/// </summary>
typedef struct id_node
{
    u32 id;
    struct id_node* next;
}id_node;

typedef struct id_queue
{
    id_node* back;
    id_node* front;

}id_queue;

void idq_init(id_queue* pQue);

void idq_push(id_queue* pQue, u32 id);

void idq_pop(id_queue* pQue, u32* id);

bool idq_is_empty(const id_queue* pQue);

void idq_free(id_queue* free);


typedef struct cvec
{
    void* components;
    sparse_set entitySet;
    u32 capacity;
    u32 componentCount;
    u32 componentSize;
} cvec;

/// <summary>
/// COMPONENT VECTOR
/// </summary>

void cv_init(cvec* pCv, u32 componentSize, u32 initialCount);

void cv_push(cvec* pCv, const void* pComponent, u32 id);

void* cv_emplace(cvec* pCv, u32 id);

void cv_remove(cvec* pCv, u32 id);

void cv_free(cvec* pCv);

void* cv_find(cvec* pCv, u32 id);

size_t cv_sizeof(const cvec* pCv);


typedef struct entity_registry
{
    id_queue idQueue;
    //component vectors
    cvec* cVectors;
    u32 maxID;
    u32 cvecCount;
    u32 cvecCapacity;
}entity_registry;


void er_init(entity_registry* pEr, u32 cvecCount);

u32 er_create_entity(entity_registry* pEr);

void er_remove_entity(entity_registry* pEr, u32 entityID);

u32 er_add_cvec(entity_registry* pEr, u32 componentSize, u32 InitialCount);

void er_push_component(entity_registry* pEr, u32 entityID, u32 componentID, const void* pComponent);

void* er_emplace_component(entity_registry* pEr, u32 entityID, u32 componentID);

void* er_get_component(entity_registry* pEr, u32 entityID, u32 componentID);

void er_free(entity_registry* pEr);