#include <common.h>
#include <dm.h>
#include <serial.h>
#include <asm/global_data.h>
#include <asm/arch/hardware.h>
#include "serial_s3c4510b.h"

DECLARE_GLOBAL_DATA_PTR;

static int serial_flush_input(UART *uart)
{
    volatile u32 tmp;
    while(uart->m_stat.bf.rxReady) {
        tmp = uart->m_rx;
    }
    return 0;
}

static int serial_flush_output(UART *uart)
{
    while(!uart->m_stat.bf.txBufEmpty);
    return 0;
}

static void serial_config_baudrate(UART *uart, unsigned baudrate)
{
    UART_LINE_CTRL ulctrl;
    UART_CTRL      uctrl;
    UART_BAUD_DIV  ubd;

    serial_flush_output(uart);
    serial_flush_input(uart);

    /* control register */
    uctrl.ui = 0x0;
    uctrl.bf.rxMode = 0x1;
    uctrl.bf.rxIrq = 0x0;
    uctrl.bf.txMode = 0x1;
    uctrl.bf.DSR = 0x0;
    uctrl.bf.sendBreak = 0x0;
    uctrl.bf.loopBack = 0x0;
    uart->m_ctrl.ui = uctrl.ui;

    /* line control register */
    ulctrl.ui  = 0x0;
    ulctrl.bf.wordLen   = 0x3; /* 8 bit data */
    ulctrl.bf.nStop     = 0x0; /* 1 stop bit */
    ulctrl.bf.parity    = 0x0; /* no parity */
    ulctrl.bf.clk       = 0x0; /* internal clock */
    ulctrl.bf.infra_red = 0x0; /* no infra_red */
    uart->m_lineCtrl.ui = ulctrl.ui;

    ubd.ui = 0x0;

    /* see table on page 10-15 in SAMSUNG S3C4510B manual */
    /* get correct divisor */
    switch(baudrate) {
        case   1200: ubd.bf.cnt0 = 1301; break;
        case   2400: ubd.bf.cnt0 =  650; break;
        case   4800: ubd.bf.cnt0 =  324; break;
        case   9600: ubd.bf.cnt0 =  162; break;
        case  19200: ubd.bf.cnt0 =   80; break;
        case  38400: ubd.bf.cnt0 =   40; break;
        case  57600: ubd.bf.cnt0 =   26; break;
        case 115200: ubd.bf.cnt0 =   13; break;
    }

    uart->m_baudDiv.ui = ubd.ui;
    uart->m_baudCnt = 0x0;
    uart->m_baudClk = 0x0;
}

static UART *console_uart = (UART*)(CONFIG_CONS_INDEX? UART1_BASE : UART0_BASE);

#ifndef CONFIG_DM_SERIAL
static void s3c4510b_serial_setbrg(void)
{
    serial_config_baudrate(console_uart, gd->baudrate);
}

static int s3c4510b_serial_init(void)
{
    serial_setbrg ();
    return 0;
}

static void s3c4510b_serial_putc(const char c)
{
    while(!console_uart->m_stat.bf.txBufEmpty);
    console_uart->m_tx = c;
    if (c=='\n')
        serial_putc('\r');
}

static void s3c4510b_serial_puts(const char *s)
{
    default_serial_puts(s);
    while (!console_uart->m_stat.bf.txComplete);
    console_uart->m_ctrl.bf.sendBreak = 0;
}

static int s3c4510b_serial_tstc(void)
{
    return console_uart->m_stat.bf.rxReady;
}

static int s3c4510b_serial_getc(void)
{
    int rv;
    for(;;) {
        rv = serial_tstc();
        if (rv) {
            return console_uart->m_rx & 0xFF;
        }
    }
}

static struct serial_device s3c4510b_serial_drv = {
    .name   = "s3c4510b_serial",
    .start  = s3c4510b_serial_init,
    .stop   = NULL,
    .setbrg = s3c4510b_serial_setbrg,
    .putc   = s3c4510b_serial_putc,
    .puts   = s3c4510b_serial_puts,
    .getc   = s3c4510b_serial_getc,
    .tstc   = s3c4510b_serial_tstc,
};

struct serial_device *default_serial_console(void)
{
    return &s3c4510b_serial_drv;
}
#endif

#if defined(CONFIG_DEBUG_UART_S3C4510)
#include <debug_uart.h>

static UART *debug_uart = (UART *)CONFIG_DEBUG_UART_BASE;

static inline void _debug_uart_init(void)
{
    serial_config_baudrate(debug_uart, CONFIG_BAUDRATE);
}

static inline void _debug_uart_putc(int c)
{
    while( !debug_uart->m_stat.bf.txBufEmpty);
    debug_uart->m_tx = c;
}

DEBUG_UART_FUNCS

#endif
