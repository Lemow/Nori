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
} Position;
#define SIZE 1024 * 1024
struct a
{
	const int lol;
};

void toBinary(u64 n, char conv[65])
{
	int i = 0;
	while (n != 0)
	{
		conv[i] = (n & 1) + '0';
		n >>= 1;
		i++;
	}
	conv[i] = NULL;

	for (int j = 0; j < i / 2; j++)
	{
		char aux = conv[j];
		conv[j] = conv[i - j - 1];
		conv[i - j - 1] = aux;
	}
}

int main()
{
	DAI_INIT(NULL);
	nori_blk_t blk = nori_malloc(1024 * 1024 * 64);
	double dStartTime = get_time();
	memset(blk.pAddress, 69, blk.uSize);
	double dEndTime = get_time();
	double std_delta = dEndTime - dStartTime;

	dStartTime = get_time();
	nori_memset(blk.pAddress, 69, blk.uSize);
	dEndTime = get_time();

	double nori_delta = dEndTime - dStartTime;
	
	printf("nori_memset: %f\n", nori_delta);
	printf("memset: %f\n", std_delta);

	printf("\n%llu bytes written", blk.uSize);

	CHECK_MEMORY();
	return 0;
}
