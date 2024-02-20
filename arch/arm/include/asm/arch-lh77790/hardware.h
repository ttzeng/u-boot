#ifndef __LH77790_HARDWARE_H
#define __LH77790_HARDWARE_H

#define SRAM_BASE_LOW   0x00000000
#define SRAM_BASE_HIGH  0x60000000
#define CACHE_BASE      0x60000800

#define CCR             0xffffa400  /* Cache Control Register */
#define CCR_EN              0x01
#define CCR_SRAM            0x02
#define CCR_FLUSH           0x04
#define CCR_INVALID         0x08

#define LSCR            0xffffa404  /* Local SRAM Control Register */
#define LSCR_EN             0x01
#define LSCR_HIGHMEM        0x02

#define MEM_REG_BASE    0xffffa000  /* Memory Segment Registers */
#define START0          (MEM_REG_BASE + 0x00)   /* Segment START Registers */
#define START1          (MEM_REG_BASE + 0x04)
#define START2          (MEM_REG_BASE + 0x08)
#define START3          (MEM_REG_BASE + 0x0c)
#define START4          (MEM_REG_BASE + 0x10)
#define START5          (MEM_REG_BASE + 0x14)
#define START6          (MEM_REG_BASE + 0x18)
#define START7          (MEM_REG_BASE + 0x1c)
#define STOP0           (MEM_REG_BASE + 0x20)   /* Segment STOP Registers */
#define STOP1           (MEM_REG_BASE + 0x24)
#define STOP2           (MEM_REG_BASE + 0x28)
#define STOP3           (MEM_REG_BASE + 0x2c)
#define STOP4           (MEM_REG_BASE + 0x30)
#define STOP5           (MEM_REG_BASE + 0x34)
#define STOP6           (MEM_REG_BASE + 0x38)
#define STOP7           (MEM_REG_BASE + 0x3c)
#define SDR0            (MEM_REG_BASE + 0x40)   /* Segment Descriptor Registers */
#define SDR1            (MEM_REG_BASE + 0x44)
#define SDR2            (MEM_REG_BASE + 0x48)
#define SDR3            (MEM_REG_BASE + 0x4c)
#define SDR4            (MEM_REG_BASE + 0x50)
#define SDR5            (MEM_REG_BASE + 0x54)
#define SDR6            (MEM_REG_BASE + 0x58)
#define SDR7            (MEM_REG_BASE + 0x5c)
#define SDR8            (MEM_REG_BASE + 0x60)
#define SDR_BSEL0           (1 << 0)
#define SDR_BSEL1           (1 << 1)
#define SDR_BSEL2           (1 << 2)
#define SDR_BSEL3           (1 << 3)
#define SDR_BSEL4           (1 << 4)
#define SDR_BSEL5           (1 << 5)
#define SDR_BSEL6           (1 << 6)
#define SDR_BSEL7           (1 << 7)
#define SDR_HW              (1 << 8)
#define SDR_C               (1 << 10)
#define SDR_UPR_NONE        (0 << 11)
#define SDR_UPR_RDONLY      (1 << 11)
#define SDR_UPR_WRONLY      (2 << 11)
#define SDR_UPR_RDWR        (3 << 11)
#define SDR_SPR_NONE        (0 << 13)
#define SDR_SPR_RDONLY      (1 << 13)
#define SDR_SPR_WRONLY      (2 << 13)
#define SDR_SPR_RDWR        (3 << 13)

#define BCR_BASE        0xffffa100  /* Bank Configuration Registers */
#define BCR0            (BCR_BASE + 0x00)
#define BCR1            (BCR_BASE + 0x04)
#define BCR2            (BCR_BASE + 0x08)
#define BCR3            (BCR_BASE + 0x0c)
#define BCR4            (BCR_BASE + 0x10)
#define BCR5            (BCR_BASE + 0x14)
#define BCR_ECE_CE0L        (1 << 0)
#define BCR_ECE_CE0H        (1 << 1)
#define BCR_ECE_CE1L        (1 << 2)
#define BCR_ECE_CE1H        (1 << 3)
#define BCR_ECE_CE2L        (1 << 4)
#define BCR_ECE_CE2H        (1 << 5)
#define BCR_ECE_CE3L        (1 << 6)
#define BCR_ECE_CE3H        (1 << 7)
#define BCR_ECE_CE4L        (1 << 8)
#define BCR_ECE_CE4H        (1 << 9)
#define BCR_ECE_CE5L        (1 << 10)
#define BCR_ECE_CE5H        (1 << 11)
#define BCR_WAIT0           (0 << 12)
#define BCR_WAIT1           (1 << 12)
#define BCR_WAIT2           (2 << 12)
#define BCR_WAIT3           (3 << 12)
#define BCR_WAIT4           (4 << 12)
#define BCR_WAIT5           (5 << 12)
#define BCR_WAIT6           (6 << 12)
#define BCR_WAIT7           (7 << 12)
#define BCR_MS              (1 << 15)
#define BCR6a           (BCR_BASE + 0x18)
#define BCR7a           (BCR_BASE + 0x1c)
#define BCR6b           (BCR_BASE + 0x20)
#define BCR7b           (BCR_BASE + 0x24)
#define DRR             (BCR_BASE + 0x28)

#define CPMU_BASE       0xffffac00  /* Clock and Power Management Unit */
#define PCSR            (CPMU_BASE + 0x04)  /* Peripheral Clock Select Register */
#define PCSR_UC0S           (1 << 0)
#define PCSR_UC1S           (1 << 1)
#define PCSR_UC2S           (1 << 2)
#define PCSR_CT0CS          (1 << 3)
#define PCSR_CT1CS          (1 << 4)
#define PCSR_CT2CS          (1 << 5)
#define PCSR_LCDCE          (1 << 6)
#define PCSR_PWMCE          (1 << 7)
#define PCSR_SIRCE          (1 << 8)
#define U0CCR           (CPMU_BASE + 0x08)  /* UART0 Clock Control Register */
#define U1CCR           (CPMU_BASE + 0x0c)  /* UART1 Clock Control Register */
#define U2CCR           (CPMU_BASE + 0x10)  /* UART2 Clock Control Register */
#define CT0CCR          (CPMU_BASE + 0x18)  /* Counter/Timer0 Clock Control Register */
#define CT1CCR          (CPMU_BASE + 0x1c)  /* Counter/Timer1 Clock Control Register */
#define CT2CCR          (CPMU_BASE + 0x20)  /* Counter/Timer2 Clock Control Register */
#define CCCR            (CPMU_BASE + 0x28)  /* Core Clock Control Register */

#define UART0_BASE      0xffff0000  /* 16C450 UART0 Registers */
#define UART1_BASE      0xffff0400  /* 16C450 UART1 Registers */
#define UART2_BASE      0xffff0800  /* 16C450 UART2 Registers */
#define OFS_RBR             0x00    /* Receiver Buffer Register */
#define OFS_THR             0x00    /* Transmit Holding Register */
#define OFS_IER             0x04    /* Interrupt Enable Register */
#define IER_ERBFI           (1 << 0)
#define IER_ETBEI           (1 << 1)
#define IER_ELSI            (1 << 2)
#define IER_EDSSI           (1 << 3)
#define OFS_IIR             0x08    /* Interrupt Identification Register */
#define IIR_NO_INT          0x01
#define IIR_RLS             0x06
#define IIR_RB              0x04
#define IIR_THRE            0x02
#define IIR_MS              0x00
#define OFS_LCR             0x0c    /* Line Control Register */
#define LCR_5BIT            (0 << 0)
#define LCR_6BIT            (1 << 0)
#define LCR_7BIT            (2 << 0)
#define LCR_8BIT            (3 << 0)
#define LCR_1STOP           (0 << 2)
#define LCR_2STOP           (1 << 2)
#define LCR_PARITY_NONE     (0 << 3)
#define LCR_PARITY_ODD      (1 << 3)
#define LCR_PARITY_EVEN     (3 << 3)
#define LCR_PARITY_1        (5 << 3)
#define LCR_PARITY_0        (7 << 3)
#define LCR_SB              (1 << 6)
#define LCR_DLAB            (1 << 7)
#define OFS_MCR             0x10    /* Modem Control Register */
#define MCR_DTR             (1 << 0)
#define MCR_RTS             (1 << 1)
#define MCR_OUT1            (1 << 2)
#define MCR_OUT2            (1 << 3)
#define MCR_LOOP            (1 << 4)
#define OFS_LSR             0x14    /* Line Status Register */
#define LSR_DR              (1 << 0)
#define LSR_OE              (1 << 1)
#define LSR_PE              (1 << 2)
#define LSR_FE              (1 << 3)
#define LSR_BI              (1 << 4)
#define LSR_THRE            (1 << 5)
#define LSR_TEMT            (1 << 6)
#define OFS_MSR             0x18    /* Modem Status Register */
#define MSR_DCTS            (1 << 0)
#define MSR_DDSR            (1 << 1)
#define MSR_TERI            (1 << 2)
#define MSR_DDCD            (1 << 3)
#define MSR_CTS             (1 << 4)
#define MSR_DSR             (1 << 5)
#define MSR_RI              (1 << 6)
#define MSR_DCD             (1 << 7)
#define OFS_SCR             0x1c    /* Scratch Pad Register */
#define OFS_DLL             0x00    /* Divisor Latches */
#define OFS_DLM             0x04

#define PWM_BASE        0xffff1000  /* Pulse Width Modulator */
#define PWM0_BASE       (PWM_BASE + 0x00)   /* PWM0 */
#define PWM1_BASE       (PWM_BASE + 0x20)   /* PWM1 */
#define PWM2_BASE       (PWM_BASE + 0x40)   /* PWM2 */
#define PWMA_BASE       (PWM_BASE + 0x60)   /* All PWMs */
#define PWM_TC_OFS          0x00
#define PWM_DC_OFS          0x04
#define PWM_ENB_OFS         0x08
#define PWM_DIV_OFS         0x0c
#define PWM_SYNC_OFS        0x10
#define PWM_INV_OFS         0x14
#define PWM0_TC         (PWM0_BASE + PWM_TC_OFS)
#define PWM0_DC         (PWM0_BASE + PWM_DC_OFS)
#define PWM0_ENB        (PWM0_BASE + PWM_ENB_OFS)
#define PWM0_DIV        (PWM0_BASE + PWM_DIV_OFS)
#define PWM0_SYNC       (PWM0_BASE + PWM_SYNC_OFS)
#define PWM0_INV        (PWM0_BASE + PWM_INV_OFS)
#define PWM1_TC         (PWM1_BASE + PWM_TC_OFS)
#define PWM1_DC         (PWM1_BASE + PWM_DC_OFS)
#define PWM1_ENB        (PWM1_BASE + PWM_ENB_OFS)
#define PWM1_DIV        (PWM1_BASE + PWM_DIV_OFS)
#define PWM1_SYNC       (PWM1_BASE + PWM_SYNC_OFS)
#define PWM1_INV        (PWM1_BASE + PWM_INV_OFS)
#define PWM2_TC         (PWM2_BASE + PWM_TC_OFS)
#define PWM2_DC         (PWM2_BASE + PWM_DC_OFS)
#define PWM2_ENB        (PWM2_BASE + PWM_ENB_OFS)
#define PWM2_DIV        (PWM2_BASE + PWM_DIV_OFS)
#define PWM2_SYNC       (PWM2_BASE + PWM_SYNC_OFS)
#define PWM2_INV        (PWM2_BASE + PWM_INV_OFS)
#define PWMA_TC         (PWM1_BASE + PWM_TC_OFS)
#define PWMA_DC         (PWM1_BASE + PWM_DC_OFS)
#define PWMA_ENB        (PWM1_BASE + PWM_ENB_OFS)
#define PWMA_DIV        (PWM1_BASE + PWM_DIV_OFS)
#define PWMA_SYNC       (PWM1_BASE + PWM_SYNC_OFS)
#define PWM1_INV        (PWM1_BASE + PWM_INV_OFS)

#define CT_BASE         0xffff1800  /* Counter / Timers */
#define CT_CNTR0        (CT_BASE + 0x00)    /* Counter Registers */
#define CT_CNTR1        (CT_BASE + 0x04)
#define CT_CNTR2        (CT_BASE + 0x08)
#define CT_CWR          (CT_BASE + 0x0c)    /* Control Word Register */
#define CT_CWR_BCD          (1 << 0)
#define CT_CWR_MODE0        (0 << 1)
#define CT_CWR_MODE1        (1 << 1)
#define CT_CWR_MODE2        (2 << 1)
#define CT_CWR_MODE3        (3 << 1)
#define CT_CWR_MODE4        (4 << 1)
#define CT_CWR_MODE5        (5 << 1)
#define CT_CWR_LATCH        (0 << 4)
#define CT_CWR_LSB          (1 << 4)
#define CT_CWR_MSB          (2 << 4)
#define CT_CWR_WORD         (3 << 4)
#define CT_CWR_SC0          (0 << 6)
#define CT_CWR_SC1          (1 << 6)
#define CT_CWR_SC2          (2 << 6)
#define CT_CWR_RDBACK       (3 << 6)
#define CT_CWR_RDBACK_CNT0  (1 << 1)
#define CT_CWR_RDBACK_CNT1  (1 << 2)
#define CT_CWR_RDBACK_CNT2  (1 << 3)
#define CT_CWR_RDBACK_STATUS (1 << 4)
#define CT_CWR_RDBACK_COUNT (1 << 5)
#define CT_STATUS_BCD       (1 << 0)
#define CT_STATUS_MODE      (7 << 1)
#define CT_STATUS_RW        (3 << 4)
#define CT_STATUS_NULL_COUNT (1 << 6)
#define CT_STATUS_OUT       (1 << 7)

#define WDT_BASE        0xffffac00          /* Watchdog Timer Register */
#define WDCTLR          (WDT_BASE + 0x30)   /* Watchdog Control Register */
#define WDCTLR_EN           (1 << 0)
#define WDCTLR_RSP_FIQ      (0 << 1)
#define WDCTLR_RSP_EXT_RST  (2 << 1)
#define WDCTLR_RSP_SYS_RST  (3 << 1)
#define WDCTLR_FRZ          (1 << 3)
#define WDCTLR_TOP17        (0 << 4)
#define WDCTLR_TOP19        (1 << 4)
#define WDCTLR_TOP21        (2 << 4)
#define WDCTLR_TOP23        (3 << 4)
#define WDCTLR_TOP25        (4 << 4)
#define WDCTLR_TOP27        (5 << 4)
#define WDCTLR_TOP29        (6 << 4)
#define WDCTLR_TOP31        (7 << 4)
#define WDCNTLR         (WDT_BASE + 0x34)   /* Watchdog Counter Register */

#define PPI_BASE        0xffff1c00  /* Programmable Peripheral Interface */
#define PPI_PA          (PPI_BASE + 0x00)   /* Port A */
#define PPI_PB          (PPI_BASE + 0x04)   /* Port B */
#define PPI_PC          (PPI_BASE + 0x08)   /* Port C */
#define PPI_CTLR        (PPI_BASE + 0x0c)   /* PPI Control Register */
#define PPI_CTLR_PCL_IN     (1 << 0)
#define PPI_CTLR_PB_IN      (1 << 1)
#define PPI_CTLR_PB_MODE0   (0 << 2)
#define PPI_CTLR_PB_MODE1   (1 << 2)
#define PPI_CTLR_PCH_IN     (1 << 3)
#define PPI_CTLR_PA_IN      (1 << 4)
#define PPI_CTLR_PA_MODE0   (0 << 5)
#define PPI_CTLR_PA_MODE1   (1 << 5)
#define PPI_CTLR_PA_MODE2   (2 << 5)
#define PPI_CTLR_MODE       (1 << 7)

#define IOCR            0xffffa410  /* Input/Output Configuration Register */
#define IOCR_PU0            (1 << 0)
#define IOCR_PU1            (1 << 1)
#define IOCR_PU2            (1 << 2)
#define IOCR_PU3            (1 << 3)
#define IOCR_PU4            (1 << 4)
#define IOCR_PU5            (1 << 5)
#define IOCR_PU6            (1 << 6)
#define IOCR_PU7            (1 << 7)
#define IOCR_PU8            (1 << 8)
#define IOCR_CT0G_EXT       (0 << 9)
#define IOCR_CT0G_PWM0      (1 << 9)
#define IOCR_CT0G_LO        (2 << 9)
#define IOCR_CT0G_HI        (3 << 9)
#define IOCR_CT1G_EXT       (0 << 11)
#define IOCR_CT1G_PWM1      (1 << 11)
#define IOCR_CT1G_LO        (2 << 11)
#define IOCR_CT1G_HI        (3 << 11)
#define IOCR_CT2G_EXT       (0 << 13)
#define IOCR_CT2G_PWM2      (1 << 13)
#define IOCR_CT2G_LO        (2 << 13)
#define IOCR_CT2G_HI        (3 << 13)

#define INTR_BASE       0xffffa800      /* Interrupt Controller Registers */
#define ICR0            (INTR_BASE + 0x00)  /* Interrupt Configuration Registers */
#define ICR1            (INTR_BASE + 0x04)
#define ICLR            (INTR_BASE + 0x08)  /* Interrupt Clear Register */
#define IRQER           (INTR_BASE + 0x0c)  /* IRQ interrupt Enable Register */
#define FIQER           (INTR_BASE + 0x10)  /* FIQ Interrupt Enable Register */
#define IRQSR           (INTR_BASE + 0x14)  /* IRQ Status Register */
#define FIQSR           (INTR_BASE + 0x18)  /* FIQ Status Register */
#define IPR             (INTR_BASE + 0x1c)  /* Interrupt Polling Register */

#ifndef __ASSEMBLY__
enum {
    INT_INT0 = 0,
    INT_INT1,
    INT_INT2,
    INT_INT3,
    INT_INT4,
    INT_INT5,
    INT_CT0,
    INT_CT1,
    INT_CT2,
    INT_UART0,
    INT_UART1,
    INT_UART2,
    INT_WDT,
    N_IRQS
};
#endif

#endif /* __LH77790_HARDWARE_H */
