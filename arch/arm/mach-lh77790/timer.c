#include <common.h>
#include <asm/io.h>
#include <asm/global_data.h>
#include <asm/arch/hardware.h>

#define TICKS_PER_MS        750

DECLARE_GLOBAL_DATA_PTR;

static uint32_t uptime_msec = 0;

int timer_init(void)
{
    /* Timer0 clock = XCLK/32, 1.28 us/tick = ~1 us */
    out_32(CT0CCR, 1 << 5);
    /* Set Timer0 in binary mode, Mode 2, read/write LSB followed by MSB */
    out_8(CT_CWR, CT_CWR_SC0 | CT_CWR_WORD | CT_CWR_MODE2);
    /* Set Timer0 Gate to high */
    out_32(IOCR, IOCR_CT0G_HI);
    /* Timer0 reload every ~1 msec */
    out_8(CT_CNTR0, TICKS_PER_MS & 0xff);
    out_8(CT_CNTR0, TICKS_PER_MS >> 8);
    return 0;
}

uint64_t get_ticks(void)
{
    static uint32_t last = 0;
    uint32_t count;
    uint8_t msb, lsb;
    /* Issue CounterLatch command to latch the counter value to Output Latches */
    out_8(CT_CWR, CT_CWR_SC0 | CT_CWR_LATCH);
    lsb = in_8(CT_CNTR0), msb = in_8(CT_CNTR0);
    count = TICKS_PER_MS - ((uint32_t)msb << 8 | lsb);
    if (count < last) uptime_msec++;
    last = count;
    return uptime_msec * TICKS_PER_MS + count;
}

unsigned long get_timer(unsigned long base)
{
    get_ticks();
    return uptime_msec - base;
}

void __udelay(unsigned long usec)
{
    uint32_t expire = get_timer(0) + (usec * CONFIG_SYS_HZ / 1000000);
    while (get_timer(0) < expire);
}
