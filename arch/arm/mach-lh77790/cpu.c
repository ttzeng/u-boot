#include <common.h>
#include <asm/io.h>
#include <asm/arch/hardware.h>
#include <asm/global_data.h>

DECLARE_GLOBAL_DATA_PTR;

void enable_caches(void)
{
    out_8(CCR, CCR_EN);
}

void flush_cache(unsigned long start, unsigned long size)
{
    out_8(CCR, CCR_FLUSH);
    out_8(CCR, CCR_EN);
}

void reset_cpu(ulong addr)
{
    out_8(WDCTLR, WDCTLR_EN | WDCTLR_RSP_SYS_RST | WDCTLR_TOP21);
}
