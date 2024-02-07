#include <asm-offsets.h>
#include <config.h>
#include <version.h>
#include <system-constants.h>
#include <asm/arch/hardware.h>

.globl lowlevel_init

lowlevel_init:
    /* configure the on chip memory as 8KB SRAM */
    ldr     r1, =REG_SYSCFG
    ldr     r0, [r1]
    bic     r0, r0, #0x32
    orr     r0, r0, #0x20
#if defined(CONFIG_PLATFORM_NETSTART)
    orr     r0, r0, #0x80000000 /* SDRAM interface for 4 DRAM banks */
#endif
    str     r0, [r1]
    /* use the on chip SRAM as initial stack as well */
    ldr     r0, onchip_sram_base
    add     sp, r0, #0x1000
    /* store a short program in the on chip SRAM for remapping memory */
    ldr     r1, =0xe8801ffe     /* stmia r0, {r1-r12} */
    ldr     r2, =0xe1a0f00e     /* mov pc, lr         */
    stm     r0, {r1,r2}

    ldr     r0, =lowlevel_init
    ldr     r1, =__image_copy_start
    ldr     r2, =__image_copy_end
    adr     r3, lowlevel_init
    sub     r3, r3, r0  /* offset b/w the running code and its actual addr */
    sub     lr, lr, r3  /* return addr of lowlevel_init relative to _start */
    add     lr, lr, r1  /* return addr of lowlevel_init after relocation   */

    push    {r1,r2,r3,lr}
    adr     r0, memory_map_data
    ldmia   r0, {r1-r12}
    ldr     r0, =REG_EXTDBWTH
    adr     lr, copy_code
    orr     lr, lr, #0x01000000     /* return the the same offset in Flash */
    ldr     pc, onchip_sram_base
copy_code:
    pop     {r1,r2,r3,lr}
    add     r0, r1, r3
    orr     r0, r0, #0x01000000
1:  ldmia   r0!, {r3-r10}
    stmia   r1!, {r3-r10}
    cmp     r1, r2
    blo     1b
    mov     pc, lr

onchip_sram_base:
    .long   SRAM_BASE

/************************************************************************
 * Below are twelve 32-bit values for the twelve memory registers of
 * the system manager, starting with register REG_EXTDBWTH.
 ***********************************************************************/
memory_map_data:
#if defined(CONFIG_PLATFORM_EVALUATOR7T)
	.long 0x0000003e     /* memory widths */
	.long 0x12040060     /* Bank0 2MB FLASH	 @ 0x01000000 */
	.long 0x00400060     /* Bank1 256KB SRAM @ 0x00000000 */
	.long 0x00801060     /* Bank2 256KB SRAM @ 0x00040000 */
	.long 0x00000000
	.long 0x00000000
	.long 0x00000000
	.long 0x00000000
	.long 0x00000000
	.long 0x00000000
	.long 0x00000000
	.long 0x000083fd     /* enable memory */
#elif defined(CONFIG_PLATFORM_NETSTART)
	.long 0x01503f02     /* memory widths */
	.long 0x12040060     /* Bank0 2MB FLASH	    @ 0x01000000 */
	.long 0x00000000
	.long 0x00000000
	.long 0x00000000
	.long 0x26198060     /* Bank4 64KB reserved @ 0x02600000 */
	.long 0x281a0014     /* Bank5 64KB reserved @ 0x02800000 */
	.long 0x10000301     /* DRAM 16MB SRAM      @ 0x00000000 */
	.long 0x00000000
	.long 0x00000000
	.long 0x00000000
	.long 0x9c298360     /* enable memory, I/O 16KB x 4 @ 0x03600000 */
#endif
