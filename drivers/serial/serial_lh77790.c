#include <common.h>
#include <asm/io.h>
#include <asm/global_data.h>
#include <serial.h>
#include <asm/arch/hardware.h>

DECLARE_GLOBAL_DATA_PTR;

static void lh77790_config_baudrate(volatile u8* base, unsigned baudrate)
{
    int divisor;
    switch (baudrate) {
        case   9600: divisor = 163; break;
        case  19200: divisor =  81; break;
        case  38400: divisor =  41; break;
        case  57600: divisor =  27; break;
        case 115200:
        default    : divisor =  13;
    }
    out_8(base + OFS_LCR, LCR_PARITY_NONE | LCR_8BIT | LCR_1STOP | LCR_DLAB);
    out_8(base + OFS_DLL, divisor);
    out_8(base + OFS_DLM, divisor >> 8);
    clrbits(8, base + OFS_LCR, LCR_DLAB);
}

static volatile u8 *uart_base = (u8*)UART1_BASE;

#ifndef CONFIG_DM_SERIAL
static void lh77790_serial_setbrg(void)
{
    lh77790_config_baudrate(uart_base, gd->baudrate);
}

static int lh77790_serial_init(void)
{
    uart_base = (u8 *)UART1_BASE;
    out_32(U1CCR, 1);
    serial_setbrg();
    return 0;
}

static int lh77790_serial_tstc(void)
{
    return in_8(uart_base + OFS_LSR) & LSR_DR;
}

static int lh77790_serial_getc(void)
{
    while (!serial_tstc());
    return in_8(uart_base + OFS_RBR);
}

static void lh77790_serial_putc(const char c)
{
    while (!(in_8(uart_base + OFS_LSR) & LSR_TEMT));
    out_8(uart_base + OFS_THR, c);
    if (c == '\n')
        serial_putc('\r');
}

static void lh77790_serial_puts(const char *s)
{
    while (*s)
        serial_putc(*s++);
}

static struct serial_device lh77790_serial_drv = {
    .name   = "lh77790_serial",
    .start  = lh77790_serial_init,
    .stop   = NULL,
    .setbrg = lh77790_serial_setbrg,
    .putc   = lh77790_serial_putc,
    .puts   = lh77790_serial_puts,
    .getc   = lh77790_serial_getc,
    .tstc   = lh77790_serial_tstc,
};

struct serial_device *default_serial_console(void)
{
    return &lh77790_serial_drv;
}
#endif

#if defined(CONFIG_DEBUG_UART_LH77790)
#include <debug_uart.h>

static volatile u8 *debug_uart_base = (volatile u8*)CONFIG_DEBUG_UART_BASE;

static inline void _debug_uart_init(void)
{
    out_32(U1CCR, 1);
    lh77790_config_baudrate(debug_uart_base, CONFIG_BAUDRATE);
}

static inline void _debug_uart_putc(int c)
{
    while (!(in_8(debug_uart_base + OFS_LSR) & LSR_TEMT));
    out_8(debug_uart_base + OFS_THR, c);
}

DEBUG_UART_FUNCS

#endif
