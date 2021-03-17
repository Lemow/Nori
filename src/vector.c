#include "Nori.h"


void* v_growFunc(void* vec, u32 increment, u32 elementSize)
{
    u32 newCap;
    if(vec)
    {
        u32 cap_double = 2 * v_capacity(vec);
        u32 minNeeded = v_size(vec) + increment;
        newCap = max(cap_double,minNeeded);
    } else
    {
        newCap = increment;
    }
    u32* ptr = REALLOC(vec ? v_raw(vec) : 0, newCap * elementSize + sizeof(u32) * 2);

    if(ptr)
    {
        if(!vec)
        {
            ptr[0] = 0;
        }
        ptr[1] = newCap;
        return ptr+2; 
    } else
    {
        fprintf(stderr, "Out of memory in "__FILE__ "\n");
        return NULL;
    }
}