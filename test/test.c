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
    float x, y, z;
} Position;

void wtf(entity_registry* er, u32 s)
{
    er->cVectors = MALLOC(s * sizeof(cvec));
}

int main()
{
    DAI_INIT();

    //entity_registry test1;
    //er_init(&test1,2);
    //printf("er.cVectors = \t%p\n", test1.cVectors);
    //er_free(&test1);
    //return CHECK_MEMORY();
    entity_registry er;

    er_init(&er, 2);

    u32 PositionID = er_add_cvec(&er, sizeof(Position), 1);

    u32 entity = er_create_entity(&er);

    Position *ptr = er_emplace_component(&er, entity, PositionID);

    ptr->x = 69.0f;
    ptr->y = 420.0f;
    ptr->z = 1337.0f;

    ptr = NULL;

    ptr = er_get_component(&er, entity, PositionID);

    printf("%f %f %f\n\n", ptr->x, ptr->y, ptr->z);

    Position pos = *ptr;
    u32 entities[69];
    for (int i = 0; i < 69; i++)
    {
        entities[i] = er_create_entity(&er);
        pos.x = i % 3;
        pos.y = (i % 3) + 1;
        pos.z = (i % 3) + 2;
        er_push_component(&er, entities[i], PositionID, &pos);

        if (i == 42)
        {
            er_remove_entity(&er, entities[i]);
        }
    }

    const cvec *test = &er.cVectors[PositionID];
    const u32* pID = er.cVectors[PositionID].entitySet.dense;
    CV_FOR(Position, v, test)
    {
         printf("@%u:{%f,%f,%f}\n\n",*pID, v->x, v->y, v->z);
         pID++;
    }

    er_free(&er);
    return CHECK_MEMORY();
}