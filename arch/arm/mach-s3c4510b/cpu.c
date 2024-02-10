#include <common.h>
#include <asm/io.h>
#include <asm/global_data.h>
#include <asm/arch/hardware.h>

DECLARE_GLOBAL_DATA_PTR;

void invalidate_icache_all(void)
{
    int i;
    /* clear TAG RAM bits */
    for (i = 0; i < 256; i++)
        out_32(CACHE_TAG_RAM + 4 * i, 0);
}

static int unified_cache_status(void)
{
    return (in_32(REG_SYSCFG) & CACHE_ENABLE);
}

static void unified_cache_enable(void)
{
    /* disable cache */
    clrbits(32, REG_SYSCFG, CACHE_MODE | CACHE_ENABLE);
    /* 8KB cache, write enable */
    setbits(32, REG_SYSCFG, CACHE_WRITE_BUFF | CACHE_MODE_01);
    invalidate_icache_all();
    /* enable cache */
    setbits(32, REG_SYSCFG, CACHE_ENABLE);
}

static void unified_cache_disable(void)
{
    /* disable all cache bits */
    clrbits(32, REG_SYSCFG, CACHE_ENABLE);
}

int icache_status(void)
{
    return unified_cache_status();
}

void icache_enable(void)
{
    unified_cache_enable();
}

void icache_disable(void)
{
    unified_cache_disable();
}

int dcache_status(void)
{
    return unified_cache_status();
}

void dcache_enable(void)
{
    unified_cache_enable();
}

void dcache_disable(void)
{
    unified_cache_disable();
}

void enable_caches(void)
{
    unified_cache_enable();
}
