/* $OpenBSD: src/sys/arch/armv7/imx/imxuartreg.h,v 1.1 2013/09/06 20:45:54 patrick Exp $ */
/*
 * Copyright (c) 2005 Dale Rahn <drahn@motorola.com>
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

#define		IMXUART_URXD			0x000
#define		IMXUART_RX_ERR			0x4000
#define		IMXUART_RX_OVERRUN		0x2000
#define		IMXUART_RX_FRMERR		0x1000
#define		IMXUART_RX_BRK			0x0800
#define		IMXUART_RX_PRERR		0x0400
#define		IMXUART_RX_PRERR_SH		10
#define		IMXUART_UTXD			0x040
#define		IMXUART_UCR1			0x080
#define		IMXUART_CR1_ADEN		0x8000
#define		IMXUART_CR1_ADBR		0x4000
#define		IMXUART_CR1_TRDYEN		0x2000
#define		IMXUART_CR1_IDEN		0x1000
#define		IMXUART_CR1_ICD			0xc000
#define		IMXUART_CR1_RRDYEN		0x0200
#define		IMXUART_CR1_RXDMAEN		0x0100
#define		IMXUART_CR1_IREN		0x0080
#define		IMXUART_CR1_TXMPTYEN		0x0040
#define		IMXUART_CR1_RTSDEN		0x0020
#define		IMXUART_CR1_SNDBRK		0x0010
#define		IMXUART_CR1_TXDMAEN		0x0008
#define		IMXUART_CR1_ATDMAEN		0x0004
#define		IMXUART_CR1_DOZE		0x0002
#define		IMXUART_CR1_EN			0x0001
#define		IMXUART_UCR2			0x084
#define		IMXUART_CR2_ESCI		0x8000
#define		IMXUART_CR2_IRTS		0x4000
#define		IMXUART_CR2_CTSC		0x2000
#define		IMXUART_CR2_CTS			0x1000
#define		IMXUART_CR2_ESCEN		0x0800
#define		IMXUART_CR2_RTEC		0x0600
#define		IMXUART_CR2_PREN		0x0100
#define		IMXUART_CR2_PROE		0x0080
#define		IMXUART_CR2_STPB		0x0040
#define		IMXUART_CR2_WS			0x0020
#define		IMXUART_CR2_RTSEN		0x0010
#define		IMXUART_CR2_ATEN		0x0008
#define		IMXUART_CR2_TXEN		0x0004
#define		IMXUART_CR2_RXEN		0x0002
#define		IMXUART_CR2_SRTS		0x0001
#define		IMXUART_UCR3			0x088
#define		IMXUART_CR3_DPEC		0xc000
#define		IMXUART_CR3_DTREN		0x2000
#define		IMXUART_CR3_PARERREN		0x1000
#define		IMXUART_CR3_FRAERREN		0x0800
#define		IMXUART_CR3_DSR			0x0400
#define		IMXUART_CR3_DCD			0x0200
#define		IMXUART_CR3_RI			0x0100
#define		IMXUART_CR3_ADNIMP		0x0080
#define		IMXUART_CR3_RXDSEN		0x0040
#define		IMXUART_CR3_AIRINTEN		0x0020
#define		IMXUART_CR3_AWAKEN		0x0010
#define		IMXUART_CR3_DTRDEN		0x0008
#define		IMXUART_CR3_RXDMUXSEL		0x0004
#define		IMXUART_CR3_INVT		0x0002
#define		IMXUART_CR3_ACIEN		0x0001
#define		IMXUART_UCR4			0x08c
#define		IMXUART_CR4_CSTL		0xfc00
#define		IMXUART_CR4_INVR		0x0200
#define		IMXUART_CR4_ENIRI		0x0100
#define		IMXUART_CR4_WKEN		0x0080
#define		IMXUART_CR4_IDDMAEN		0x0040
#define		IMXUART_CR4_IRSC		0x0020
#define		IMXUART_CR4_LPBYP		0x0010
#define		IMXUART_CR4_TCEN		0x0008
#define		IMXUART_CR4_BKEN		0x0004
#define		IMXUART_CR4_OREN		0x0002
#define		IMXUART_CR4_DREN		0x0001
#define		IMXUART_UFCR			0x090
#define		IMXUART_FCR_TXTL_SH		10
#define		IMXUART_FCR_TXTL_M		0x3f
#define		IMXUART_FCR_RFDIV_SH		7
#define		IMXUART_FCR_RFDIV_M		0x07
#define		IMXUART_FCR_RXTL_SH		0
#define		IMXUART_FCR_RXTL_M		0x3f
#define		IMXUART_USR1			0x094
#define		IMXUART_SR1_PARITYERR		0x8000
#define		IMXUART_SR1_RTSS		0x4000
#define		IMXUART_SR1_TRDY		0x2000
#define		IMXUART_SR1_RTSD		0x1000
#define		IMXUART_SR1_ESCF		0x0800
#define		IMXUART_SR1_FRAMERR		0x0400
#define		IMXUART_SR1_RRDY		0x0200
#define		IMXUART_SR1_AGTIM		0x0100
#define		IMXUART_SR1_DTRD		0x0080
#define		IMXUART_SR1_RXDS		0x0040
#define		IMXUART_SR1_AIRINT		0x0020
#define		IMXUART_SR1_AWAKE		0x0010
#define		IMXUART_USR2			0x098
#define		IMXUART_SR2_ADET		0x8000
#define		IMXUART_SR2_TXFE		0x4000
#define		IMXUART_SR2_DTRF		0x2000
#define		IMXUART_SR2_IDLE		0x1000
#define		IMXUART_SR2_ACST		0x0800
#define		IMXUART_SR2_RIDELT		0x0400
#define		IMXUART_SR2_RIIN		0x0200
#define		IMXUART_SR2_IRINT		0x0100
#define		IMXUART_SR2_WAKE		0x0080
#define		IMXUART_SR2_DCDELT		0x0040
#define		IMXUART_SR2_DCDIN		0x0020
#define		IMXUART_SR2_RTSF		0x0010
#define		IMXUART_SR2_TXDC		0x0008
#define		IMXUART_SR2_BRCD		0x0004
#define		IMXUART_SR2_ORE			0x0002
#define		IMXUART_SR2_RDR			0x0001
#define		IMXUART_UESC			0x09c
#define		IMXUART_UTIM			0x0a0
#define		IMXUART_UBIR			0x0a4
#define		IMXUART_UBMR			0x0a8
#define		IMXUART_UBRC			0x0ac
#define		IMXUART_ONEMS			0x0b0
#define		IMXUART_UTS			0x0b4
#define		IMXUART_SPACE			0x0c0
