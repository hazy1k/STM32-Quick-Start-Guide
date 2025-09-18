#include "chipid.h"

uint32_t Unique_ID[3];

void Get_Unique_ID(void)
{
    Unique_ID[0] = *(__IO uint32_t*)(0x1FFFF7F0);
    Unique_ID[1] = *(__IO uint32_t*)(0x1FFFF7EC);
    Unique_ID[2] = *(__IO uint32_t*)(0x1FFFF7E8);
}
