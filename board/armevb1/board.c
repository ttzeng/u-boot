#include <common.h>
#include <asm/io.h>
#include <asm/global_data.h>
#include <asm/arch/hardware.h>

DECLARE_GLOBAL_DATA_PTR;

void green_led_on(void)     { setbits(8, PPI_PC, 0x10); }
void green_led_off(void)    { clrbits(8, PPI_PC, 0x10); }
void yellow_led_on(void)    { setbits(8, PPI_PC, 0x20); }
void yellow_led_off(void)   { clrbits(8, PPI_PC, 0x20); }
void red_led_on(void)       { setbits(8, PPI_PC, 0x40); }
void red_led_off(void)      { clrbits(8, PPI_PC, 0x40); }

int board_early_init_f(void)
{
    /* Set clock sources of UARTs, Counters to internal, disable LCD, PWM, SIR clocks */
    out_32(PCSR, 0);
    /* Set CPU core speed, halt UARTs and Counters */
    out_8 (CCCR,   1);
    out_32(U0CCR,  0);
    out_32(U1CCR,  0);
    out_32(U2CCR,  0);
    out_32(CT0CCR, 0);
    out_32(CT1CCR, 0);
    out_32(CT2CCR, 0);

    /* Set Group A & B in mode 0, PC[4:7] as outputs, others as inputs */
    out_8(PPI_CTLR, PPI_CTLR_MODE | (PPI_CTLR_PA_MODE0 + PPI_CTLR_PA_IN) |
                    (PPI_CTLR_PB_MODE0 + PPI_CTLR_PB_IN + PPI_CTLR_PCL_IN));
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
