#include "Nori.h"
#include <stdio.h>

#ifdef _WIN32
#include <Windows.h>
double get_time()
{
	LARGE_INTEGER t, f;
	QueryPerformanceCounter(&t);
	QueryPerformanceFrequency(&f);
	return (double)t.QuadPart / (double)f.QuadPart;
}
#else
#include <sys/time.h>
#include <sys/resource.h>
double get_time()
{
	struct timeval t;
	gettimeofday(&t, NULL);
	return t.tv_sec + t.tv_usec * 1e-6;
}
#endif
typedef struct Position
{
	float x, y, z;
}Position;
#define SIZE 1024 * 1024
struct a
{
	const int lol;
};




int main()
{

	DAI_INIT();


	nori_stack_allocator(stackAllocator, 128 * sizeof(int));


	nori_blk blk = nori_stack_alloc(&stackAllocator, 4 * sizeof(int));

	int* pInts = blk.pAddress;

	pInts[0] = 2;
	pInts[1] = 3;
	pInts[2] = 4;
	pInts[3] = 5;

	printf("%d\n%d\n%d\n%d\n", pInts[0], pInts[1], pInts[2], pInts[3]);

	return 0;
}