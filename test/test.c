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
    entity_registry er;
    er_init(&er, 2);

    u32 PositionID = er_add_cvec(&er, sizeof(Position), 1024);

    u32 entity = er_create_entity(&er);

    Position *ptr = er_emplace_component(&er, entity, PositionID);

    ptr->x = 69.0f;
    ptr->y = 420.0f;
    ptr->z = 1337.0f;

    ptr = NULL;

    ptr = er_get_component(&er, entity, PositionID);

    printf("%f %f %f\n\n", ptr->x, ptr->y, ptr->z);

    Position pos = *ptr;
    Position sexy = {69.0f,420.0f,1337.0f};
    u32 entities[1024];
    for (int i = 0; i < 1024; i++)
    {
        entities[i] = er_create_entity(&er);

            pos.x = i % 3;
            pos.y = (i % 3) + 1;
            pos.z = (i % 3) + 2;
            er_push_component(&er, entities[i], PositionID, &pos);
    }

    ptr = er_get_component(&er, 69, PositionID);
    *ptr = sexy;
    
    er_serialize(&er, "data.save");
    er_free(&er);
    er_deserialize(&er, "data.save");

    const cvec *test = &er.cVectors[PositionID];
    const u32* pID = er.cVectors[PositionID].entitySet.dense;
    u32 choose = 0;
    for(int i = 0; i < 100; i++)
    {
        choose = rand()%1024;
        ptr = er_get_component(&er, entities[choose],PositionID);
        printf("@%u:{%f,%f,%f}\n\n",choose, ptr->x, ptr->y, ptr->z);
    }

    er_free(&er);
    return CHECK_MEMORY();
}