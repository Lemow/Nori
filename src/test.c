#include <stdio.h>
#include <stdlib.h>
#include "entity_registry.h"
#include <sys/time.h>
#include <sys/resource.h>

double get_time()
{
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec + t.tv_usec * 1e-6;
}

typedef struct Position
{
    float x, y, z;
} Position;

int main()
{

    sparse_set lol;

    ss_init(&lol, 32);

    for(int i = 0; i < 10000; i++)
    {
        ss_insert(&lol, (u32)i);
    }

    ss_free(&lol);
    return 0;
    entity_registry er;

    er_init(&er, 2);

    u32 PositionID = er_add_cvec(&er, sizeof(Position), 32);

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
        fprintf(stderr, "After createEntity, i = %d\n", i);

        er_push_component(&er, entities[i], PositionID, &pos);
        fprintf(stderr, "After push, i = %d\n", i);

        if (i == 42)
        {
            er_remove_entity(&er, entities[i]);
        }
            fprintf(stderr, "After If, i = %d\n",i);
    }

    cvec *test = &er.cVectors[PositionID];

    const u32 *pID = test->entitySet.sparse;

    CV_FOR(Position, v, test)
    {
        printf("ID: %u\nPOS:{%f,%f,%f}\n\n", *pID, v->x, v->y, v->z);
        pID++;
    }

    er_free(&er);
    return 0;
}