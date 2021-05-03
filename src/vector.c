#include "Nori.h"

void *v_growFunc(void *vec, u32 increment, u32 elementSize)
{
    u32 newCap;
    u32 *ptr;
    if (vec)
    {
        u32 cap_double = 2 * v_capacity(vec);
        u32 minNeeded = v_size(vec) + increment;
        newCap = max(cap_double, minNeeded);
        ptr = REALLOC(v_raw(vec), newCap * elementSize + sizeof(u32) * 2);
        if (ptr)
        {
            ptr[1] = newCap;
            return ptr + 2;
        }
    }
    else
    {
        newCap = increment;
        ptr = MALLOC(newCap * elementSize + sizeof(u32) * 2);
        if (ptr)
        {
            ptr[0] = 0;
            ptr[1] = newCap;
            return ptr + 2;
        }
    }
    fprintf(stderr, "Out of memory in "__FILE__"\n");
    return NULL;
}

void *v_reserveFunc(void *vec, u32 count, u32 elementSize)
{
    if (vec)
    {
        if (count <= v_capacity(vec))
            return vec;
        else
        {
            u32 *ptr = REALLOC(v_raw(vec), count * elementSize + sizeof(u32) * 2);
            if (ptr)
            {
                ptr[0] = 0;
                ptr[1] = count * elementSize;
                return ptr + 2;
            }
        }
    }
    else
    {
        u32 *ptr = MALLOC(count * elementSize + sizeof(u32) * 2);
        if (ptr)
        {
            ptr[0] = 0;
            ptr[1] = count * elementSize;
            return ptr + 2;
        }
    }
    fprintf(stderr, "Out of memory in "__FILE__
                    "\n");
    return NULL;
}
// nice