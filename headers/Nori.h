#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#ifndef min
    #define min(a,b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef max
    #define max(a, b) (((a) > (b)) ? (a) : (b))
#endif

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

typedef char* string;
typedef unsigned char byte;

typedef u32 entity_t;
typedef u32 componentID_t;


static_assert(sizeof(i8)  == 1, "Size of types is different than expected");
static_assert(sizeof(i16) == 2, "Size of types is different than expected");
static_assert(sizeof(i32) == 4, "Size of types is different than expected");
static_assert(sizeof(i64) == 8, "Size of types is different than expected");

static_assert(sizeof(u8)  == 1, "Size of types is different than expected");
static_assert(sizeof(u16) == 2, "Size of types is different than expected");
static_assert(sizeof(u32) == 4, "Size of types is different than expected");
static_assert(sizeof(u64) == 8, "Size of types is different than expected");

static_assert(sizeof(f32) == 4, "Size of types is different than expected");
static_assert(sizeof(f64) == 8, "Size of types is different than expected");

#define global static
#define internal static
#define local_persistent static

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
void av_free(alloc_vector * pAv);
size_t av_size(const alloc_vector * pAv);
Allocation* av_emplace(alloc_vector * pAv);
Allocation* av_find(alloc_vector * pAv, const void* ptr);
bool av_remove(alloc_vector * pAv, void* pToRemove);


void dai_init();
void dfree(void* ptr);
void* dalloc(size_t size, const char* fileName, const char* functionName, int line);
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

void nr_ss_init(sparse_set* const pSs, size_t initialCount);
void nr_ss_insert(sparse_set* const pSs, entity_t entityID);
void nr_ss_remove(sparse_set* const pSs, entity_t entityID);
void nr_ss_free(sparse_set* const pSs);
u32  nr_ss_find(const sparse_set* const pSs, entity_t entityID);
u32  nr_ss_count(const sparse_set* pSs);

/// <summary>
///  ID QUEUE
/// </summary>

typedef struct id_queue
{
    u32* queue;
}id_queue;

void nr_idq_init(id_queue* pQue, u32 initialSize);
void nr_idq_push(id_queue* pQue, entity_t id);
void nr_idq_pop(id_queue* pQue, entity_t* id);
void nr_idq_free(id_queue* free);
bool nr_idq_is_empty(const id_queue* pQue);




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

void   nr_cv_init(cvec* pCv, u32 componentSize, u32 initialCount);
void   nr_cv_push(cvec* pCv, const void* pComponent, entity_t entityID);
void   nr_cv_remove(cvec* pCv, entity_t entityID);
void   nr_cv_free(cvec* pCv);
void*  nr_cv_emplace(cvec* pCv, entity_t entityID);
void*  nr_cv_find(cvec* pCv, entity_t entityID);
size_t nr_cv_sizeof(const cvec* pCv);


typedef struct entity_registry
{
    id_queue idQueue;
    //component vectors
    cvec* cVectors;
    u32 maxID;
    u32 cvecCount;
    u32 cvecCapacity;
}entity_registry;


void          nr_er_init(entity_registry* pEr, u32 cvecCount);
void          nr_er_remove_entity(entity_registry* pEr, entity_t entityID);
void          nr_er_push_component(entity_registry* pEr, entity_t entityID, componentID_t componentID, const void* pComponent);
void          nr_er_free(entity_registry* pEr);
void*         nr_er_emplace_component(entity_registry* pEr, entity_t entityID, componentID_t componentID);
void*         nr_er_get_component(entity_registry* pEr, entity_t entityID, componentID_t componentID);
componentID_t nr_er_add_cvec(entity_registry* pEr, u32 componentSize, u32 InitialCount);
entity_t      nr_er_create_entity(entity_registry* pEr);
cvec*         nr_er_get_cvec(entity_registry* pEr, componentID_t componentID);
void          nr_er_serialize(entity_registry* pEr, const char* filePath);
void          nr_er_deserialize(entity_registry *pEr, const char *filePath);



/// UTILITY

void recalculate_sparse(sparse_set *pSs);


/// VECTOR

void* nr_v_growFunc(void* vec, u32 increment, u32 elementSize);
void* nr_v_reserveFunc(void* vec, u32 count, u32 elementSize);

#define nr_v_size(vec) nr_v_raw(vec)[0]
#define nr_v_raw(vec) (((u32 *)(void *)(vec)-2))
#define nr_v_capacity(vec) nr_v_raw(vec)[1]
#define nr_v__need_grow(vec, n) ((vec) == 0 || nr_v_size(vec) + n == nr_v_capacity(vec))
#define nr_v_last(vec) (vec)[nr_v_size(vec)]
#define nr_v_push(vec, element) if(nr_v__need_grow(vec,1)) { (vec) = nr_v_growFunc(vec,1,sizeof(*vec)); } nr_v_last(vec) = element; nr_v_size(vec)++;
#define nr_v_free(vec) FREE(nr_v_raw(vec))
#define nr_v_reserve(vec, n) nr_v_reserveFunc(vec,n,sizeof(*vec))
#define nr_v_pop(vec) nr_v_size(vec)--;