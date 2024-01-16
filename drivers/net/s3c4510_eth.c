/***********************************************************************
 *
 * Copyright (c) 2004	Cucy Systems (http://www.cucy.com)
 * Curt Brune <curt@cucy.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 * Description:   Ethernet interface for Samsung S3C4510B SoC
 */

#include <common.h>
#include <command.h>
#include <net.h>
#include <dm.h>
#include <malloc.h>
#include <linux/delay.h>
#include <asm/io.h>
#include <asm/arch/hardware.h>
#include "s3c4510_eth.h"

static TX_FrameDescriptor   txFDbase[ETH_MaxTxFrames];
static MACFrame             txFrameBase[ETH_MaxTxFrames];
static RX_FrameDescriptor   rxFDbase[PKTBUFSRX];
/*
   The common code sets up packet buffers for you already in the .bss
   (net_rx_packets), so there should be no need to allocate your own.
static MACFrame             rxFrameBase[PKTBUFSRX];
*/
static ETH                  m_eth;

static s32 TxFDinit(ETH *eth)
{
    s32 i;
    MACFrame *txFrmBase;

    /* disable cache for access to the TX buffers */
    txFrmBase = (MACFrame *)((u32)txFrameBase | CACHE_DISABLE_MASK);

    /* store start of Tx descriptors and set current */
    eth->m_curTX_FD  = (TX_FrameDescriptor *)((u32)txFDbase | CACHE_DISABLE_MASK);
    eth->m_baseTX_FD = eth->m_curTX_FD;

    for (i = 0; i < ETH_MaxTxFrames; i++) {
        eth->m_baseTX_FD[i].m_frameDataPtr.bf.dataPtr = (u32)&txFrmBase[i];
        eth->m_baseTX_FD[i].m_frameDataPtr.bf.owner   = 0x0; /* CPU owner */
        eth->m_baseTX_FD[i].m_opt.ui                  = 0x0;
        eth->m_baseTX_FD[i].m_status.ui               = 0x0;
        eth->m_baseTX_FD[i].m_nextFD                  = &eth->m_baseTX_FD[i+1];
    }
    /* make the list circular */
    eth->m_baseTX_FD[i-1].m_nextFD = &eth->m_baseTX_FD[0];

    out_32(REG_BDMATXPTR, (u32)eth->m_curTX_FD);

    return 0;
}

static s32 RxFDinit(ETH *eth)
{
    s32 i;
    /* MACFrame *rxFrmBase; */

    /* disable cache for access to the RX buffers */
    /* rxFrmBase = (MACFrame *)((u32)rxFrameBase | CACHE_DISABLE_MASK); */

    /* store start of Rx descriptors and set current */
    eth->m_curRX_FD  = (RX_FrameDescriptor *)((u32)rxFDbase | CACHE_DISABLE_MASK);
    eth->m_baseRX_FD = eth->m_curRX_FD;

    for (i = 0; i < PKTBUFSRX; i++) {
        eth->m_baseRX_FD[i].m_frameDataPtr.bf.dataPtr = (u32)net_rx_packets[i] | CACHE_DISABLE_MASK;
        eth->m_baseRX_FD[i].m_frameDataPtr.bf.owner   = 0x1; /* BDMA owner */
        eth->m_baseRX_FD[i].m_reserved                = 0x0;
        eth->m_baseRX_FD[i].m_status.ui               = 0x0;
        eth->m_baseRX_FD[i].m_nextFD                  = &eth->m_baseRX_FD[i+1];
    }
    /* make the list circular */
    eth->m_baseRX_FD[i-1].m_nextFD = &eth->m_baseRX_FD[0];

    out_32(REG_BDMARXPTR, (u32)eth->m_curRX_FD);

    return 0;
}

static int s3c4510_eth_init(struct udevice *dev)
{
    struct eth_pdata *pdata = dev_get_plat(dev);
    ETH *eth = &m_eth;

    /* store our MAC address */
    memcpy(eth->m_mac, pdata->enetaddr, ETH_MAC_ADDR_SIZE);

    /* setup DBMA and MAC */
    out_32(REG_BDMARXCON, ETH_BRxRS);   /* reset BDMA RX machine */
    out_32(REG_BDMATXCON, ETH_BTxRS);   /* reset BDMA TX machine */
    out_32(REG_MACCON   , ETH_SwReset); /* reset MAC machine */
    out_32(REG_BDMARXLSZ, sizeof(MACFrame));
    out_32(REG_MACCON   , 0);           /* reset MAC machine */

    /* init frame descriptors */
    TxFDinit(eth);
    RxFDinit(eth);

    /* init the CAM with our MAC address */
    out_32(REG_CAM_BASE, (eth->m_mac[0] << 24) |
                         (eth->m_mac[1] << 16) |
                         (eth->m_mac[2] <<  8) |
                         (eth->m_mac[3]));
    out_32(REG_CAM_BASE + 0x4, (eth->m_mac[4] << 24) | (eth->m_mac[5] << 16));

    /* enable CAM address 1 -- the MAC we just loaded */
    out_32(REG_CAMEN, 0x1);

    out_32(REG_CAMCON,
           ETH_BroadAcc |   /* accept broadcast packetes */
           ETH_CompEn);     /* enable compare mode (check against the CAM) */

    /* configure the BDMA Transmitter control */
    out_32(REG_BDMATXCON,
           ETH_BTxBRST   |  /* BDMA Tx burst size 16 words  */
           ETH_BTxMSL110 |  /* BDMA Tx wait to fill 6/8 of the BDMA */
           ETH_BTxSTSKO  |  /* BDMA Tx interrupt(Stop) on non-owner TX FD */
           ETH_BTxEn);      /* BDMA Tx Enable  */

    /* configure the MAC Transmitter control */
    out_32(REG_MACTXCON,
           ETH_EnComp |     /* interrupt when the MAC transmits or discards packet */
           ETH_TxEn);       /* MAC transmit enable */

    /* configure the BDMA Receiver control */
    out_32(REG_BDMARXCON,
           ETH_BRxBRST   |  /* BDMA Rx Burst Size 16 words */
           ETH_BRxSTSKO  |  /* BDMA Rx interrupt(Stop) on non-owner RX FD */
           ETH_BRxMAINC  |  /* BDMA Rx Memory Address increment */
           ETH_BRxDIE    |  /* BDMA Rx Every Received Frame Interrupt Enable */
           ETH_BRxNLIE   |  /* BDMA Rx NULL List Interrupt Enable */
           ETH_BRxNOIE   |  /* BDMA Rx Not Owner Interrupt Enable */
           ETH_BRxLittle |  /* BDMA Rx Little endian */
           ETH_BRxEn);      /* BDMA Rx Enable */

    /* configure the MAC Receiver control */
    out_32(REG_MACRXCON,
           ETH_RxEn);       /* MAC ETH_RxEn */

    return 0;
}

static void s3c4510_eth_halt(struct udevice *dev)
{
    /* disable MAC */
    out_32(REG_MACCON, ETH_HaltReg);
}

static int s3c4510_eth_send(struct udevice *dev, void *packet, int length)
{
    ETH *eth = &m_eth;

    if (eth->m_curTX_FD->m_frameDataPtr.bf.owner) {
        printf("eth_send(): TX Frame.  CPU not owner.\n");
        return -1;
    }

    /* copy user data into frame data pointer */
    memcpy((void *)((u32)(eth->m_curTX_FD->m_frameDataPtr.bf.dataPtr)),
           (void *)packet,
           length);

    /* Set TX Frame flags */
    eth->m_curTX_FD->m_opt.bf.widgetAlign  = 0;
    eth->m_curTX_FD->m_opt.bf.frameDataDir = 1;
    eth->m_curTX_FD->m_opt.bf.littleEndian = 1;
    eth->m_curTX_FD->m_opt.bf.macTxIrqEnbl = 1;
    eth->m_curTX_FD->m_opt.bf.no_crc       = 0;
    eth->m_curTX_FD->m_opt.bf.no_padding   = 0;

    /* Set TX Frame length */
    eth->m_curTX_FD->m_status.bf.len = length;

    /* Change ownership to BDMA */
    eth->m_curTX_FD->m_frameDataPtr.bf.owner = 1;

    /* Enable MAC and BDMA Tx control register */
    setbits(32, REG_BDMATXCON, ETH_BTxEn);
    setbits(32, REG_MACTXCON,  ETH_TxEn);

    /* poll on TX completion status */
    while (!eth->m_curTX_FD->m_status.bf.complete) {
        /* sleep  */
        udelay(1);
    }

    /* Change the Tx frame descriptor for next use */
    eth->m_curTX_FD = eth->m_curTX_FD->m_nextFD;

    return 0;
}

static int s3c4510_eth_recv(struct udevice *dev, int flags, uchar **packetp)
{
    int result = 0;
    ETH *eth = &m_eth;

    /* check if packet ready */
    if ((in_32(REG_BDMASTAT)) & ETH_S_BRxRDF &&
        !eth->m_curRX_FD->m_frameDataPtr.bf.owner) {
        result = eth->m_curRX_FD->m_status.bf.len;
        *packetp = (u8 *)eth->m_curRX_FD->m_frameDataPtr.ui;
    } else result = -EAGAIN;

    return result;
}

static int s3c4510_eth_free_pkt(struct udevice *dev, uchar *packet, int length)
{
    ETH *eth = &m_eth;

    /* set owner back to BDMA */
    eth->m_curRX_FD->m_frameDataPtr.bf.owner = 1;
    /* clear status */
    eth->m_curRX_FD->m_status.ui = 0x0;
    /* advance to next descriptor */
    eth->m_curRX_FD = eth->m_curRX_FD->m_nextFD;

    if (eth->m_curRX_FD->m_frameDataPtr.bf.owner) {
        /* clear received frame bit if no more pending frames */
        out_32(REG_BDMASTAT, ETH_S_BRxRDF);
    }

    return 0;
}

static const struct eth_ops s3c4510_ether_ops = {
    .start          = s3c4510_eth_init,
    .send           = s3c4510_eth_send,
    .recv           = s3c4510_eth_recv,
    .stop           = s3c4510_eth_halt,
    .free_pkt       = s3c4510_eth_free_pkt,
};

U_BOOT_DRIVER(s3c4510_eth) = {
    .name       = "s3c4510_eth",
    .id         = UCLASS_ETH,
    .ops        = &s3c4510_ether_ops,
    .flags      = 0,
};

#if !CONFIG_IS_ENABLED(OF_CONTROL)
U_BOOT_DRVINFO(s3c4510_eth) = {
    .name = "s3c4510_eth",
};
#endif
