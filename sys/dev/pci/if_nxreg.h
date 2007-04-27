/*	$OpenBSD: src/sys/dev/pci/Attic/if_nxreg.h,v 1.2 2007/04/27 14:46:25 reyk Exp $	*/

/*
 * Copyright (c) 2007 Reyk Floeter <reyk@openbsd.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * NetXen NX2031 register definitions based on:
 * http://www.netxen.com/products/downloads/
 *     Ethernet_Driver_Ref_Guide_Open_Source.pdf
 */

#ifndef _NX_REG_H
#define _NX_REG_H

/*
 * Common definitions
 */

#define NX_MAX_PORTS	4

#define NX_MAX_MTU	ETHER_MTU
#define NX_JUMBO_MTU	8000			/* less than 9k */

/*
 * Hardware descriptors
 */

struct nx_txdesc {
	u_int64_t		tx_next;	/* reserved */
	u_int32_t		tx_addr2_low;	/* low address of buffer 2 */
	u_int32_t		tx_addr2_high;	/* high address of buffer 2 */
	u_int32_t		tx_length;
#define  NX_TXDESC_LENGTH_S	0		/* length */
#define  NX_TXDESC_LENGTH_M	0x00ffffff
#define  NX_TXDESC_TCPOFF_S	24		/* TCP header offset for TSO */
#define  NX_TXDESC_TCPOFF_M	0xff000000
	u_int8_t		tx_ipoff;	/* IP header offset for TSO */
	u_int8_t		tx_nbuf;	/* number of buffers */
	u_int8_t		tx_flags;
#define  NX_TXDESC_F_VLAN	(1<<8)		/* VLAN tagged */
#define  NX_TXDESC_F_TSO	(1<<1)		/* TSO enabled */
#define  NX_TXDESC_F_CKSUM	(1<<0)		/* checksum enabled */
	u_int8_t		tx_opcode;
#define  NX_TXDESC_OP_STOPSTATS	(1<<9)		/* Stop statistics */
#define  NX_TXDESC_OP_GETSTATS	(1<<8)		/* Get statistics */
#define  NX_TXDESC_OP_TX_TSO	(1<<5)		/* TCP packet, do TSO */
#define  NX_TXDESC_OP_TX_IP	(1<<4)		/* IP packet, compute cksum */
#define  NX_TXDESC_OP_TX_UDP	(1<<3)		/* UDP packet, compute cksum */
#define  NX_TXDESC_OP_TX_TCP	(1<<2)		/* TCP packet, compute cksum */
#define  NX_TXDESC_OP_TX	(1<<1)		/* raw Ethernet packet */
	u_int16_t		tx_handle;	/* handle of the buffer */
	u_int16_t		tx_mss;		/* MSS for the packet */
	u_int8_t		tx_port;	/* interface port */
#define  NX_TXDESC_PORT_S	0
#define  NX_TXDESC_PORT_M	0x0f
	u_int8_t		tx_hdrlength;	/* MAC+IP+TCP length for TSO */
	u_int16_t		tx_reserved;
	u_int32_t		tx_addr3_low;	/* low address of buffer 3 */
	u_int32_t		tx_addr3_high;	/* high address of buffer 3 */
	u_int32_t		tx_addr1_low;	/* low address of buffer 1 */
	u_int32_t		tx_addr1_high;	/* high address of buffer 1 */
	u_int16_t		tx_buf1_length;	/* length of buffer 1 */
	u_int16_t		tx_buf2_length;	/* length of buffer 2 */
	u_int16_t		tx_buf3_length;	/* length of buffer 3 */
	u_int16_t		tx_buf4_length;	/* length of buffer 4 */
	u_int32_t		tx_addr4_low;	/* low address of buffer 4 */
	u_int32_t		tx_addr4_high;	/* high address of buffer 4 */
} __packed;	

struct nx_rxdesc {
	u_int16_t		rx_handle;	/* handle of the buffer */
	u_int16_t		rx_reserved;
	u_int32_t		rx_length;	/* length of the buffer */
	u_int64_t		rx_addr;	/* address of the buffer */
} __packed;

struct nx_statusdesc {
	u_int16_t		rx_port;
#define  NX_STSDESC_PORT_S	0		/* interface port */
#define  NX_STSDESC_PORT_M	0x000f
#define  NX_STSDESC_STS_S	4		/* completion status */
#define  NX_STSDESC_STS_M	0x00f0
#define   NX_STSDESC_STS_NOCHK	1		/* checksum not verified */
#define   NX_STSDESC_STS_CHKOK	2		/* checksum verified ok */
#define  NX_STSDESC_TYPE_S	8		/* type/index of the ring */
#define  NX_STSDESC_TYPE_M	0x0f00
#define  NX_STSDESC_OPCODE_S	12		/* opcode */
#define  NX_STSDESC_OPCODE_M	0xf000
#define   NX_STSDESC_OPCODE	0xa		/* received packet */
	u_int16_t		rx_length;	/* total length of the packet */
	u_int16_t		rx_handle;	/* handle of the buffer */
	u_int16_t		rx_owner;
#define  NX_STSDESC_OWNER_S	0		/* owner of the descriptor */
#define  NX_STSDESC_OWNER_M	0x0003
#define   NX_STSDESC_OWNER_HOST	1		/* owner is the host (t.b.d) */
#define   NX_STSDESC_OWNER_CARD	2		/* owner is the card */
#define  NX_STSDESC_PROTO_S	2		/* protocol type */
#define  NX_STSDESC_PROTO_M	0x003c
} __packed;

/*
 * Memory layout
 */

#define NXPCIMEM_SIZE_128MB	0x08000000	/* 128MB size */
#define NXPCIMEM_SIZE_32MB	0x02000000	/* 32MB size */

#define NXPCIMAP_DIRECT_CRB	0x04400000
#define NXPCIMAP_CRB		0x06000000

/*
 * PCI Express Registers
 */

/* Interrupt Vector */
#define NXISR_INT_VECTOR		0x00010100
#define  NXISR_INT_VECTOR_TARGET3	(1<<10)	/* interrupt for function 3 */
#define  NXISR_INT_VECTOR_TARGET2	(1<<9)	/* interrupt for function 2 */
#define  NXISR_INT_VECTOR_TARGET1	(1<<8)	/* interrupt for function 1 */
#define  NXISR_INT_VECTOR_TARGET0	(1<<7)	/* interrupt for function 0 */
#define  NXISR_INT_VECTOR_RC_INT	(1<<5)	/* root complex interrupt */

/* Interrupt Mask */
#define NXISR_INT_MASK			0x00010104
#define  NXISR_INT_MASK_TARGET3		(1<<10)	/* mask for function 3 */
#define  NXISR_INT_MASK_TARGET2		(1<<9)	/* mask for function 2 */
#define  NXISR_INT_MASK_TARGET1		(1<<8)	/* mask for function 1 */
#define  NXISR_INT_MASK_TARGET0		(1<<7)	/* mask for function 0 */
#define  NXISR_INT_MASK_RC_INT		(1<<5)	/* root complex mask */

/*
 * Network Interface Unit (NIU) registers
 */

/* Mode Register (see also NXNIU_RESET_SYS_FIFOS) */
#define NXNIU_MODE			0x00000000
#define  NXNIU_MODE_XGE			(1<<2)	/* XGE interface enabled */
#define  NXNIU_MODE_GBE			(1<<1)	/* 4 GbE interfaces enabled */
#define  NXNIU_MODE_FC			(1<<0)	/* *Fibre Channel enabled */
#define NXNIU_MODE_DEF			NUI_XGE_ENABLE

/* 10G - 1G Mode Enable Register */
#define NXNIU_XG_SINGLE_TERM		0x00000004
#define  NXNIU_XG_SINGLE_TERM_ENABLE	(1<<0)	/* Enable 10G + 1G mode */
#define NXNIU_XG_SINGLE_TERM_DEF	0		/* Disabled */

/* XGE Reset Register */
#define NXNIU_XG_RESET			0x0000001c
#define  NXNIU_XG_RESET_CD		(1<<1)	/* Reset channels CD */
#define  NXNIU_XG_RESET_AB		(1<<0)	/* Reset channels AB */
#define NXNIU_XG_RESET_DEF		(NXNIU_XG_RESET_AB|NXNIU_XG_RESET_CD)

/* Interrupt Mask Register */
#define NXNIU_INT_MASK			0x00000040
#define  NXNIU_INT_MASK_XG		(1<<6)	/* XGE Interrupt Mask */
#define  NXNIU_INT_MASK_RES5		(1<<5)	/* Reserved bit */
#define  NXNIU_INT_MASK_RES4		(1<<4)	/* Reserved bit */
#define  NXNIU_INT_MASK_GB3		(1<<3)	/* GbE 3 Interrupt Mask */
#define  NXNIU_INT_MASK_GB2		(1<<2)	/* GbE 2 Interrupt Mask */
#define  NXNIU_INT_MASK_GB1		(1<<1)	/* GbE 1 Interrupt Mask */
#define  NXNIU_INT_MASK_GB0		(1<<0)	/* GbE 0 Interrupt Mask */
#define NXNIU_INT_MASK_DEF		(				\
	NXNIU_INT_MASK_XG|NXNIU_INT_MASK_RES5|NXNIU_INT_MASK_RES4|	\
	NXNIU_INT_MASK_GB3|NXNIU_INT_MASK_GB2|NXNIU_INT_MASK_GB1|	\
	NXNIU_INT_MASK_GB0)			/* Reserved bits enabled */

/* Reset System FIFOs Register (needed before changing NXNIU_MODE) */
#define NXNIU_RESET_SYS_FIFOS		0x00000088
#define  NXNIU_RESET_SYS_FIFOS_RX	(1<<31)	/* Reset all Rx FIFOs */
#define  NXNIU_RESET_SYS_FIFOS_TX	(1<<0)	/* Reset all Tx FIFOs */
#define NXNIU_RESET_SYS_FIFOS_DEF	0	/* Disabled */

/* XGE Configuration 0 Register */
#define NXNIU_XGE_CONFIG0		0x00070000
#define  NXNIU_XGE_CONFIG0_SOFTRST_FIFO	(1<<31)	/* Soft reset FIFOs */
#define  NXNIU_XGE_CONFIG0_SOFTRST_MAC	(1<<4)	/* Soft reset XGE MAC */
#define  NXNIU_XGE_CONFIG0_RX_ENABLE	(1<<2)	/* Enable frame Rx */
#define  NXNIU_XGE_CONFIG0_TX_ENABLE	(1<<0)	/* Enable frame Tx */
#define NXNIU_XGE_CONFIG0_DEF		0	/* Disabled */

/* XGE Configuration 1 Register */
#define NXNIU_XGE_CONFIG1		0x00070004
#define  NXNIU_XGE_CONFIG1_PROMISC	(1<<13)	/* Pass all Rx frames */
#define  NXNIU_XGE_CONFIG1_MCAST_ENABLE	(1<<12) /* Rx all multicast frames */
#define  NXNIU_XGE_CONFIG1_SEQ_ERROR	(1<<10) /* Sequence error detection */
#define  NXNIU_XGE_CONFIG1_NO_PAUSE	(1<<8)	/* Ignore pause frames */
#define  NXNIU_XGE_CONFIG1_LOCALERR	(1<<6)	/* Wire local error */
#define   NXNIU_XGE_CONFIG1_LOCALERR_FE	0	/* Signal with 0xFE */
#define   NXNIU_XGE_CONFIG1_LOCALERR_I	1	/* Signal with Ierr */
#define  NXNIU_XGE_CONFIG1_NO_MAXSIZE	(1<<5)	/* Ignore max Rx size */
#define  NXNIU_XGE_CONFIG1_CRC_TX	(1<<1)	/* Append CRC to Tx frames */
#define  NXNIU_XGE_CONFIG1_CRC_RX	(1<<0)	/* Remove CRC from Rx frames */
#define NXNIU_XGE_CONFIG1_DEF		0	/* Disabled */

/*
 * Software defined registers (used by the firmware or the driver)
 */

#define NXSW_CMD_PRODUCER_OFFSET	0x2208	/* Producer CMD ring index */
#define NXSW_CMD_CONSUMER_OFFSET	0x220c	/* Consumer CMD ring index */
#define NXSW_RCV_PRODUCER_OFFSET	0x2218	/* Producer Rx ring index */
#define NXSW_RCV_CONSUMER_OFFSET	0x221c	/* Consumer Rx ring index */
#define NXSW_RCV_GLOBAL_RING		0x2220	/* Address of Rx buffer */
#define NXSW_RCV_STATUS_RING		0x2224	/* Address of Rx status ring */
#define NXSW_RCV_STATUS_PRODUCER	0x2228	/* Producer Rx status index */
#define NXSW_RCV_STATUS_CONSUMER	0x222c	/* Consumer Rx status index */
#define NXSW_CMD_ADDR_HI		0x2230	/* CMD ring phys address */
#define NXSW_CMD_ADDR_LO		0x2234	/* CMD ring phys address */
#define NXSW_CMD_RING_SIZE		0x2238	/* Entries in the CMD ring */
#define NXSW_RCV_RING_SIZE		0x223c	/* Entries in the Rx ring */
#define NXSW_JRCV_RING_SIZE		0x2240	/* Entries in the jumbo ring */
#define NXSW_RCVPEG_STATE		0x2248	/* State of the NX2031 */
#define NXSW_CMDPEG_STATE		0x2250	/* State of the firmware */
#define  NXSW_CMDPEG_STATE_INIT_START	0xff00	/* Start of initialization */
#define  NXSW_CMDPEG_STATE_INIT_DONE	0xff01	/* Initialization complete */
#define  NXSW_CMDPEG_STATE_INIT_FAILED	0xffff	/* Initialization failed */
#define NXSW_GLOBAL_INT_COAL		0x2280	/* Interrupt coalescing */
#define NXSW_INT_COAL_MODE		0x2284	/* Reserved */
#define NXSW_MAX_RCV_BUFS		0x2288	/* Interrupt tuning register */
#define NXSW_TX_INT_THRESHOLD		0x228c	/* Interrupt tuning register */
#define NXSW_RX_PKT_TIMER		0x2290	/* Interrupt tuning register */
#define NXSW_TX_PKT_TIMER		0x2294	/* Interrupt tuning register */
#define NXSW_RX_PKT_CNT			0x2298	/* Rx packet count register */
#define NXSW_RX_TMR_CNT			0x229c	/* Rx timer count register */
#define NXSW_XG_STATE			0x22a0	/* PHY state register */
#define  NXSW_XG_LINK_UP		(1<<4)	/* 10G PHY state up */
#define  NXSW_XG_LINK_DOWN		(1<<5)	/* 10G PHY state down */
#define NXSW_JRCV_PRODUCER_OFFSET	0x2300	/* Producer jumbo ring index */
#define NXSW_JRCV_CONSUMER_OFFSET	0x2304	/* Consumer jumbo ring index */
#define NXSW_JRCV_GLOBAL_RING		0x2220	/* Address of jumbo buffer */

/*
 * Secondary Interrupt Registers
 */

/* I2Q Register */
#define NXI2Q_CLR_PCI_HI		0x00000034
#define  NXI2Q_CLR_PCI_HI_PHY		(1<<13)	/* PHY interrupt */
#define NXI2Q_CLR_PCI_HI_DEF		0	/* Cleared */

/* Reset Unit Register */
#define NXROMUSB_GLB_SW_RESET		0x1a100008
#define  NXROMUSB_GLB_SW_RESET_EFC_SIU	(1<<30)	/* EFC_SIU reset */
#define  NXROMUSB_GLB_SW_RESET_NIU	(1<<29)	/* NIU software reset */
#define  NXROMUSB_GLB_SW_RESET_U0QMSQG	(1<<28)	/* Network side QM_SQG reset */
#define  NXROMUSB_GLB_SW_RESET_U1QMSQG	(1<<27)	/* Storage side QM_SQG reset */
#define  NXROMUSB_GLB_SW_RESET_C2C1	(1<<26)	/* Chip to Chip 1 reset */
#define  NXROMUSB_GLB_SW_RESET_C2C0	(1<<25)	/* Chip to Chip 2 reset */
#define  NXROMUSB_GLB_SW_RESET_U1PEGI	(1<<11)	/* Storage Pegasus I-Cache */
#define  NXROMUSB_GLB_SW_RESET_U1PEGD	(1<<10)	/* Storage Pegasus D-Cache */
#define  NXROMUSB_GLB_SW_RESET_U1PEG3	(1<<9)	/* Storage Pegasus3 reset */
#define  NXROMUSB_GLB_SW_RESET_U1PEG2	(1<<8)	/* Storage Pegasus2 reset */
#define  NXROMUSB_GLB_SW_RESET_U1PEG1	(1<<7)	/* Storage Pegasus1 reset */
#define  NXROMUSB_GLB_SW_RESET_U1PEG0	(1<<6)	/* Storage Pegasus0 reset */
#define  NXROMUSB_GLB_SW_RESET_U0PEGI	(1<<11)	/* Network Pegasus I-Cache */
#define  NXROMUSB_GLB_SW_RESET_U0PEGD	(1<<10)	/* Network Pegasus D-Cache */
#define  NXROMUSB_GLB_SW_RESET_U0PEG3	(1<<9)	/* Network Pegasus3 reset */
#define  NXROMUSB_GLB_SW_RESET_U0PEG2	(1<<8)	/* Network Pegasus2 reset */
#define  NXROMUSB_GLB_SW_RESET_U0PEG1	(1<<7)	/* Network Pegasus1 reset */
#define  NXROMUSB_GLB_SW_RESET_U0PEG0	(1<<6)	/* Network Pegasus0 reset */
#define NXROMUSB_GLB_SW_RESET_DEF	0xffffffff

/* Casper Reset Register */
#define NXROMUSB_GLB_CAS_RESET		0x1a100038
#define  NXRUMUSB_GLB_CAS_RESET_ENABLE	(1<<0)	/* Enable Casper reset */
#define NXROMUSB_GLB_CAS_RESET_DEF	0	/* Disabled */

#endif /* _NX_REG_H */
