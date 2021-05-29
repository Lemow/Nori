#include "Nori.h"
#ifdef _WIN32
#include <Windows.h>
#endif

internal_func void* nori_alloc(size_t uSize)
{
	return MALLOC(uSize);
}

internal_func u64 nori_alignment_distance(void* pAddress, u64 uAlignment)
{
	return ((u64)pAddress % uAlignment != 0) * (uAlignment - ((u64)pAddress % uAlignment));
}


#ifdef _WIN32
//TODO: track memory pages
internal_func void* nori_page_alloc(size_t uSize, size_t* pAllocatedSize)
{
	if (pAllocatedSize)
	{
		SYSTEM_INFO sysInfo;
		GetSystemInfo(&sysInfo);

		*pAllocatedSize = uSize;
		*pAllocatedSize += (sysInfo.dwPageSize - (uSize % sysInfo.dwPageSize)) * (uSize % sysInfo.dwPageSize != 0);
	}

	VirtualAlloc(NULL, uSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
}

internal_func void nori_page_free(void* pPage, size_t uPageSize)
{
#ifdef DEBUG

	DWORD dwOldProtect;
	VirtualProtect(pPage, uPageSize, PAGE_NOACCESS, &dwOldProtect);
#else
	VirtualFree(pPage, 0, MEM_RELEASE);
#endif
}
#endif

#ifdef DEBUG

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

DebugAllocateInfo debugAllocationInfo;

void av_grow(alloc_vector* pAv)
{
	size_t newCap = pAv->cap - pAv->begin;
	size_t newEnd = pAv->end - pAv->begin;
	Allocation* newBegin = realloc(pAv->begin, newCap * sizeof(Allocation));
	if (newBegin)
	{
		pAv->begin = newBegin;
		pAv->end = pAv->begin + newEnd;
	}
	pAv->cap = pAv->begin + newCap;
}

void av_init(alloc_vector* pAv)
{
	pAv->begin = malloc(sizeof(Allocation) * 16);
	pAv->end = pAv->begin;
	pAv->cap = pAv->begin + 16;
}

void av_push(alloc_vector* pAv, const Allocation* alloc)
{
	if (pAv->end == pAv->cap)
	{
		av_grow(pAv);
	}

	memcpy(pAv->end, alloc, sizeof(Allocation));
	pAv->end++;
}

bool av_remove(alloc_vector* pAv, void* pToRemove)
{
	Allocation* pToDelete = av_find(pAv, pToRemove);

	if (!pToDelete)
	{
		fprintf(stderr, "%p not found in allocation vector.\n", pToRemove);
		return false;
	}
	else
	{
		pAv->end--;
		memcpy(pToDelete, pAv->end, sizeof(Allocation));
		return true;
	}
}

void av_free(alloc_vector* pAv)
{
	free(pAv->begin);
}

size_t av_size(const alloc_vector* pAv)
{
	return pAv->end - pAv->begin;
}

Allocation* av_emplace(alloc_vector* pAv)
{
	if (pAv->end == pAv->cap)
	{
		av_grow(pAv);
	}

	Allocation* retval = pAv->end;
	pAv->end++;

	return retval;
}

Allocation* av_find(alloc_vector* pAv, const void* ptr)
{
	Allocation* begin = pAv->begin;
	Allocation* end = pAv->end;
	for (; begin != end; begin++)
	{
		if (begin->ptr == ptr)
		{
			return begin;
		}
	}

	return NULL;
}

void dai_init()
{
	av_init(&debugAllocationInfo.allocations);
}

void* dalloc(size_t size, const char* fileName, const char* functionName, int line)
{
	void* retval = malloc(size);

	Allocation* ptr = av_emplace(&debugAllocationInfo.allocations);
	ptr->ptr = retval;
	strcpy(ptr->file, fileName);
	strcpy(ptr->function, functionName);
	ptr->size = size;
	ptr->line = line;

	return retval;
}


void dfree(void* ptr)
{
	av_remove(&debugAllocationInfo.allocations, ptr);
	free(ptr);
}

void* drealloc(void* ptr, size_t size)
{
	Allocation* pAlloc = av_find(&debugAllocationInfo.allocations, ptr);
	if (!pAlloc)
		return realloc(ptr, size);

	void* retval = realloc(ptr, size);

	if (retval)
		pAlloc->ptr = retval;

	return retval;
}

int checkMemory()
{
	Allocation* iter = debugAllocationInfo.allocations.begin;
	Allocation* end = debugAllocationInfo.allocations.end;

	for (; iter != end; iter++)
	{
		fprintf(stderr, "%lu bytes @%p allocated in %s:%s, line %d not freed!\n", iter->size, iter->ptr, iter->file, iter->function, iter->line);
	}

	int retval;
	if (av_size(&debugAllocationInfo.allocations) != 0)
		retval = -1;
	else
		retval = 0;
	av_free(&debugAllocationInfo.allocations);
	return retval;
}

#endif


nori_arena* nori_arena_create(u64 uCapacity)
{
	nori_arena* pArena = nori_page_alloc(uCapacity + sizeof(nori_arena), &uCapacity);
	pArena->uCapacity = uCapacity - sizeof(nori_arena);
	pArena->pStack = pArena->pData;
	return pArena;
}

void nori_arena_free(nori_arena* pArena)
{
#ifdef DEBUG
	nori_page_free(pArena, sizeof(nori_arena) + pArena->uCapacity);
#else
	nori_page_free(pArena, 0);
#endif
}

void* nori_arena_alloc(nori_arena* pArena, u64 uBytes)
{
	uBytes += nori_alignment_distance(pArena->pStack + uBytes, 8);
	if ((pArena->pData + pArena->uCapacity) - pArena->pStack < uBytes)
	{
		void* retval = pArena->pStack;
		pArena->pStack += uBytes;
		return retval;
	}
	else
	{
		nori_freeblock* pLastBlock = NULL;
		nori_freeblock* pFreeBlock = pArena->pFreeListHead;
		void* retval = NULL;
		while (pFreeBlock)
		{
			if (pFreeBlock->size <= uBytes)
			{
				retval = pFreeBlock;
				break;
			}

			pLastBlock = pFreeBlock;
			pFreeBlock = pFreeBlock->pNext;
		}

		if (retval)
		{
			if (pLastBlock)
				pLastBlock->pNext = pFreeBlock->pNext;
			else
				pArena->pFreeListHead = pFreeBlock->pNext;
		}

		return retval;
	}
}
