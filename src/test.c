#include <stdio.h>
#include <stdlib.h>
#include "entity_registry.h"
#include <sys/time.h>
#include <sys/resource.h>
#include "allocation.h"

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

void wtf(sparse_set* ss)
{
    FREE(ss->dense);
    FREE(ss->sparse);
}

int main()
{
    dai_init();
    cvec cv;
    cv_init(&cv, sizeof(Position), 1);

    Position idk;
    idk.x = 69;
    idk.y = 69;
    idk.z = 69;

    cv_push(&cv, &idk, 0);
    cv_push(&cv, &idk, 1);
    cv_push(&cv, &idk, 2);
    cv_push(&cv, &idk, 3);
    cv_push(&cv, &idk, 4);
    cv_push(&cv, &idk, 5);
    cv_push(&cv, &idk, 6);
    cv_push(&cv, &idk, 7);
    cv_push(&cv, &idk, 8);

    FREE(cv.components);
    wtf(&cv.entitySet);

    return CHECK_MEMORY();
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
        //   printf("i: %d\n",i);
        entities[i] = er_create_entity(&er);

        er_push_component(&er, entities[i], PositionID, &pos);

        if (i == 42)
        {
            er_remove_entity(&er, entities[i]);
        }
    }

    fprintf(stderr, "%s:%d\n\n", __FILE__, __LINE__);
    const cvec *test = &er.cVectors[PositionID];

    CV_FOR(Position, v, test)
    {
        // printf("{%f,%f,%f}\n\n", v->x, v->y, v->z);
    }

    er_free(&er);
    return CHECK_MEMORY();
}