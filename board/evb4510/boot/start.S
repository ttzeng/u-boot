#include <config.h>
#include <asm-offsets.h>
#include <linux/linkage.h>
#include <asm/assembler.h>
#include <system-constants.h>

.macro CLEAR_BSS
    ldr     r0, =__bss_start
    ldr     r1, =__bss_end
    mov     r2, #0x00000000
clbss_l:    cmp r0, r1
    strlo   r2, [r0]
    addlo   r0, r0, #4
    blo clbss_l
.endm

.globl _start
_start:
    mrs     r0, cpsr
    bic     r0, r0, #0x1f
    orr     r0, r0, #0xd3
    msr     cpsr, r0

    bl      lowlevel_init
#if defined(CONFIG_DEBUG_UART) && CONFIG_IS_ENABLED(SERIAL)
    bl      debug_uart_init
#endif
    CLEAR_BSS
    b       main
