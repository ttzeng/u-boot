#include <asm-offsets.h>
#include <config.h>
#include <version.h>
#include <system-constants.h>
#include <asm/arch/hardware.h>

.globl relocate_vectors
relocate_vectors:
    ldr     r0, [r9, #GD_RELOCADDR] /* r0 = gd->relocaddr */
    sub     r1, r1, r1
    ldmia   r0!, {r2-r8,r10}
    stmia   r1!, {r2-r8,r10}
    ldmia   r0!, {r2-r8,r10}
    stmia   r1!, {r2-r8,r10}
    mov     pc, lr

#ifndef CONFIG_SKIP_LOWLEVEL_INIT
#ifndef CONFIG_SKIP_LOWLEVEL_INIT_ONLY

onchip_sram_base:
    .word   SRAM_BASE_HIGH

.globl lowlevel_init
lowlevel_init:
    /* Enable the on-chip 2KB SRAM @ 0x60000000 */
    ldr     r1, =LSCR
    ldr     r0, =(LSCR_EN | LSCR_HIGHMEM)
    str     r0, [r1]
    /* store a code snippet in the on chip SRAM for memory copy */
    ldr     r0, onchip_sram_base
    ldr     r1, =0xe8b007f8     /* copy: ldmia r0!, {r3-r10} */
    ldr     r2, =0xe8a107f8     /*       stmia r1!, {r3-r10} */
    ldr     r3, =0xe1510002     /*       cmp r1, r2          */
    ldr     r4, =0x3afffffb     /*       bcc loop            */
    ldr     r5, =0xe1a0f00e     /*       mov pc, lr          */
    stm     r0, {r1-r5}
    /* use the on chip SRAM as initial stack as well */
    add     sp, r0, #0x100

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

#ifdef CONFIG_MONITOR_IS_IN_RAM
    pop     {r1,r2,r3,ip,lr}
    add     r0, r1, r3
    ldr     pc, onchip_sram_base
#else
    adr     r0, memory_map_initial
    bl      memory_setup
    add     r0, r1, r3
    ldr     r4, shadow_ram_base
    add     r1, r1, r4
    add     r2, r2, r4
    adr     lr, remap
    ldr     pc, onchip_sram_base

remap:
    pop     {r1,r2,r3,ip,lr}
    adr     r0, memory_map_remap
memory_setup:
1:  ldmia   r0!, {r4,r5}
    cmp     r4, #0
    moveq   pc, lr
    str     r5, [r4]
    b       1b

#define SRAM_BASE_SHADOW    0x10000000

memory_map_initial:
    /* Bank0: 8-bit Flash selected by CE0#, 3 Wait cycles */
    .word   BCR0, BCR_ECE_CE0L | BCR_ECE_CE0H | BCR_WAIT3
    /* Bank1: 16-bit SRAM selected by CE1#, 1 Wait cycles */
    .word   BCR1, BCR_ECE_CE1L | BCR_ECE_CE1H | BCR_WAIT1 | BCR_MS
    /* Segment0: Bank0, 0x04000000-0x04080000, System:Read/Write, User:Read/Write */
    .word   SDR0, SDR_BSEL0 | SDR_SPR_RDWR | SDR_UPR_RDWR
    .word   START0, CFG_SYS_FLASH_BASE
    .word   STOP0,  CFG_SYS_FLASH_BASE + CFG_SYS_FLASH_SIZE
    /* Segment1: Bank1, 0x10000000-0x10040000, System:Read/Write, User:Read/Write */
    .word   SDR1, SDR_BSEL1 | SDR_SPR_RDWR | SDR_UPR_RDWR
    .word   START1
shadow_ram_base:
    .word   SRAM_BASE_SHADOW
    .word   STOP1, SRAM_BASE_SHADOW + CFG_SYS_INIT_RAM_SIZE
    .word   0

memory_map_remap:
    /* Segment1: Bank1, 0x00000000-0x00040000, System:Read/Write, User:Read/Write */
    .word   STOP1, CFG_SYS_INIT_RAM_ADDR + CFG_SYS_INIT_RAM_SIZE
    .word   START1, CFG_SYS_INIT_RAM_ADDR
    .word   SDR1, SDR_BSEL1 | SDR_SPR_RDWR | SDR_UPR_RDWR
    .word   0
#endif
#endif
#endif
