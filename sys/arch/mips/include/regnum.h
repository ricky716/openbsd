/*	$OpenBSD: src/sys/arch/mips/include/Attic/regnum.h,v 1.2 1998/03/16 09:03:14 pefo Exp $	*/

/*
 * Copyright (c) 1988 University of Utah.
 * Copyright (c) 1992, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * the Systems Programming Group of the University of Utah Computer
 * Science Department and Ralph Campbell.
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
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	from: Utah Hdr: reg.h 1.1 90/07/09
 *	@(#)reg.h	8.2 (Berkeley) 1/11/94
 */

#define STAND_ARG_SIZE		16
#define STAND_FRAME_SIZE	24
#define STAND_RA_OFFSET		20  

/*
 * Location of the saved registers relative to ZERO.
 * Usage is p->p_regs[XX].
 */
#define ZERO	0
#define AST	1
#define V0	2
#define V1	3
#define A0	4
#define A1	5
#define A2	6
#define A3	7
#define T0	8
#define T1	9
#define T2	10
#define T3	11
#define T4	12
#define T5	13
#define T6	14
#define T7	15
#define S0	16
#define S1	17
#define S2	18
#define S3	19
#define S4	20
#define S5	21
#define S6	22
#define S7	23
#define T8	24
#define T9	25
#define K0	26
#define K1	27
#define GP	28
#define SP	29
#define S8	30
#define RA	31
#define	SR	32
#define	PS	SR	/* alias for SR */
#define MULLO	33
#define MULHI	34
#define BADVADDR 35
#define CAUSE	36
#define	PC	37

#define	NUMSAVEREGS 38		/* Number of registers saved in trap */

#define FPBASE	38
#define F0	(FPBASE+0)
#define F1	(FPBASE+1)
#define F2	(FPBASE+2)
#define F3	(FPBASE+3)
#define F4	(FPBASE+4)
#define F5	(FPBASE+5)
#define F6	(FPBASE+6)
#define F7	(FPBASE+7)
#define F8	(FPBASE+8)
#define F9	(FPBASE+9)
#define F10	(FPBASE+10)
#define F11	(FPBASE+11)
#define F12	(FPBASE+12)
#define F13	(FPBASE+13)
#define F14	(FPBASE+14)
#define F15	(FPBASE+15)
#define F16	(FPBASE+16)
#define F17	(FPBASE+17)
#define F18	(FPBASE+18)
#define F19	(FPBASE+19)
#define F20	(FPBASE+20)
#define F21	(FPBASE+21)
#define F22	(FPBASE+22)
#define F23	(FPBASE+23)
#define F24	(FPBASE+24)
#define F25	(FPBASE+25)
#define F26	(FPBASE+26)
#define F27	(FPBASE+27)
#define F28	(FPBASE+28)
#define F29	(FPBASE+29)
#define F30	(FPBASE+30)
#define F31	(FPBASE+31)
#define	FSR	(FPBASE+32)

#ifdef IPCREG
#define	NIPCREG (FSR + 1)
int ipcreg[NIPCREG] = {
	ZERO, AST, V0, V1, A0, A1, A2, A3, T0, T1, T2, T3, T4, T5, T6, T7,
	S0, S1, S2, S3, S4, S5, S6, S7, T8, T9, K0, K1, GP, SP, S8, RA,
	SR, MULLO, MULHI, BADVADDR, CAUSE, PC,
	F0, F1, F2, F3, F4, F5, F6, F7,
	F8, F9, F10, F11, F12, F13, F14, F15,
	F16, F17, F18, F19, F20, F21, F22, F23,
	F24, F25, F26, F27, F28, F29, F30, F31, FSR,
};
#endif
