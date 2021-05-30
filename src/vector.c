#include "Nori.h"


void *nr_v_growFunc(void *vec, u32 increment, u32 elementSize)
{
    u32 newCap;
    u32 *ptr; 
    if (vec)
    {
        const u32 cap_double = 2 * nr_v_capacity(vec);
        u32 minNeeded = nr_v_size(vec) + increment;
        newCap = max(cap_double, minNeeded);
        ptr = REALLOC(nr_v_raw(vec), newCap * elementSize + sizeof(u32) * 2);
        if (ptr)
        {
            ptr[1] = newCap;
            return ptr + 2;
        }
    }
    else
    {
        ptr = MALLOC(increment * elementSize);
        ptr[0] = 0;
        ptr[1] = increment;
        return ptr + 2;
    }
}


void *nr_v_reserveFunc(void *vec, u32 count, u32 elementSize)
{
    if (nr_v_size(vec) + count >= nr_v_capacity(vec))
    {

        if (count <= nr_v_capacity(vec))
            return vec;
        else
        {
            u32 *ptr = REALLOC(nr_v_raw(vec), count * elementSize + sizeof(u32) * 2);
            if (ptr)
            {
                ptr[0] = 0;
                ptr[1] = count * elementSize;
                return ptr + 2;
            }
        }
    }

    
}

