#include <common.h>
#include <asm/io.h>
#include <asm/global_data.h>
#include <asm/arch/hardware.h>

DECLARE_GLOBAL_DATA_PTR;

void enable_caches(void)
{
    s32 i;
    /* disable all cache bits */
    clrbits(32, REG_SYSCFG, 0x3F);
    /* 8KB cache, write enable */
    setbits(32, REG_SYSCFG, CACHE_WRITE_BUFF | CACHE_MODE_01);
    /* clear TAG RAM bits */
    for (i = 0; i < 256; i++)
        out_32(CACHE_TAG_RAM + 4 * i, 0);
    /* clear SET0 RAM */
    for (i = 0; i < 1024; i++)
        out_32(CACHE_SET0_RAM + 4 * i, 0);
    /* clear SET1 RAM */
    for (i = 0; i < 1024; i++)
        out_32(CACHE_SET1_RAM + 4 * i, 0);
    /* enable cache */
    setbits(32, REG_SYSCFG, CACHE_ENABLE);
}
