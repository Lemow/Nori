#include "Nori.h"
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
   
    int* test = NULL;
    v_push(test, 2);
    v_push(test, 69);
    v_push(test, 78);
    v_push(test, 12);
    v_push(test, 54);
    v_push(test, 12);


    int n = v_size(test);
   
    for (int i = 0; i < n; i++)
    {
        printf("%d ", test[i]);
    }

    return CHECK_MEMORY();
}