#include <asm-offsets.h>
#include <config.h>
#include <version.h>
#include <system-constants.h>
#include <asm/arch/hardware.h>

#ifndef CONFIG_SKIP_LOWLEVEL_INIT
#ifndef CONFIG_SKIP_LOWLEVEL_INIT_ONLY

.globl lowlevel_init

lowlevel_init:
    /* configure the on chip memory as 8KB SRAM */
    ldr     r1, =REG_SYSCFG
    ldr     r0, [r1]
    bic     r0, r0, #0x32
    orr     r0, r0, #0x20
    str     r0, [r1]
    /* use the on chip SRAM as initial stack as well */
    ldr     r0, onchip_sram_base
    add     sp, r0, #0x1000
    /* store a short program in the on chip SRAM for remapping memory */
#ifndef CONFIG_MONITOR_IS_IN_RAM
    ldr     r1, =0xe8801ffe     /* stmia r0, {r1-r12} */
    ldr     r2, =0xe1a0f00e     /* mov pc, lr         */
    stm     r0, {r1,r2}
#else
    ldr     r1, =0xe8b007f8     /* copy: ldmia r0!, {r3-r10} */
    ldr     r2, =0xe8a107f8     /*       stmia r1!, {r3-r10} */
    ldr     r3, =0xe1510002     /*       cmp r1, r2          */
    ldr     r4, =0x3afffffb     /*       bcc loop            */
    ldr     r5, =0xe1a0f00e     /*       mov pc, lr          */
    stm     r0, {r1-r5}
#endif

    ldr     r0, =lowlevel_init
    ldr     r1, =_start
    ldr     r2, =_image_binary_end
    adr     r3, lowlevel_init
    sub     r3, r3, r0  /* offset b/w the running code and its actual addr */
    sub     ip, ip, r3  /* return addr of cpu_init_crit relative to _start */
    add     ip, ip, r1  /* return addr of cpu_init_crit after relocation   */
    sub     lr, lr, r3  /* return addr of lowlevel_init relative to _start */
    add     lr, lr, r1  /* return addr of lowlevel_init after relocation   */
    push    {r1,r2,r3,ip,lr}

#ifndef CONFIG_MONITOR_IS_IN_RAM
    adr     r0, memory_map_data
    ldmia   r0, {r1-r12}
    ldr     r0, =REG_EXTDBWTH
    adr     lr, copy_code
    orr     lr, lr, #0x01000000     /* return the the same offset in Flash */
    ldr     pc, onchip_sram_base
copy_code:
    pop     {r1,r2,r3,ip,lr}
    add     r0, r1, r3
    orr     r0, r0, #0x01000000
1:  ldmia   r0!, {r3-r10}
    stmia   r1!, {r3-r10}
    cmp     r1, r2
    blo     1b
    mov     pc, lr
#else
    pop     {r1,r2,r3,ip,lr}
    add     r0, r1, r3
    ldr     pc, onchip_sram_base
#endif

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
#else
	.long 0x00f03005     /* memory widths */
	.long 0x12040060     /* Bank0 2MB FLASH	  @ 0x01000000 */
	.long 0x22080060     /* Bank1 512KB FLASH @ 0x02000000 */
	.long 0x00000000
	.long 0x00000000
	.long 0x00000000
	.long 0x00000000
	.long 0x08000380     /* DRAM0 8MB SRAM	  @ 0x00000000 */
	.long 0x00000000
	.long 0x00000000
	.long 0x00000000
	.long 0x9c218360     /* enable memory */
#endif

#endif
#endif

.globl relocate_vectors
relocate_vectors:
	ldr	r0, [r9, #GD_RELOCADDR]	/* r0 = gd->relocaddr */
	sub	r1, r1, r1
	ldmia	r0!, {r2-r8,r10}
	stmia	r1!, {r2-r8,r10}
	ldmia	r0!, {r2-r8,r10}
	stmia	r1!, {r2-r8,r10}
	mov	pc, lr
