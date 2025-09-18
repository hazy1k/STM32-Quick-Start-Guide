#include "bsp_init.h"

int main(void)
{
    bsp_init();
    while(1)
    {
        delay_ms(1000);
    }
}
