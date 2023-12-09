#include <common.h>
#include <asm/io.h>
#include <linux/delay.h>
#include <asm/arch/hardware.h>
#include <flash.h>
#include <init.h>

flash_info_t flash_info[CFI_FLASH_BANKS];
static ulong flash_base[] = CFG_SYS_FLASH_STARTS;

#ifdef CFG_FLASH_16BIT
#define ADDR_SHIFT  1
#else
#define ADDR_SHIFT  0
#endif

inline static void FLASH_CMD_UNLOCK (u32 base)
{
    out_8(base + (0x555 << ADDR_SHIFT), 0xAA);
    out_8(base + (0x2AA << ADDR_SHIFT), 0x55);
}

inline static void FLASH_CMD_RESET (u32 base)
{
    FLASH_CMD_UNLOCK(base);
    out_8(base, 0xF0);
}

inline static void FLASH_CMD_SELECT (u32 base)
{
    FLASH_CMD_UNLOCK(base);
    out_8(base + (0x555 << ADDR_SHIFT), 0x90);
}

inline static void FLASH_CMD_ERASE_SEC (u32 base, u32 addr)
{
    FLASH_CMD_UNLOCK(base);
    out_8(base + (0x555 << ADDR_SHIFT), 0x80);
    FLASH_CMD_UNLOCK(base);
    out_8(addr, 0x30);
}

static int flash_check_protection (flash_info_t *info, int s_first, int s_last)
{
    int sect, prot = 0;
    for (sect = s_first; sect <= s_last; sect++)
        if (info->protect[sect]) {
            printf("  Flash sector %d protected.\n", sect);
            prot++;
        }
    return prot;
}

static int flash_poll (u32 addr, u8 data, ulong timeOut)
{
    u32 done = 0;
    ulong t0;
    int error = 0;
    volatile u8 flashData;

    data = data & 0xFF;
    t0 = get_timer(0);
    while (get_timer(t0) < timeOut) {
        /*  Read the Data */
        flashData = in_8(addr);
        /*  FLASH_DQ7 = Data? */
        if ((flashData & 0x80) == (data & 0x80)) {
            done = 1;
            break;
        }

        /*  Check Timeout (FLASH_DQ5==1) */
        if (flashData & 0x20) {
            /*  Read the Data */
            flashData = in_8(addr);
            /*  FLASH_DQ7 = Data? */
            if (!((flashData & 0x80) == (data & 0x80))) {
                printf("_flash_poll(): FLASH_DQ7 & flashData not equal to write value\n");
                error = ERR_PROG_ERROR;
            }
            FLASH_CMD_RESET(addr);
            done = 1;
            break;
        }
        /*  spin delay */
        udelay(10);
    }

    /*  error update */
    if (!done) {
        printf("_flash_poll(): Timeout\n");
        error = ERR_TIMEOUT;
    }

    /*  Check the data */
    if (!error) {
        /*  Read the Data */
        flashData = in_8(addr);
        if (flashData != data) {
            error = ERR_PROG_ERROR;
            printf("_flash_poll(): flashData(0x%04x) not equal to data(0x%04x)\n", flashData, data);
        }
    }

    return error;
}

static void flash_is_amdlv160b(flash_info_t *info, u32 base)
{
    u32 i, flashtest;
    info->flash_id = FLASH_AM160B;
    info->sector_count = 35;
    info->size = 2 * 1024 * 1024;   /* 2MB */
    /* 1*16K Boot Block
       2*8K Parameter Block
       1*32K Small Main Block */
    info->start[0] = base;
    info->start[1] = base + 0x4000;
    info->start[2] = base + 0x6000;
    info->start[3] = base + 0x8000;
    for (i = 1; i < info->sector_count; i++)
        info->start[3 + i] = base + i * (64 * 1024);

    for (i = 0; i < info->sector_count; i++) {
        /* Write auto select command sequence and query sector protection */
        FLASH_CMD_SELECT(info->start[i]);
        flashtest = in_8(info->start[i] + 4);
        FLASH_CMD_RESET(base);
        info->protect[i] = (flashtest & 0x0001);
    }
}

static void flash_is_amdf040(flash_info_t *info, u32 base)
{
    int i, flashtest;
    info->flash_id = FLASH_AM040;
    info->sector_count = 8;
    for (i = 0; i < 8; i++)
        info->start[i] = base + (i * 0x10000);
    info->size = i * 0x10000;

    for (i = 0; i < info->sector_count; i++) {
        FLASH_CMD_SELECT(info->start[i]);
        flashtest = in_8(info->start[i] + 2);
        FLASH_CMD_RESET(base);
        info->protect[i] = (flashtest & 0x0001);
    }
}

unsigned long flash_init(void)
{
    u32 i, size, base;
    u32 vendorId, deviceId;
    for (size = i = 0; i < CFI_FLASH_BANKS; i++) {
        base = flash_base[i];
        FLASH_CMD_SELECT(base);
#ifdef CFG_FLASH_16BIT
        vendorId = in_16(base);
        deviceId = in_16(base + 2);
#else
        vendorId = in_8(base);
        deviceId = in_8(base + 1);
#endif
        FLASH_CMD_RESET(base);

        switch (deviceId) {
            case AMD_ID_LV160B & 65535:
                flash_is_amdlv160b(flash_info + i, base);
                break;
            case AMD_ID_F040B:
                flash_is_amdf040(flash_info + i, base);
                break;
        }
        size += flash_info[i].size;
    }

    /*
     * protect monitor sectors in the 1st flash
     */
    base = flash_base[0];
    flash_protect(FLAG_PROTECT_SET, base,
                  base + monitor_flash_len + CONFIG_ENV_SECT_SIZE - 1, flash_info);

    return size;
}

void flash_print_info (flash_info_t * info)
{
    int i;

    switch (info->flash_id) {
        case FLASH_AM160B:
            printf ("AMD Am29LV160DB (2MB) 16KB,2x8KB,32KB,31x64KB\n");
            break;
        case FLASH_AM040:
            printf ("AMD Am29LV040B (512KB) 8x64KB\n");
            break;
    }

    printf ("  Size: %ld KB in %d Sectors\n", info->size >> 10, info->sector_count);
    printf ("  Sector Start Addresses:");
    for (i = 0; i < info->sector_count; i++) {
        if ((i % 4) == 0)
            printf ("\n   ");
        printf (" S%02d @ 0x%08lX%s", i, info->start[i], info->protect[i] ? " !" : "  ");
    }
    printf ("\n");
}

int flash_erase (flash_info_t * info, int s_first, int s_last)
{
    u16 i, error = 0;

    printf ("\n");
    /* check flash protection bits */
    if (flash_check_protection (info, s_first, s_last)) {
        printf ("  Flash erase aborted due to protected sectors\n");
        return ERR_PROTECTED;
    }

    if ((s_first < info->sector_count) && (s_first <= s_last)) {
        for (i = s_first; i <= s_last && !error; i++) {
            printf ("  Erasing Sector %d @ 0x%08lx ... ", i, info->start[i]);
            FLASH_CMD_ERASE_SEC(info->start[0], info->start[i]);
            /* look for sector to become 0xFF after erase */
            error = flash_poll(info->start[i], 0xFF, CFG_SYS_FLASH_ERASE_TOUT);
            FLASH_CMD_RESET(info->start[0]);
            printf ("done\n");
            if (error) {
                break;
            }
        }
    } else error = ERR_INVAL;

    return error;
}

int write_buff (flash_info_t * info, uchar * src, ulong addr, ulong cnt)
{
    int i, n, error = 0;
    u32 base = info->start[0];
    u8 *bp = (u8*)addr, *bps = (u8*)src;

#ifdef CFG_FLASH_16BIT
    cnt = (cnt + 1) & ~1;
    for (n = 0; n < cnt && !error; n += 2, bp += 2, bps += 2) {
#else
    for (n = 0; n < cnt && !error; n++, bp++, bps++) {
#endif
        if (!(n % 8192)) {
            printf (".");
        }
        /*  Write the flash command for flash memory */
        FLASH_CMD_UNLOCK(base);
        out_8(base + (0x555 << ADDR_SHIFT), 0xA0);
        /*  Write the data */
#ifdef CFG_FLASH_16BIT
        *((u16 *)bp) = *((u16 *)bps);
#else
        *bp = *bps;
#endif
        /*  Check if the write is done */
        for (i = 0; i < 0xff; i++);
        error = flash_poll ((u32)bp, *bps, CFG_SYS_FLASH_WRITE_TOUT);
        if (error) {
            return error;
        }
    }

    /*  Reset the Flash Mode to read */
    FLASH_CMD_RESET(base);

    printf (" ");
    return error;
}
