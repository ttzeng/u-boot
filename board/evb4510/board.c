#include <common.h>
#include <asm/io.h>
#include <asm/global_data.h>
#include <asm/arch/hardware.h>
#include <hang.h>

DECLARE_GLOBAL_DATA_PTR;

#if defined(CONFIG_PLATFORM_EVALUATOR7T)
void green_led_on(void)     { setbits(32, REG_IOPDATA, 0x10); }
void green_led_off(void)    { clrbits(32, REG_IOPDATA, 0x10); }
void yellow_led_on(void)    { setbits(32, REG_IOPDATA, 0x20); }
void yellow_led_off(void)   { clrbits(32, REG_IOPDATA, 0x20); }
void red_led_on(void)       { setbits(32, REG_IOPDATA, 0x40); }
void red_led_off(void)      { clrbits(32, REG_IOPDATA, 0x40); }
#elif defined(CONFIG_PLATFORM_NETSTART)
void green_led_on(void)     { setbits(32, REG_IOPDATA, 1 << 16); }
void green_led_off(void)    { clrbits(32, REG_IOPDATA, 1 << 16); }
void red_led_on(void)       { setbits(32, REG_IOPDATA, 1 << 17); }
void red_led_off(void)      { clrbits(32, REG_IOPDATA, 1 << 17); }
#endif

int board_early_init_f(void)
{
#if defined(CONFIG_PLATFORM_EVALUATOR7T)
    /* Set GPIO[4:7] as outputs */
    setbits(32, REG_IOPMODE, 0xf0);
#elif defined(CONFIG_PLATFORM_NETSTART)
    /* Set GPIO[16:17] as outputs */
    setbits(32, REG_IOPMODE, (3 << 16));
    /* Setup access time of I/O banks */
    out_32(REG_EXTACON0, 0x02000e41);
    out_32(REG_EXTACON1, 0x00000200);
#endif
    return 0;
}

int dram_init(void)
{
    gd->ram_size = CFG_SYS_INIT_RAM_SIZE;
    return 0;
}

int board_init(void)
{
    return 0;
}

#ifndef CONFIG_SYSRESET
void reset_cpu(void)
{
#if defined(CONFIG_WDT_NETSTART)
    hang();
#else
    void (*rom_start)(void) = (void (*)(void))CFG_SYS_FLASH_BASE;
    (*rom_start)();
#endif
}
#endif
