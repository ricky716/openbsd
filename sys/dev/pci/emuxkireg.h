/*	$OpenBSD: src/sys/dev/pci/emuxkireg.h,v 1.2 2001/10/24 15:09:28 brad Exp $	*/
/*	$NetBSD: emuxkireg.h,v 1.1 2001/10/17 18:39:41 jdolecek Exp $	*/

/*-
 * Copyright (c) 2001 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Yannick Montulet.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the NetBSD
 *	Foundation, Inc. and its contributors.
 * 4. Neither the name of The NetBSD Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _DEV_PCI_EMUXKIREG_H_
#define _DEV_PCI_EMUXKIREG_H_

/* 
 * Register values for Creative EMU10000. The register values have been
 * taken from GPLed SBLive! header file published by Creative. The comments
 * have been stripped to avoid GPL pollution in kernel. The Creative version
 * including comments is available in Linux 2.4.* kernel as file
 * 	drivers/sound/emu10k1/8010.h
 */
 

#define	EMU_MKSUBREG(sz, idx, reg)	(((sz) << 24) | ((idx) << 16) | (reg))

#define EMU_PTR	0x00
#define  EMU_PTR_CHNO_MASK	0x0000003f
#define  EMU_PTR_ADDR_MASK	0x07ff0000

#define EMU_DATA	0x04

#define EMU_IPR	0x08
#define  EMU_IPR_RATETRCHANGE	0x01000000
#define  EMU_IPR_FXDSP		0x00800000
#define  EMU_IPR_FORCEINT	0x00400000
#define  EMU_PCIERROR		0x00200000
#define  EMU_IPR_VOLINCR	0x00100000
#define  EMU_IPR_VOLDECR	0x00080000
#define  EMU_IPR_MUTE		0x00040000
#define  EMU_IPR_MICBUFFULL	0x00020000
#define  EMU_IPR_MICBUFHALFFULL	0x00010000
#define  EMU_IPR_ADCBUFFULL	0x00008000
#define  EMU_IPR_ADCBUFHALFFULL	0x00004000
#define  EMU_IPR_EFXBUFFULL	0x00002000
#define  EMU_IPR_EFXBUFHALFFULL	0x00001000
#define  EMU_IPR_GPSPDIFSTCHANGE 0x00000800
#define  EMU_IPR_CDROMSTCHANGE	0x00000400
#define  EMU_IPR_INTERVALTIMER	0x00000200
#define  EMU_IPR_MIDITRANSBUFE	0x00000100
#define  EMU_IPR_MIDIRECVBUFE	0x00000080
#define  EMU_IPR_CHANNELLOOP	0x00000040
#define  EMU_IPR_CHNOMASK	0x0000003f

#define EMU_INTE	0x0c

#define  EMU_INTE_VSB_MASK	0xc0000000
#define   EMU_INTE_VSB_220	0x00000000
#define   EMU_INTE_VSB_240	0x40000000
#define   EMU_INTE_VSB_260	0x80000000
#define   EMU_INTE_VSB_280	0xc0000000

#define  EMU_INTE_VMPU_MASK	0x30000000
#define   EMU_INTE_VMPU_300	0x00000000
#define   EMU_INTE_VMPU_310	0x10000000
#define   EMU_INTE_VMPU_320	0x20000000
#define   EMU_INTE_VMPU_330	0x30000000
#define  EMU_INTE_MDMAENABLE	0x08000000
#define  EMU_INTE_SDMAENABLE	0x04000000
#define  EMU_INTE_MPICENABLE	0x02000000
#define  EMU_INTE_SPICENABLE	0x01000000
#define  EMU_INTE_VSBENABLE	0x00800000
#define  EMU_INTE_ADLIBENABLE	0x00400000
#define  EMU_INTE_MPUENABLE	0x00200000
#define  EMU_INTE_FORCEINT	0x00100000
#define  EMU_INTE_MRHANDENABLE	0x00080000
#define  EMU_INTE_SAMPLERATER	0x00002000
#define  EMU_INTE_FXDSPENABLE	0x00001000
#define  EMU_INTE_PCIERRENABLE	0x00000800
#define  EMU_INTE_VOLINCRENABLE	0x00000400
#define  EMU_INTE_VOLDECRENABLE	0x00000200
#define  EMU_INTE_MUTEENABLE	0x00000100
#define  EMU_INTE_MICBUFENABLE	0x00000080
#define  EMU_INTE_ADCBUFENABLE	0x00000040
#define  EMU_INTE_EFXBUFENABLE	0x00000020
#define  EMU_INTE_GPSPDIFENABLE	0x00000010
#define  EMU_INTE_CDSPDIFENABLE	0x00000008
#define  EMU_INTE_INTERTIMERENB	0x00000004
#define  EMU_INTE_MIDITXENABLE	0x00000002
#define  EMU_INTE_MIDIRXENABLE	0x00000001

#define EMU_WC	0x10

#define  EMU_WC_SAMPLECOUNTER_MASK	0x03FFFFC0
#define  EMU_WC_SAMPLECOUNTER EMU_MKSUBREG(20, 6, EMU_WC)
#define  EMU_WC_CURRENTCHANNEL	0x0000003F

#define EMU_HCFG	0x14
#define  EMU_HCFG_LEGACYFUNC_MASK	0xe0000000
#define  EMU_HCFG_LEGACYFUNC_MPU	0x00000000
#define  EMU_HCFG_LEGACYFUNC_SB		0x40000000
#define  EMU_HCFG_LEGACYFUNC_AD		0x60000000
#define  EMU_HCFG_LEGACYFUNC_MPIC	0x80000000
#define  EMU_HCFG_LEGACYFUNC_MDMA	0xa0000000
#define  EMU_HCFG_LEGACYFUNC_SPCI	0xc0000000
#define  EMU_HCFG_LEGACYFUNC_SDMA	0xe0000000
#define  EMU_HCFG_IOCAPTUREADDR		0x1f000000
#define  EMU_HCFG_LEGACYWRITE		0x00800000
#define  EMU_HCFG_LEGACYWORD		0x00400000

#define  EMU_HCFG_LEGACYINT		0x00200000
#define  EMU_HCFG_CODECFMT_MASK		0x00070000
#define  EMU_HCFG_CODECFMT_AC97		0x00000000
#define  EMU_HCFG_CODECFMT_I2S		0x00010000
#define  EMU_HCFG_GPINPUT0		0x00004000
#define  EMU_HCFG_GPINPUT1		0x00002000
#define  EMU_HCFG_GPOUTPUT_MASK		0x00001c00
#define  EMU_HCFG_JOYENABLE    		0x00000200
#define  EMU_HCFG_PHASETRACKENABLE	0x00000100
#define  EMU_HCFG_AC3ENABLE_MASK	0x000000e0
#define  EMU_HCFG_AC3ENABLE_ZVIDEO	0x00000080
#define  EMU_HCFG_AC3ENABLE_CDSPDIF	0x00000040
#define  EMU_HCFG_AC3ENABLE_GPSPDIF	0x00000020
#define  EMU_HCFG_AUTOMUTE		0x00000010
#define  EMU_HCFG_LOCKSOUNDCACHE	0x00000008
#define  EMU_HCFG_LOCKTANKCACHE_MASK 0x00000004
#define  EMU_HCFG_LOCKTANKCACHE EMU_MKSUBREG(1, 2, EMU_HCFG)
#define  EMU_HCFG_MUTEBUTTONENABLE	0x00000002
#define  EMU_HCFG_AUDIOENABLE	0x00000001

#define EMU_MUDATA			0x18
#define EMU_MUCMD			0x19
#define  EMU_MUCMD_RESET		0xff
#define  EMU_MUCMD_ENTERUARTMODE	0x3f

#define EMU_MUSTAT	EMU_MUCMD
#define  EMU_MUSTAT_IRDYN		0x80
#define  EMU_MUSTAT_ORDYN		0x40

#define EMU_TIMER	0x1a
#define  EMU_TIMER_RATE_MASK	0x000003ff
#define  EMU_TIMER_RATE		EMU_MKSUBREG(10, 0, EMU_TIMER)

#define EMU_AC97DATA	0x1c
#define EMU_AC97ADDR	0x1e
#define  EMU_AC97ADDR_RDY	0x80
#define  EMU_AC97ADDR_ADDR	0x7f


#define EMU_CHAN_CPF	0x00

#define  EMU_CHAN_CPF_PITCH_MASK	0xffff0000
#define  EMU_CHAN_CPF_PITCH	EMU_MKSUBREG(16, 16, EMU_CHAN_CPF)

#define  EMU_CHAN_CPF_STEREO_MASK		0x00008000
#define  EMU_CHAN_CPF_STEREO	EMU_MKSUBREG(1, 15, EMU_CHAN_CPF)
#define  EMU_CHAN_CPF_STOP_MASK	0x00004000

#define  EMU_CHAN_CPF_FRACADDRESS_MASK	0x00003fff


#define EMU_CHAN_PTRX	0x01

#define  EMU_CHAN_PTRX_PITCHTARGET_MASK	0xffff0000
#define  EMU_CHAN_PTRX_PITCHTARGET	EMU_MKSUBREG(16, 16, EMU_CHAN_PTRX)

#define  EMU_CHAN_PTRX_FXSENDAMOUNT_A_MASK	0x0000ff00
#define  EMU_CHAN_PTRX_FXSENDAMOUNT_A EMU_MKSUBREG(8, 8, EMU_CHAN_PTRX)

#define  EMU_CHAN_PTRX_FXSENDAMOUNT_B_MASK	0x000000ff
#define  EMU_CHAN_PTRX_FXSENDAMOUNT_B EMU_MKSUBREG(8, 0, EMU_CHAN_PTRX)

#define EMU_CHAN_CVCF	0x02
#define  EMU_CHAN_CVCF_CURRVOL_MASK	0xffff0000

#define  EMU_CHAN_CVCF_CURRVOL	EMU_MKSUBREG(16, 16, EMU_CHAN_CVCF)
#define  EMU_CHAN_CVCF_CURRFILTER_MASK	0x0000ffff

#define  EMU_CHAN_CVCF_CURRFILTER EMU_MKSUBREG(16, 0, EMU_CHAN_CVCF)

#define EMU_CHAN_VTFT	0x03
#define  EMU_CHAN_VTFT_VOLUMETARGET_MASK	0xffff0000

#define  EMU_CHAN_VTFT_VOLUMETARGET	EMU_MKSUBREG(16, 16, EMU_CHAN_VTFT)
#define  EMU_CHAN_VTFT_FILTERTARGET_MASK	0x0000ffff

#define	 EMU_CHAN_VTFT_FILTERTARGET	EMU_MKSUBREG(16, 0, EMU_CHAN_VTFT)

#define EMU_CHAN_Z1		0x05
#define EMU_CHAN_Z2		0x04

#define EMU_CHAN_PSST	0x06
#define  EMU_CHAN_PSST_FXSENDAMOUNT_C_MASK	0xff000000

#define  EMU_CHAN_PSST_FXSENDAMOUNT_C EMU_MKSUBREG(8, 24, EMU_CHAN_PSST)
#define  EMU_CHAN_PSST_LOOPSTARTADDR_MASK	0x00ffffff

#define  EMU_CHAN_PSST_LOOPSTARTADDR  EMU_MKSUBREG(24, 0, EMU_CHAN_PSST)

#define EMU_CHAN_DSL	0x07
#define  EMU_CHAN_DSL_FXSENDAMOUNT_D_MASK	0xff000000

#define  EMU_CHAN_DSL_FXSENDAMOUNT_D  EMU_MKSUBREG(8, 24, EMU_CHAN_DSL)
#define  EMU_CHAN_DSL_LOOPENDADDR_MASK	0x00ffffff

#define  EMU_CHAN_DSL_LOOPENDADDR	 EMU_MKSUBREG(24, 0, EMU_CHAN_DSL)

#define EMU_CHAN_CCCA	0x08

#define  EMU_CHAN_CCCA_RESONANCE		0xf0000000
#define  EMU_CHAN_CCCA_INTERPROMMASK		0x0e000000
#define   EMU_CHAN_CCCA_INTERPROM_0		0x00000000
#define   EMU_CHAN_CCCA_INTERPROM_1		0x02000000
#define   EMU_CHAN_CCCA_INTERPROM_2		0x04000000
#define   EMU_CHAN_CCCA_INTERPROM_3		0x06000000
#define   EMU_CHAN_CCCA_INTERPROM_4		0x08000000
#define   EMU_CHAN_CCCA_INTERPROM_5		0x0a000000
#define   EMU_CHAN_CCCA_INTERPROM_6		0x0c000000
#define   EMU_CHAN_CCCA_INTERPROM_7		0x0e000000

#define   EMU_CHAN_CCCA_8BITSELECT		0x01000000

#define  EMU_CHAN_CCCA_CURRADDR_MASK		0x00ffffff
#define  EMU_CHAN_CCCA_CURRADDR	EMU_MKSUBREG(24, 0, EMU_CHAN_CCCA)

#define EMU_CHAN_CCR	0x09
#define  EMU_CHAN_CCR_CACHEINVALIDSIZE_MASK	0xfe000000

#define  EMU_CHAN_CCR_CACHEINVALIDSIZE EMU_MKSUBREG(7, 25, EMU_CHAN_CCR)
#define  EMU_CHAN_CCR_CACHELOOPFLAG		0x01000000

#define  EMU_CHAN_CCR_INTERLEAVEDSAMPLES	0x00800000

#define  EMU_CHAN_CCR_WORDSIZEDSAMPLES	0x00400000

#define  EMU_CHAN_CCR_READADDRESS_MASK	0x003f0000

#define  EMU_CHAN_CCR_READADDRESS	EMU_MKSUBREG(6, 16, EMU_CHAN_CCR)
#define  EMU_CHAN_CCR_LOOPINVALSIZE		0x0000fe00
#define  EMU_CHAN_CCR_LOOPFLAG		0x00000100

#define  EMU_CHAN_CCR_CACHELOOPADDRHI	0x000000ff

#define EMU_CHAN_CLP	0x0a
#define  EMU_CHAN_CLP_CACHELOOPADDR		0x0000ffff

#define EMU_CHAN_FXRT	0x0b
#define  EMU_CHAN_FXRT_CHANNELA		0x000f0000
#define  EMU_CHAN_FXRT_CHANNELB		0x00f00000
#define  EMU_CHAN_FXRT_CHANNELC		0x0f000000
#define  EMU_CHAN_FXRT_CHANNELD		0xf0000000

#define EMU_CHAN_MAPA	0x0c
#define EMU_CHAN_MAPB	0x0d

#define  EMU_CHAN_MAP_PTE_MASK		0xffffe000
#define  EMU_CHAN_MAP_PTI_MASK		0x00001fff


#define EMU_CHAN_ENVVOL	0x10
#define  EMU_CHAN_ENVVOL_MASK		0x0000ffff


#define EMU_CHAN_ATKHLDV 	0x11
#define  EMU_CHAN_ATKHLDV_PHASE0	0x00008000
#define  EMU_CHAN_ATKHLDV_HOLDTIME_MASK	0x00007f00
#define  EMU_CHAN_ATKHLDV_ATTACKTIME_MASK	0x0000007f


#define EMU_CHAN_DCYSUSV	0x12

#define  EMU_CHAN_DCYSUSV_PHASE1_MASK	0x00008000

#define  EMU_CHAN_DCYSUSV_SUSTAINLEVEL_MASK 0x00007f00
#define  EMU_CHAN_DCYSUSV_CHANNELENABLE_MASK 0x00000080
#define  EMU_CHAN_DCYSUSV_DECAYTIME_MASK	0x0000007f


#define EMU_CHAN_LFOVAL1 	0x13
#define  EMU_CHAN_LFOVAL_MASK		0x0000ffff


#define EMU_CHAN_ENVVAL	0x14
#define  EMU_CHAN_ENVVAL_MASK		0x0000ffff


#define EMU_CHAN_ATKHLDM	0x15
#define  EMU_CHAN_ATKHLDM_PHASE0	0x00008000
#define  EMU_CHAN_ATKHLDM_HOLDTIME	0x00007f00
#define  EMU_CHAN_ATKHLDM_ATTACKTIME	0x0000007f


#define EMU_CHAN_DCYSUSM	0x16
#define  EMU_CHAN_DCYSUSM_PHASE1_MASK		0x00008000
#define  EMU_CHAN_DCYSUSM_SUSTAINLEVEL_MASK	0x00007f00
#define  EMU_CHAN_DCYSUSM_DECAYTIME_MASK	0x0000007f


#define EMU_CHAN_LFOVAL2	0x17
#define  EMU_CHAN_LFOVAL2_MASK		0x0000ffff


#define EMU_CHAN_IP		0x18
#define  EMU_CHAN_IP_MASK			0x0000ffff
#define  EMU_CHAN_IP_UNITY			0x0000e000

#define EMU_CHAN_IFATN	0x19
#define  EMU_CHAN_IFATN_FILTERCUTOFF_MASK	0x0000ff00
#define  EMU_CHAN_IFATN_FILTERCUTOFF EMU_MKSUBREG(8, 8,	EMU_CHAN_IFATN)

#define  EMU_CHAN_IFATN_ATTENUATION_MASK	0x000000ff
#define  EMU_CHAN_IFATN_ATTENUATION	 EMU_MKSUBREG(8, 0, EMU_CHAN_IFATN)


#define EMU_CHAN_PEFE	0x1a
#define  EMU_CHAN_PEFE_PITCHAMOUNT_MASK	0x0000ff00
#define  EMU_CHAN_PEFE_PITCHAMOUNT	EMU_MKSUBREG(8, 8, EMU_CHAN_PEFE)
#define  EMU_CHAN_PEFE_FILTERAMOUNT_MASK	0x000000ff
#define  EMU_CHAN_PEFE_FILTERAMOUNT	EMU_MKSUBREG(8, 0, EMU_CHAN_PEFE)


#define EMU_CHAN_FMMOD	0x1b
#define  EMU_CHAN_FMMOD_MODVIBRATO	0x0000ff00
#define EMU_CHAN_FMMOD_MOFILTER		0x000000ff


#define EMU_CHAN_TREMFRQ	0x1c
#define  EMU_CHAN_TREMFRQ_DEPTH		0x0000ff00


#define EMU_CHAN_FM2FRQ2	0x1d
#define  EMU_CHAN_FM2FRQ2_DEPTH		0x0000ff00
#define  EMU_CHAN_FM2FRQ2_FREQUENCY	0x000000ff


#define EMU_CHAN_TEMPENV	0x1e
#define  EMU_CHAN_TEMPENV_MASK		0x0000ffff

#define EMU_CHAN_CD0	0x20
#define EMU_CHAN_CD1	0x21
#define EMU_CHAN_CD2	0x22
#define EMU_CHAN_CD3	0x23
#define EMU_CHAN_CD4	0x24
#define EMU_CHAN_CD5	0x25
#define EMU_CHAN_CD6	0x26
#define EMU_CHAN_CD7	0x27
#define EMU_CHAN_CD8	0x28
#define EMU_CHAN_CD9	0x29
#define EMU_CHAN_CDA	0x2a
#define EMU_CHAN_CDB	0x2b
#define EMU_CHAN_CDC	0x2c
#define EMU_CHAN_CDD	0x2d
#define EMU_CHAN_CDE	0x2e
#define EMU_CHAN_CDF	0x2f

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

#define EMU_PTB		0x40
#define  EMU_PTB_MASK			0xfffff000

#define EMU_TCB		0x41
#define  EMU_TCB_MASK			0xfffff000

#define EMU_ADCCR		0x42
#define  EMU_ADCCR_RCHANENABLE		0x00000010
#define  EMU_ADCCR_LCHANENABLE		0x00000008

#define  EMU_ADCCR_SAMPLERATE_MASK		0x00000007
#define   EMU_ADCCR_SAMPLERATE_48		0x00000000
#define   EMU_ADCCR_SAMPLERATE_44		0x00000001
#define   EMU_ADCCR_SAMPLERATE_32		0x00000002
#define   EMU_ADCCR_SAMPLERATE_24		0x00000003
#define   EMU_ADCCR_SAMPLERATE_22		0x00000004
#define   EMU_ADCCR_SAMPLERATE_16		0x00000005
#define   EMU_ADCCR_SAMPLERATE_11		0x00000006
#define   EMU_ADCCR_SAMPLERATE_8		0x00000007

#define EMU_FXWC		0x43
#define EMU_TCBS		0x44
#define  EMU_TCBS_MASK			0x00000007
#define   EMU_TCBS_BUFFSIZE_16K		0x00000000
#define   EMU_TCBS_BUFFSIZE_32K		0x00000001
#define   EMU_TCBS_BUFFSIZE_64K		0x00000002
#define   EMU_TCBS_BUFFSIZE_128K		0x00000003
#define   EMU_TCBS_BUFFSIZE_256K		0x00000004
#define   EMU_TCBS_BUFFSIZE_512K		0x00000005
#define   EMU_TCBS_BUFFSIZE_1024K		0x00000006
#define   EMU_TCBS_BUFFSIZE_2048K		0x00000007


#define EMU_MICBA		0x45
#define EMU_ADCBA		0x46
#define EMU_FXBA		0x47

#define  EMU_RECBA_MASK			0xfffff000

#define EMU_MICBS		0x49
#define EMU_ADCBS		0x4a
#define EMU_FXBS		0x4b
#define  EMU_RECBS_BUFSIZE_NONE		0x00000000
#define  EMU_RECBS_BUFSIZE_384		0x00000001
#define  EMU_RECBS_BUFSIZE_448		0x00000002
#define  EMU_RECBS_BUFSIZE_512		0x00000003
#define  EMU_RECBS_BUFSIZE_640		0x00000004
#define  EMU_RECBS_BUFSIZE_768		0x00000005
#define  EMU_RECBS_BUFSIZE_896		0x00000006
#define  EMU_RECBS_BUFSIZE_1024		0x00000007
#define  EMU_RECBS_BUFSIZE_1280		0x00000008
#define  EMU_RECBS_BUFSIZE_1536		0x00000009
#define  EMU_RECBS_BUFSIZE_1792		0x0000000a
#define  EMU_RECBS_BUFSIZE_2048		0x0000000b
#define  EMU_RECBS_BUFSIZE_2560		0x0000000c
#define  EMU_RECBS_BUFSIZE_3072		0x0000000d
#define  EMU_RECBS_BUFSIZE_3584		0x0000000e
#define  EMU_RECBS_BUFSIZE_4096		0x0000000f
#define  EMU_RECBS_BUFSIZE_5120		0x00000010
#define  EMU_RECBS_BUFSIZE_6144		0x00000011
#define  EMU_RECBS_BUFSIZE_7168		0x00000012
#define  EMU_RECBS_BUFSIZE_8192		0x00000013
#define  EMU_RECBS_BUFSIZE_10240	0x00000014
#define  EMU_RECBS_BUFSIZE_12288	0x00000015
#define  EMU_RECBS_BUFSIZE_14366	0x00000016
#define  EMU_RECBS_BUFSIZE_16384	0x00000017
#define  EMU_RECBS_BUFSIZE_20480	0x00000018
#define  EMU_RECBS_BUFSIZE_24576	0x00000019
#define  EMU_RECBS_BUFSIZE_28672	0x0000001a
#define  EMU_RECBS_BUFSIZE_32768	0x0000001b
#define  EMU_RECBS_BUFSIZE_40960	0x0000001c
#define  EMU_RECBS_BUFSIZE_49152	0x0000001d
#define  EMU_RECBS_BUFSIZE_57344	0x0000001e
#define  EMU_RECBS_BUFSIZE_65536	0x0000001f


#define EMU_CDCS	0x50
#define EMU_GPSCS	0x51


#define EMU_DBG	0x52
#define EMU_DBG_ZC			0x80000000
#define  EMU_DBG_SATURATION_OCCURED	0x02000000
#define  EMU_DBG_SATURATION_ADDR	0x01ff0000
#define  EMU_DBG_SINGLE_STEP		0x00008000
#define  EMU_DBG_STEP			0x00004000
#define  EMU_DBG_CONDITION_CODE		0x00003e00
#define  EMU_DBG_SINGLE_STEP_ADDR	0x000001ff
#define EMU_REG53	0x53


#define EMU_SPCS0	0x54
#define EMU_SPCS1	0x55
#define EMU_SPCS2	0x56

#define  EMU_SPCS_CLKACCYMASK		0x30000000
#define   EMU_SPCS_CLKACCY_1000PPM	0x00000000
#define   EMU_SPCS_CLKACCY_50PPM	0x10000000
#define   EMU_SPCS_CLKACCY_VARIABLE	0x20000000

#define  EMU_SPCS_SAMPLERATEMASK	0x0f000000
#define   EMU_SPCS_SAMPLERATE_44	0x00000000
#define   EMU_SPCS_SAMPLERATE_48	0x02000000
#define   EMU_SPCS_SAMPLERATE_32	0x03000000

#define  EMU_SPCS_CHANNELNUMMASK	0x00f00000

#define   EMU_SPCS_CHANNELNUM_UNSPEC	0x00000000
#define   EMU_SPCS_CHANNELNUM_LEFT	0x00100000
#define   EMU_SPCS_CHANNELNUM_RIGHT	0x00200000
#define  EMU_SPCS_SOURCENUMMASK		0x000f0000
#define   EMU_SPCS_SOURCENUM_UNSPEC	0x00000000
#define  EMU_SPCS_GENERATIONSTATUS	0x00008000

#define  EMU_SPCS_CATEGORYCODEMASK	0x00007f00

#define  EMU_SPCS_MODEMASK		0x000000c0
#define  EMU_SPCS_EMPHASISMASK		0x00000038
#define   EMU_SPCS_EMPHASIS_NONE	0x00000000
#define   EMU_SPCS_EMPHASIS_50_15	0x00000008
#define  EMU_SPCS_COPYRIGHT		0x00000004

#define  EMU_SPCS_NOTAUDIODATA		0x00000002

#define  EMU_SPCS_PROFESSIONAL		0x00000001


#define EMU_CLIEL	0x58
#define EMU_CLIEH	0x59
#define EMU_CLIPL	0x5a
#define EMU_CLIPH	0x5b
#define EMU_SOLEL	0x5c
#define EMU_SOLEH	0x5d

#define EMU_SPBYPASS 0x5e
#define  EMU_SPBYPASS_ENABLE		0x00000001


#define EMU_CDSRCS	0x60
#define EMU_GPSRCS	0x61
#define EMU_ZVSRCS	0x62
#define  EMU_SRCS_SPDIFLOCKED		0x02000000
#define  EMU_SRCS_RATELOCKED		0x01000000
#define  EMU_SRCS_ESTSAMPLERATE		0x0007ffff


#define EMU_MICIDX	0x63
#define EMU_ADCIDX	0x64
#define EMU_FXIDX	0x65
#define  EMU_RECIDX_MASK		0x0000ffff
#define	 EMU_RECIDX(idxreg)	       (0x10000000|(idxreg))
/*
#define  EMU_MICIDX_IDX			0x10000063
#define  EMU_ADCIDX_IDX			0x10000064
#define  EMU_FXIDX_IDX			0x10000065
*/

#define EMU_FXGPREGBASE		0x100
#define EMU_TANKMEMDATAREGBASE	0x200
#define  EMU_TANKMEMDATAREG_MASK		0x000fffff

#define EMU_TANKMEMADDRREGBASE	0x300
#define  EMU_TANKMEMADDRREG_ADDR_MASK		0x000fffff
#define  EMU_TANKMEMADDRREG_CLEAR		0x00800000
#define  EMU_TANKMEMADDRREG_ALIGN		0x00400000
#define  EMU_TANKMEMADDRREG_WRITE		0x00200000
#define  EMU_TANKMEMADDRREG_READ		0x00100000


#define  EMU_MICROCODEBASE		0x400

#define  EMU_DSP_LOWORD_OPX_MASK		0x000ffc00
#define  EMU_DSP_LOWORD_OPY_MASK		0x000003ff
#define  EMU_DSP_HIWORD_OPCODE_MASK		0x00f00000
#define  EMU_DSP_HIWORD_RESULT_MASK		0x000ffc00
#define  EMU_DSP_HIWORD_OPA_MASK		0x000003ff


#define	EMU_DSP_OP_MACS		0x0
#define	EMU_DSP_OP_MACS1	0x1
#define	EMU_DSP_OP_MACW		0x2
#define	EMU_DSP_OP_MACW1	0x3
#define	EMU_DSP_OP_MACINTS	0x4
#define	EMU_DSP_OP_MACINTW	0x5
#define	EMU_DSP_OP_ACC3		0x6
#define	EMU_DSP_OP_MACMV	0x7
#define	EMU_DSP_OP_ANDXOR	0x8
#define	EMU_DSP_OP_TSTNEG	0x9
#define	EMU_DSP_OP_LIMIT	0xA
#define	EMU_DSP_OP_LIMIT1	0xB
#define	EMU_DSP_OP_LOG		0xC
#define	EMU_DSP_OP_EXP		0xD
#define	EMU_DSP_OP_INTERP	0xE
#define	EMU_DSP_OP_SKIP	0xF


#define	EMU_DSP_FX(num)	(num)


#define	EMU_DSP_IOL(base, num)	(base + (num << 1))
#define	EMU_DSP_IOR(base, num)	(EMU_DSP_IOL(base, num) + 1)

#define	EMU_DSP_INL_BASE	0x010
#define	EMU_DSP_INL(num)	(EMU_DSP_IOL(EMU_DSP_INL_BASE, num))
#define	EMU_DSP_INR(num)	(EMU_DSP_IOR(EMU_DSP_INL_BASE, num))
#define	 EMU_DSP_IN_AC97	0
#define	 EMU_DSP_IN_CDSPDIF	1
#define  EMU_DSP_IN_ZOOM	2
#define	 EMU_DSP_IN_TOSOPT	3
#define	 EMU_DSP_IN_LVDLM1	4
#define	 EMU_DSP_IN_LVDCOS	5
#define	 EMU_DSP_IN_LVDLM2	6
#define	EMU_DSP_IN_UNKOWN	7

#define	EMU_DSP_OUTL_BASE	0x020
#define	EMU_DSP_OUTL(num)	(EMU_DSP_IOL(EMU_DSP_OUTL_BASE, num))
#define	EMU_DSP_OUTR(num)	(EMU_DSP_IOR(EMU_DSP_OUTL_BASE, num))
#define	 EMU_DSP_OUT_AC97	 0
#define	 EMU_DSP_OUT_TOSOPT	 1
#define	 EMU_DSP_OUT_UNKNOWN 2
#define	 EMU_DSP_OUT_HEAD	 3
#define	 EMU_DSP_OUT_RCHAN	 4
#define  EMU_DSP_OUT_ADC	 5
#define	  EMU_DSP_OUTL_MIC	 6


#define	EMU_DSP_CST_BASE	0x40
#define	EMU_DSP_CST(num)	(EMU_DSP_CST_BASE + num)
/*
00	= 0x00000000
01	= 0x00000001
02	= 0x00000002
03	= 0x00000003
04	= 0x00000004
05	= 0x00000008
06	= 0x00000010
07	= 0x00000020
08	= 0x00000100
09	= 0x00010000
0A	= 0x00080000
0B	= 0x10000000
0C	= 0x20000000
0D	= 0x40000000
0E	= 0x80000000
0F	= 0x7FFFFFFF
10	= 0xFFFFFFFF
11	= 0xFFFFFFFE
12	= 0xC0000000
13	= 0x4F1BBCDC
14	= 0x5A7EF9DB
15	= 0x00100000
*/

#define	EMU_DSP_HWR_ACC	0x056
#define EMU_DSP_HWR_CCR	0x057
#define	 EMU_DSP_HWR_CCR_S	0x04
#define	 EMU_DSP_HWR_CCR_Z	0x03
#define	 EMU_DSP_HWR_CCR_M	0x02
#define	 EMU_DSP_HWR_CCR_N	0x01
#define	 EMU_DSP_HWR_CCR_B	0x00
#define	EMU_DSP_HWR_NOISE0	0x058
#define	EMU_DSP_HWR_NOISE1	0x059
#define	EMU_DSP_HWR_INTR	0x05A
#define	EMU_DSP_HWR_DBAC	0x05B

#define EMU_DSP_GPR(num)	(EMU_FXGPREGBASE + num)

#endif /* _DEV_PCI_EMUXKIREG_H_ */
