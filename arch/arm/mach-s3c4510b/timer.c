#include <common.h>
#include <asm/io.h>
#include <asm/global_data.h>
#include <asm/arch/hardware.h>

DECLARE_GLOBAL_DATA_PTR;

int timer_init(void)
{
    // Stop timer 0
    clrbits(32, REG_TMOD, TM0_RUN);
    // Configure for interval mode
    clrbits(32, REG_TMOD, TM0_TOGGLE);
    // Load Timer data register with count down value.
    out_32(REG_TDATA0, -1);
    // Start timer
    setbits(32, REG_TMOD, TM0_RUN);

    return 0;
}

uint64_t get_ticks(void)
{
    static uint64_t ticks = 0;
    static uint32_t last = 0;
    uint32_t now = ~in_32(REG_TCNT0);
    ticks += (now > last)? (now - last) : (0xffffffff - last + now);
    last = now;
    return ticks;
}
