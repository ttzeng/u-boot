#include <common.h>
#include <asm/io.h>
#include <asm/global_data.h>
#include <asm/arch/hardware.h>

#define TICKS_PER_MS        (CONFIG_SYS_CLK_FREQ / 1000 - 2)

DECLARE_GLOBAL_DATA_PTR;

static uint32_t uptime_msec = 0;

int timer_init(void)
{
    /* Timer0 clock = XCLK */
    out_32(CT0CCR, 1 << 0);
    /* Set Timer0 in binary mode, Mode 0, read/write LSB followed by MSB */
    out_8(CT_CWR, CT_CWR_SC0 | CT_CWR_WORD | CT_CWR_MODE0);
    /* Set Timer0 Gate to high */
    setbits_32(IOCR, IOCR_CT0G_HI);
    /* Set CR of Timer0 */
    out_8(CT_CNTR0, TICKS_PER_MS & 0xff);
    out_8(CT_CNTR0, TICKS_PER_MS >> 8);
    return 0;
}

uint64_t get_ticks(void)
{
    uint64_t ticks = uptime_msec;
    ticks *= TICKS_PER_MS;
    /* Check the OUT pin state of Timer0 by read back its status */
    out_8(CT_CWR, CT_CWR_RDBACK | CT_CWR_RDBACK_COUNT | CT_CWR_RDBACK_CNT0);
    if (in_8(CT_CNTR0) & CT_STATUS_OUT) {
        /* The OUT pin goes HIGH on Timer0 expired */
        uptime_msec++;
        ticks += TICKS_PER_MS;
        /* Write the CR to reset the OUT pin to LOW */
        out_8(CT_CNTR0, TICKS_PER_MS & 0xff);
        out_8(CT_CNTR0, TICKS_PER_MS >> 8);
    } else {
        uint8_t msb, lsb;
        lsb = in_8(CT_CNTR0), msb = in_8(CT_CNTR0);
        ticks += ((uint32_t)msb << 8 | lsb);
    }
    return ticks;
}

unsigned long get_timer(unsigned long base)
{
    get_ticks();
    return uptime_msec - base;
}

void __udelay(unsigned long usec)
{
    ulong start = get_timer(0);
    uint32_t expire_ms = 1;
    while (usec >= 1000)
        expire_ms++, usec -= 1000;
    while (get_timer(start) < expire_ms);
}
