#include <linux/types.h>
#include <common.h>
#include <debug_uart.h>
#include <gzip.h>

#define SIZE_GZIP_BUFFER 0x40000

extern uchar input_data[], input_data_end[], unzip_buffer[];

void puts(const char *s)
{
#if CONFIG_IS_ENABLED(DEBUG_UART)
    while (*s)
        printch(*s++);
#endif
}

int printf(const char* fmt, ...)
{
    return 0;
}

void main(int argc, char *const argv[])
{
    unsigned long src_len = input_data_end - input_data;
    puts("Uncompressing ...");
    if (gunzip(unzip_buffer, SIZE_GZIP_BUFFER, input_data, &src_len) == 0) {
        void (*jump)(void) = (void (*)(void))unzip_buffer;
        (*jump)();
    }
}

void schedule(void)
{
}

void* malloc(int size)
{
    static long free_mem_ptr = (long)unzip_buffer + SIZE_GZIP_BUFFER;
    void *p;
    free_mem_ptr = (free_mem_ptr + 15) & ~15;   /* Align */
    p = (void*)free_mem_ptr;
    free_mem_ptr += size;
    return p;
}

void free(void *where)
{   /* Don't care */
}
