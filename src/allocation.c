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

nori_blk_t nori_malloc(size_t uSize)
{
	nori_blk_t retval = {NULL, 0};
	uSize += nori_alignment_distance(uSize, getpagesize());
	retval.pAddress = mmap(NULL, uSize, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	retval.uSize = uSize;
	return retval;
}

int nori_free(nori_blk_t *pBlk)
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
nori_blk_t nori_malloc(size_t uSize)
{
	nori_blk_t retval = {NULL, 0};
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);

	retval.uSize = uSize;
	//retval.uSize += (sysInfo.dwPageSize - (uSize % sysInfo.dwPageSize)) * (uSize % sysInfo.dwPageSize != 0);
	retval.uSize += nori_alignment_distance(uSize, sysInfo.dwPageSize);
	retval.pAddress = VirtualAlloc(NULL, uSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	return retval;
}
#endif

void nori_free(nori_blk_t *blk)
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
	size_t newCap = 2 * (pAv->cap - pAv->begin);
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
	static int i = 0;
	if(i == 250)
	{
		int x = 'd';
	}

	void *retval = malloc(size);

	Allocation *ptr = av_emplace(&debugAllocationInfo.allocations);
	ptr->ptr = retval;
	strcpy(ptr->file, fileName);
	strcpy(ptr->function, functionName);
	ptr->size = size;
	ptr->line = line;
	i++;
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

	for (; iter != end; iter++)
	{
		fprintf(debugAllocationInfo.stream, "%lu bytes @%p allocated in %s:%s, line %d not freed!\n", iter->size, iter->ptr, iter->file, iter->function, iter->line);
	}

	int retval;
	if (av_size(&debugAllocationInfo.allocations) != 0)
		retval = -1;
	else
		retval = 0;

	return retval;
}
#endif

void nori_stack_init(nori_stack_allocator_t *pAlloc, u64 uCapacity)
{
	pAlloc->pStack = pAlloc->pMemory;
	pAlloc->uCapacity = uCapacity;
}

nori_blk_t nori_stack_alloc(nori_stack_allocator_t *pArena, u64 uSize)
{
	nori_blk_t retval = {NULL, 0};
	uSize += nori_alignment_distance(pArena->pStack + uSize, 8);
	if ((pArena->pMemory + pArena->uCapacity) - pArena->pStack >= uSize)
	{
		retval.pAddress = pArena->pStack;
		retval.uSize = uSize;
		pArena->pStack += uSize;
	}
	return retval; 
}

void nori_stack_dealloc(nori_stack_allocator_t *pArena, nori_blk_t memoryBlock)
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
	nori_blk_t blk = nori_malloc(sizeof(nori_stack_allocator_t) + uCapacity);
	REGISTERALLOC(blk.pAddress, blk.uSize);
	nori_stack_allocator_t *retval = blk.pAddress;
	retval->pStack = retval->pMemory;
	retval->uCapacity = blk.uSize;
	return retval;
}

nori_pool_allocator_t *nori_pool_create(u32 uPoolSize, u32 uPoolCount)
{
	nori_blk_t blk = nori_malloc(uPoolSize * uPoolCount + (uPoolCount / 8) + 1 + sizeof(nori_pool_allocator_t));
	REGISTERALLOC(blk.pAddress, blk.uSize);
	nori_pool_allocator_t *retval = blk.pAddress;
	retval->uPoolSize = uPoolSize;
	retval->uPoolCount = uPoolCount;
	retval->uPoolBitmap = retval->pMemory + uPoolSize * uPoolCount;
}

void nori_pool_init(nori_pool_allocator_t *pAlloc, u32 uPoolSize, u32 uPoolCount)
{
	pAlloc->uPoolSize = uPoolSize;
	pAlloc->uPoolCount = uPoolCount;
	pAlloc->uPoolBitmap = pAlloc->pMemory + uPoolSize * uPoolCount;
}

nori_blk_t nori_pool_alloc(nori_pool_allocator_t *pAlloc)
{

	nori_blk_t retval = {NULL, 0};

	for (i32 byte = 0; byte < 1 + pAlloc->uPoolCount / 8; byte++)
	{
		for (i8 bit = 0; bit < 8; bit++)
			if ((pAlloc->uPoolBitmap[byte] & BIT(bit)) == 0)
			{
				retval.pAddress = pAlloc->pMemory + (pAlloc->uPoolSize * (bit + byte * 8));
				retval.uSize = pAlloc->uPoolSize;
				pAlloc->uPoolBitmap[byte] |= BIT(bit);
				return retval;
			}
	}
	return retval;
}

void nori_pool_dealloc(nori_pool_allocator_t *pAlloc, nori_blk_t *pBlk)
{
	u64 poolIndex = ((u8 *)pBlk->pAddress - pAlloc->pMemory) / pAlloc->uPoolSize;
	pAlloc->uPoolBitmap[poolIndex / 8] &= ~BIT(poolIndex % 8);
}

void nori_pool_dealloc_all(nori_pool_allocator_t *pAlloc)
{
	memset(pAlloc->uPoolBitmap, 0, 1 + pAlloc->uPoolCount / 8);
}

int nori_pool_owns(const nori_pool_allocator_t *pAlloc, const nori_blk_t *pBlk)
{
	if (pBlk->pAddress >= pAlloc->pMemory && pBlk->pAddress < pAlloc->pMemory + (pAlloc->uPoolCount + pAlloc->uPoolSize))
	{
		if (((u8 *)pBlk->pAddress - pAlloc->pMemory) % pAlloc->uPoolSize == 0)
			return true;
	}
	return false;
}

int nori_stack_owns(const nori_stack_allocator_t *pAlloc, const nori_blk_t *pBlk)
{
	return (pBlk->pAddress >= pAlloc->pMemory && pBlk->pAddress < pAlloc->pStack);
}

bool nori_is_freelist_block_free(const nori_freelist_block_t* pBlock)
{
    return 1 - ((u64)(pBlock->pNext) & FREELIST_BLOCK_FREE_MASK);
}


i32 nori_memcpy(byte* pSrc, byte* pDst, size_t uBytes)
{
	size_t n = uBytes >> 4;
	switch(uBytes % 16)
	{
		case 0: do { *pSrc = *pDst++;
		case 1: *pSrc = *pDst++;
		case 2: *pSrc = *pDst++;
		case 3: *pSrc = *pDst++;
		case 4: *pSrc = *pDst++;
		case 5: *pSrc = *pDst++;
		case 6: *pSrc = *pDst++;
		case 7: *pSrc = *pDst++;
		case 8: *pSrc = *pDst++;
		case 9: *pSrc = *pDst++;
		case 10: *pSrc = *pDst++;
		case 11: *pSrc = *pDst++;
		case 12: *pSrc = *pDst++;
		case 13: *pSrc = *pDst++;
		case 14: *pSrc = *pDst++;
		case 15: *pSrc = *pDst++;
				}while(--n > 0);
	}
	return 0;
}


i32 nori_memset(byte* pSrc, byte val, size_t uBytes)
{
	size_t n = uBytes >> 4;
	switch(uBytes % 16)
	{
		case 0: do { *pSrc++ = val;
		case 1: *pSrc++ = val;
		case 2: *pSrc++ = val;
		case 3: *pSrc++ = val;
		case 4: *pSrc++ = val;
		case 5: *pSrc++ = val;
		case 6: *pSrc++ = val;
		case 7: *pSrc++ = val;
		case 8: *pSrc++ = val;
		case 9: *pSrc++ = val;
		case 10: *pSrc++ = val;
		case 11: *pSrc++ = val;
		case 12: *pSrc++ = val;
		case 13: *pSrc++ = val;
		case 14: *pSrc++ = val;
		case 15: *pSrc++ = val;
		}while(--n > 0);	
	}
}
