OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_start)
SECTIONS
{
    . = 0x00000000;
    __image_copy_start = .;
    .text : {
        *(.text*)
    }
    . = ALIGN(4);
    .rodata : {
        *(.rodata*)
    }
    . = ALIGN(4);
    .data : {
        *(.data*)
        *(.piggydata)
    }
    . = ALIGN(4);
    __image_copy_end = .;
    .bss : {
        __bss_start = .;
        *(.bss*)
        __bss_end = .;
    }
    _end = .;

    . = ALIGN(4096);
    unzip_buffer = .;
}
