#include "Nori.h"
#ifdef _WIN32
#include <Windows.h>
#endif

u64 nori_alignment_distance(u64 n, u64 uAlignment)
{
	return (n % uAlignment != 0) * (uAlignment - (n % uAlignment));
}
#ifdef __linux__
#include <sys/mman.h>
#include <unistd.h>

nori_blk nori_malloc(size_t uSize)
{
	nori_blk retval = {NULL, 0};
	uSize += nori_alignment_distance(uSize, getpagesize());
	retval.pAddress = (NULL, uSize, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS);
	retval.uSize = uSize;
	return retval;
}

int nori_free(nori_blk *pBlk)
{
#ifdef DEBUG
	return mprotect(pBlk->pAddress, pBlk->uSize, PROT_NONE);
#else
	return munmap(pBlk->pAddress, pBlk->uSize);
#endif
}

#else
#ifdef _WIN32
//TODO: track memory pages
nori_blk nori_malloc(size_t uSize)
{
	nori_blk retval = {NULL, 0};
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);

	retval.uSize = uSize;
	//retval.uSize += (sysInfo.dwPageSize - (uSize % sysInfo.dwPageSize)) * (uSize % sysInfo.dwPageSize != 0);
	retval.uSize += nori_alignment_distance(uSize, sysInfo.dwPageSize);
	retval.pAddress = VirtualAlloc(NULL, uSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	return retval;
}
#endif

void nori_free(nori_blk *blk)
{
#ifdef DEBUG

	DWORD dwOldProtect;
	VirtualProtect(blk->pAddress, blk->uSize, PAGE_NOACCESS, &dwOldProtect);
#else
	VirtualFree(blk->pAddress, 0, MEM_RELEASE);
#endif
}
#endif

#ifdef DEBUG

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

global DebugAllocateInfo debugAllocationInfo;

void av_grow(alloc_vector *pAv)
{
	size_t newCap = pAv->cap - pAv->begin;
	size_t newEnd = pAv->end - pAv->begin;
	Allocation *newBegin = realloc(pAv->begin, newCap * sizeof(Allocation));
	if (newBegin)
	{
		pAv->begin = newBegin;
		pAv->end = pAv->begin + newEnd;
	}
	pAv->cap = pAv->begin + newCap;
}

void av_init(alloc_vector *pAv)
{
	pAv->begin = malloc(sizeof(Allocation) * 16);
	pAv->end = pAv->begin;
	pAv->cap = pAv->begin + 16;
}

void av_push(alloc_vector *pAv, const Allocation *alloc)
{
	if (pAv->end == pAv->cap)
	{
		av_grow(pAv);
	}

	memcpy(pAv->end, alloc, sizeof(Allocation));
	pAv->end++;
}

bool av_remove(alloc_vector *pAv, void *pToRemove)
{
	Allocation *pToDelete = av_find(pAv, pToRemove);

	if (!pToDelete)
	{
		fprintf(debugAllocationInfo.stream, "%p not found in allocation vector.\n", pToRemove);
		return false;
	}
	else
	{
		pAv->end--;
		memcpy(pToDelete, pAv->end, sizeof(Allocation));
		return true;
	}
}

void av_free(alloc_vector *pAv)
{
	free(pAv->begin);
}

size_t av_size(const alloc_vector *pAv)
{
	return pAv->end - pAv->begin;
}

Allocation *av_emplace(alloc_vector *pAv)
{
	if (pAv->end == pAv->cap)
	{
		av_grow(pAv);
	}

	Allocation *retval = pAv->end;
	pAv->end++;

	return retval;
}

Allocation *av_find(alloc_vector *pAv, const void *ptr)
{
	Allocation *begin = pAv->begin;
	Allocation *end = pAv->end;
	for (; begin != end; begin++)
	{
		if (begin->ptr == ptr)
		{
			return begin;
		}
	}

	return NULL;
}

void dai_init(FILE *stream)
{
	if (stream)
		debugAllocationInfo.stream = stream;
	else
		debugAllocationInfo.stream = stderr;

	av_init(&debugAllocationInfo.allocations);
}

void dregalloc(void *pAddress, size_t size, const char *fileName, const char *functionName, int line)
{
	Allocation *ptr = av_emplace(&debugAllocationInfo.allocations);
	ptr->ptr = pAddress;
	strcpy(ptr->file, fileName);
	strcpy(ptr->function, functionName);
	ptr->size = size;
	ptr->line = line;
}

void *dalloc(size_t size, const char *fileName, const char *functionName, int line)
{
	void *retval = malloc(size);

	Allocation *ptr = av_emplace(&debugAllocationInfo.allocations);
	ptr->ptr = retval;
	strcpy(ptr->file, fileName);
	strcpy(ptr->function, functionName);
	ptr->size = size;
	ptr->line = line;

	return retval;
}

void dfree(void *ptr)
{
	av_remove(&debugAllocationInfo.allocations, ptr);
	free(ptr);
}

void *drealloc(void *ptr, size_t size)
{
	Allocation *pAlloc = av_find(&debugAllocationInfo.allocations, ptr);
	if (!pAlloc)
		return realloc(ptr, size);

	void *retval = realloc(ptr, size);

	if (retval)
		pAlloc->ptr = retval;

	return retval;
}

int checkMemory()
{
	Allocation *iter = debugAllocationInfo.allocations.begin;
	Allocation *end = debugAllocationInfo.allocations.end;

	FILE *pFile = fopen("memoryLog.txt", "w");

	for (; iter != end; iter++)
	{
		fprintf(pFile, "%lu bytes @%p allocated in %s:%s, line %d not freed!\n", iter->size, iter->ptr, iter->file, iter->function, iter->line);
	}

	fclose(pFile);

	int retval;
	if (av_size(&debugAllocationInfo.allocations) != 0)
		retval = -1;
	else
		retval = 0;

	return retval;
}
#endif

nori_blk nori_stack_alloc(nori_stack_allocator_t *pArena, u64 uSize)
{
	nori_blk retval = {NULL, 0};
	uSize += nori_alignment_distance(pArena->pStack + uSize, 8);
	if ((pArena->pMemory + pArena->uCapacity) - pArena->pStack >= uSize)
	{
		retval.pAddress = pArena->pStack;
		retval.uSize = uSize;
		pArena->pStack += uSize;
	}
	return retval;
}

void nori_stack_dealloc(nori_stack_allocator_t *pArena, nori_blk memoryBlock)
{
	if (pArena->pStack == (char *)memoryBlock.pAddress + memoryBlock.uSize)
		pArena->pStack -= memoryBlock.uSize;
}

void nori_stack_dealloc_all(nori_stack_allocator_t *pArena)
{
	pArena->pStack = pArena->pMemory;
}

nori_stack_allocator_t *nori_stack_allocator_create(u64 uCapacity)
{
	nori_blk blk = nori_malloc(sizeof(nori_stack_allocator_t) + uCapacity);
	REGISTERALLOC(blk.pAddress, blk.uSize);
	nori_stack_allocator_t *retval = blk.pAddress;
	retval->pStack = retval->pMemory;
	retval->uCapacity = blk.uSize;
	return retval;
}

nori_pool_allocator_t *nori_pool_create(u32 uPoolSize, u32 uPoolCount)
{
	nori_blk blk = nori_malloc(uPoolSize * uPoolCount + (uPoolCount / 8) + 1 + sizeof(nori_pool_allocator_t));
	REGISTERALLOC(blk.pAddress, blk.uSize);
	nori_pool_allocator_t *retval = blk.pAddress;
	retval->uPoolSize = uPoolSize;
	retval->uPoolCount = uPoolCount;
	retval->uPoolBitmap = retval->pMemory;
}

void nori_pool_init(nori_pool_allocator_t *pAlloc, u32 uPoolSize, u32 uPoolCount)
{
	pAlloc->uPoolSize = uPoolSize;
	pAlloc->uPoolCount = uPoolCount;
	pAlloc->uPoolBitmap = pAlloc->pMemory;
}


//TODO: make a good search for this |
//								   	V
nori_blk nori_pool_alloc(nori_pool_allocator_t *pAlloc, u32 PoolCount)
{
	const u32 uFullPages = PoolCount / 8; // 8 -> 1, 7 -> 0
	const u32 uRemainder = PoolCount % 8; // 8 -> 0, 7 -> 7



}