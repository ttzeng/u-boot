#include <common.h>
#include <dm.h>
#include <asm/io.h>
#include <asm/arch/hardware.h>
#include <wdt.h>

static int ds1819_wdt_start(struct udevice *dev, u64 timeout_ms, ulong flags)
{
    /* The DS1819 watchdog timer is disabled by floating the ST/ input,
       which is dirven by the GPIO[5] on the NETStart! boatrd. To enable
       the watchdog timer function, set the pin to output mode, the board
       will be reset in 1.12 seconnd. */
    setbits(32, REG_IOPMODE, (1 << 5));
    setbits(32, REG_IOPDATA, (1 << 5));
    return 0;
}

static int ds1819_wdt_stop(struct udevice *dev)
{
    /* Set the GPIO[5] to input mode to floating the ST/ input */
    clrbits(32, REG_IOPMODE, (1 << 5));
    return 0;
}

static int ds1819_wdt_reset(struct udevice *dev)
{
    /* Output a pulse on the ST/ input */
    clrbits(32, REG_IOPDATA, (1 << 5));
    setbits(32, REG_IOPDATA, (1 << 5));
    return 0;
}

static const struct wdt_ops ds1819_wdt_ops = {
    .start = ds1819_wdt_start,
    .stop  = ds1819_wdt_stop,
    .reset = ds1819_wdt_reset,
};

U_BOOT_DRIVER(ds1819_wdt) = {
    .name = "ds1819_wdt",
    .id = UCLASS_WDT,
    .ops = &ds1819_wdt_ops,
};

#if !CONFIG_IS_ENABLED(OF_CONTROL)
U_BOOT_DRVINFO(ds1819_wdt) = {
    .name = "ds1819_wdt",
};
#endif
