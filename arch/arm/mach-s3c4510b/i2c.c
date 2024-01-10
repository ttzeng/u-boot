#include <common.h>
#include <asm/io.h>
#include <i2c.h>
#include <asm/arch/hardware.h>
#include <linux/delay.h>

#define I2C_WRITE       0
#define I2C_READ        1
#define I2C_OK          0
#define I2C_ERR         -1
#define I2C_TIMEOUT     1000    /* in usec */
#define I2C_RETRY       10

unsigned int i2c_get_bus_speed(void)
{
    return CFG_SYS_I2C_SPEED;
}

int i2c_set_bus_speed(unsigned int speed)
{
    if (speed != CFG_SYS_I2C_SPEED)
        return -1;
    return 0;
}

static inline int wait_for_bf(int *status)
{
    int retry;
    for (retry = I2C_RETRY; retry && !(in_32(REG_I2C_CON) & I2CCON_BF); retry--)
        udelay(I2C_TIMEOUT);
    *status = in_32(REG_I2C_CON);
    return !(*status & I2CCON_BF);
}

static inline void i2c_start(void)
{
    out_32(REG_I2C_CON, I2CCON_COND_START | I2CCON_ACK | I2CCON_IEN);
}

static inline void i2c_stop(void)
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

static inline void i2c_recven(void)
{
    out_32(REG_I2C_CON, I2CCON_ACK | I2CCON_IEN);
}

static inline void set_addr(uchar addr, uchar dir)
{
    addr = (addr << 1) | ((dir == I2C_READ) ? 1 : 0);
    out_32(REG_I2C_BUF, addr);
}

static int sendbytes(const char *buf, int count)
{
    int wc, status;
    for (wc = 0; wc < count; wc++) {
        out_32(REG_I2C_BUF, buf[wc]);
        if (wait_for_bf(&status) || (status & I2CCON_LRB)) {
            i2c_stop();
            return I2C_ERR;
        }
    }
    i2c_stop();
    return wc;
}

static int readbytes(char *buf, int count)
{
    int i, status;
    for (i = 0; i < count; i++) {
        if (wait_for_bf(&status)) {
            i2c_stop();
            return I2C_ERR;
        }
        if (i == count - 2)
            out_32(REG_I2C_CON, I2CCON_BF | I2CCON_IEN);
        else if (i == count - 1)
            i2c_stop();
        buf[i] = in_32(REG_I2C_BUF);
    }
    return i;
}

void i2c_init(int speed, int slaveadd)
{
    out_32(REG_I2C_CON, I2CCON_RESET);
    out_32(REG_I2C_PS, I2CPS_100K);
}

int i2c_probe(uchar chip)
{
    int status;
    set_addr(chip, I2C_READ);
    i2c_start();
    wait_for_bf(&status);
    i2c_stop();
    return status & I2CCON_LRB;
}

int i2c_read(uchar chip, uint addr, int alen, uchar *buffer, int len)
{
    int status;
    set_addr(chip, I2C_READ);
    i2c_start();
    if (wait_for_bf(&status) || (status & I2CCON_LRB)) {
        i2c_stop();
        return I2C_ERR;
    }
    i2c_recven();
    return (readbytes(buffer, len) == I2C_ERR) ? I2C_ERR : 0;
}

int i2c_write(uchar chip, uint addr, int alen, uchar *buffer, int len)
{
    int status;
    set_addr(chip, I2C_WRITE);
    i2c_start();
    if (wait_for_bf(&status) || (status & I2CCON_LRB)) {
        i2c_stop();
        return I2C_ERR;
    }
    return (sendbytes(buffer, len) == I2C_ERR) ? I2C_ERR : 0;
}
