#include <common.h>
#include <command.h>
#include <rtc.h>
#include <i2c.h>

#define DS1672_I2C_ADDR     0x68
#define DS1672_COUNT1       0
#define DS1672_COUNT2       1
#define DS1672_COUNT3       2
#define DS1672_COUNT4       3
#define DS1672_CONTROL      4
#define DS1672_CHARGER      5

#define RTC_SUCCESS         0

int rtc_get(struct rtc_time *tmp)
{
    int result = RTC_SUCCESS;
    uchar buf[4];
    buf[0] = 0;
    if (i2c_write(DS1672_I2C_ADDR, 0, 1, buf, 1) == 0 &&
        i2c_read (DS1672_I2C_ADDR, 0, 1, buf, 4) == 0) {
        u32 seconds = ((u32)buf[0])        |
                       ((u32)buf[1] <<  8) |
                       ((u32)buf[2] << 16) |
                       ((u32)buf[3] << 24);
        rtc_to_tm(seconds, tmp);
    } else
        result = ~RTC_SUCCESS;
    return result;
}

int rtc_set(struct rtc_time *tmp)
{
    int result = RTC_SUCCESS;
    u32 seconds = rtc_mktime(tmp);
    uchar buf[7];
    buf[0] = 0,
    buf[1] = seconds >>  0,
    buf[2] = seconds >>  8,
    buf[3] = seconds >> 16,
    buf[4] = seconds >> 24;
    buf[5] = 0;
    buf[6] = 0xaa;
    if (i2c_write(DS1672_I2C_ADDR, 0, 1, buf, 7) != 0)
        result = ~RTC_SUCCESS;
    return result;
}

void rtc_reset(void)
{
}
