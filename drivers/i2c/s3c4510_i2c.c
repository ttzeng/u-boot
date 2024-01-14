#include <common.h>
#include <i2c.h>
#include <linux/delay.h>
#include <asm/io.h>
#include <asm/arch/hardware.h>

#ifndef CONFIG_SYS_I2C_SPEED
#define CONFIG_SYS_I2C_SPEED 100000
#endif

#ifndef CONFIG_SYS_I2C_SLAVE
#define CONFIG_SYS_I2C_SLAVE 0xfe
#endif

#define I2C_WRITE       0
#define I2C_READ        1
#define I2C_OK          0
#define I2C_ERR         -1
#define I2C_TIMEOUT     1000    /* in usec */
#define I2C_RETRY       10

static inline int __wait_for_bf(int *status)
{
    int retry;
    for (retry = I2C_RETRY; retry && !(in_32(REG_I2C_CON) & I2CCON_BF); retry--)
        udelay(I2C_TIMEOUT);
    *status = in_32(REG_I2C_CON);
    return !(*status & I2CCON_BF);
}

static inline void __i2c_start(void)
{
    out_32(REG_I2C_CON, I2CCON_COND_START | I2CCON_ACK | I2CCON_IEN);
}

static inline void __i2c_stop(void)
{
    int retry;
    out_32(REG_I2C_CON, I2CCON_COND_STOP);
    for (retry = I2C_RETRY; retry && (in_32(REG_I2C_CON) & I2CCON_BUSY); retry--)
        udelay(I2C_TIMEOUT);
    if (!retry) {
        out_32(REG_I2C_CON, I2CCON_RESET);
        out_32(REG_I2C_PS, I2CPS_100K);
    }
}

static inline void __i2c_recven(void)
{
    out_32(REG_I2C_CON, I2CCON_ACK | I2CCON_IEN);
}

static inline void __set_addr(uchar addr, uchar dir)
{
    addr = (addr << 1) | ((dir == I2C_READ) ? 1 : 0);
    out_32(REG_I2C_BUF, addr);
}

static inline int __sendbytes(const char *buf, int count)
{
    int wc, status;
    for (wc = 0; wc < count; wc++) {
        out_32(REG_I2C_BUF, buf[wc]);
        if (__wait_for_bf(&status) || (status & I2CCON_LRB)) {
            __i2c_stop();
            return I2C_ERR;
        }
    }
    __i2c_stop();
    return wc;
}

static inline int __readbytes(char *buf, int count)
{
    int i, status;
    for (i = 0; i < count; i++) {
        if (__wait_for_bf(&status)) {
            __i2c_stop();
            return I2C_ERR;
        }
        if (i == count - 2)
            out_32(REG_I2C_CON, I2CCON_BF | I2CCON_IEN);
        else if (i == count - 1)
            __i2c_stop();
        buf[i] = in_32(REG_I2C_BUF);
    }
    return i;
}

static inline void __i2c_init(int speed, int slaveadd)
{
    out_32(REG_I2C_CON, I2CCON_RESET);
    out_32(REG_I2C_PS, I2CPS_100K);
}

static inline int __i2c_probe(uchar chip)
{
    int status;
    __set_addr(chip, I2C_READ);
    __i2c_start();
    __wait_for_bf(&status);
    __i2c_stop();
    return status & I2CCON_LRB;
}

static inline int __i2c_read(uchar chip, uint addr, int alen, uchar *buffer, int len)
{
    int status;
    __set_addr(chip, I2C_READ);
    __i2c_start();
    if (__wait_for_bf(&status) || (status & I2CCON_LRB)) {
        __i2c_stop();
        return I2C_ERR;
    }
    __i2c_recven();
    return (__readbytes(buffer, len) == I2C_ERR) ? I2C_ERR : I2C_OK;
}

static inline int __i2c_write(uchar chip, uint addr, int alen, uchar *buffer, int len)
{
    int status;
    __set_addr(chip, I2C_WRITE);
    __i2c_start();
    if (__wait_for_bf(&status) || (status & I2CCON_LRB)) {
        __i2c_stop();
        return I2C_ERR;
    }
    return (__sendbytes(buffer, len) == I2C_ERR) ? I2C_ERR : I2C_OK;
}

static inline unsigned int __i2c_get_bus_speed(void)
{
    return CONFIG_SYS_I2C_SPEED;
}

static inline int __i2c_set_bus_speed(unsigned int speed)
{
    if (speed != CONFIG_SYS_I2C_SPEED)
        return I2C_ERR;
    return I2C_OK;
}

#if defined(CONFIG_SYS_I2C_LEGACY)
static void s3c4510_i2c_init(struct i2c_adapter *adap, int speed, int slaveadd)
{
    __i2c_init(speed, slaveadd);
}

static int s3c4510_i2c_probe(struct i2c_adapter *adap, uchar chip)
{
    return __i2c_probe(chip);
}

static int s3c4510_i2c_read(struct i2c_adapter *adap, u8 chip, uint addr, int alen, u8 *data, int len)
{
    return __i2c_read(chip, addr, alen, data, len);
}

static int s3c4510_i2c_write(struct i2c_adapter *adap, u8 chip, uint addr, int alen, u8 *data, int len)
{
    return __i2c_write(chip, addr, alen, data, len);
}

static unsigned int s3c4510_i2c_set_bus_speed(struct i2c_adapter *adap, unsigned int speed)
{
    return __i2c_set_bus_speed(speed);
}

U_BOOT_I2C_ADAP_COMPLETE(s3c4510,
                         s3c4510_i2c_init,
                         s3c4510_i2c_probe,
                         s3c4510_i2c_read,
                         s3c4510_i2c_write,
                         s3c4510_i2c_set_bus_speed,
                         CONFIG_SYS_I2C_SPEED,
                         CONFIG_SYS_I2C_SLAVE,
                         0)
#else
void i2c_init(int speed, int slaveadd)
{
    __i2c_init(speed, slaveadd);
}

int i2c_probe(uchar chip)
{
    return __i2c_probe(chip);
}

int i2c_read(uchar chip, uint addr, int alen, uchar *buffer, int len)
{
    return __i2c_read(chip, addr, alen, buffer, len);
}

int i2c_write(uchar chip, uint addr, int alen, uchar *buffer, int len)
{
    return __i2c_write(chip, addr, alen, buffer, len);
}

unsigned int i2c_get_bus_speed(void)
{
    return __i2c_get_bus_speed();
}

int i2c_set_bus_speed(unsigned int speed)
{
    return __i2c_set_bus_speed(speed);
}
#endif
