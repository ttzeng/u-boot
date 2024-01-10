#ifndef __CONFIG_H
#define __CONFIG_H

#define CFG_SYS_TIMER_RATE          CONFIG_SYS_CLK_FREQ /* 50 MHz */
#define CFG_SYS_FLASH_ERASE_TOUT    3000                /* 3000 msec */
#define CFG_SYS_FLASH_WRITE_TOUT    3000                /* 3000 msec */

#undef CONFIG_NR_DRAM_BANKS
#if defined(CONFIG_PLATFORM_EVALUATOR7T)
  #define CONFIG_NR_DRAM_BANKS  1           /* we have 1 banks of SRAM        */
  #define CFG_SYS_INIT_RAM_ADDR 0x00000000  /* SRAM Bank #1                   */
  #define CFG_SYS_INIT_RAM_SIZE 0x00080000  /*   512KB                        */
  #define CFI_FLASH_BANKS       1
  #define CFG_FLASH_16BIT
  #define CFG_SYS_FLASH_BASE    0x05000000  /* Flash Bank #1                  */
  #define CFG_SYS_FLASH_SIZE    0x00200000  /*   2 MB (one chip,16bit access) */
  #define CFG_SYS_FLASH_STARTS  { CFG_SYS_FLASH_BASE }
#elif defined(CONFIG_PLATFORM_NETSTART)
  #define CONFIG_NR_DRAM_BANKS  1           /* we have 1 banks of DRAM        */
  #define CFG_SYS_INIT_RAM_ADDR 0x00000000  /* SDRAM Bank #1                  */
  #define CFG_SYS_INIT_RAM_SIZE 0x01000000  /*   16MB                         */
  #define CFI_FLASH_BANKS       1
  #define CFG_FLASH_16BIT
  #define CFG_SYS_FLASH_BASE    0x05000000  /* Flash Bank #1                  */
  #define CFG_SYS_FLASH_SIZE    0x00200000  /*   2 MB (one chip,16bit access) */
  #define CFG_SYS_FLASH_STARTS  { CFG_SYS_FLASH_BASE }
  #define CFG_SYS_I2C_SPEED     100000      /* 100 kHz */
#endif

#endif
