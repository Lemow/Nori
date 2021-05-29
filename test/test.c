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
    float x,y,z;
}Position;
#define SIZE 1024 * 1024
int main()
{
    DAI_INIT();

    nori_arena* pArena = nori_arena_create(1024 * 4);
    Position* pPosition = nori_arena_alloc(pArena,sizeof(Position) * 100);

    for (int i = 0; i < 100; i++)
    {
        pPosition[i].x = i;
        pPosition[i].y = 100 - i;
    }

    Position* test = NULL;
    Position lmao = {69.0f,420.0f,1337.0f};
    v_push(test, lmao);

    for(int i = 0; i < 128; i++)
    {
        lmao.x = lmao.y = lmao.z = (f32)i;
        v_push(test, lmao);
    }


    int n = v_size(test);
    printf("N: %d\n", n);
    for (int i = 0; i < n; i++)
    {
        printf("@%d:{%f, %f, %f}\n", i, test[i].x, test[i].y,test[i].z);
    }

    v_free(test);
    return 0;
}