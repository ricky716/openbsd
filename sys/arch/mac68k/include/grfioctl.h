/*	$OpenBSD: src/sys/arch/mac68k/include/Attic/grfioctl.h,v 1.6 2005/08/06 19:51:44 martin Exp $	*/
/*	$NetBSD: grfioctl.h,v 1.5 1995/07/02 05:26:45 briggs Exp $	*/

/*
 * Copyright (c) 1988 University of Utah.
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * the Systems Programming Group of the University of Utah Computer
 * Science Department.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
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
 * from: Utah $Hdr: grfioctl.h 1.1 90/07/09$
 *
 *	@(#)grfioctl.h	7.2 (Berkeley) 11/4/90
 */

#ifndef _MAC68K_GRFIOCTL_H_
#define _MAC68K_GRFIOCTL_H_

struct grfmode {
	u_int8_t	mode_id;	/* Identifier for mode              */
	caddr_t		fbbase;		/* Base of page of frame buffer     */
	u_int32_t	fbsize;		/* Size of frame buffer             */
	u_int16_t	fboff;		/* Offset of frame buffer from base */
	u_int16_t	rowbytes;	/* Screen rowbytes                  */
	u_int16_t	width;		/* Screen width                     */
	u_int16_t	height;		/* Screen height                    */
	u_int16_t	hres;		/* Horizontal resolution (dpi)      */
	u_int16_t	vres;		/* Vertical resolution (dpi)        */
	u_int16_t	ptype;		/* 0 = indexed, 0x10 = direct       */
	u_int16_t	psize;		/* Screen depth                     */
	char		pad[32];	/* Space for expansion              */
};

struct grfmodes {
	int		nmodes;		/* Number of modes in modelist */
	struct grfmode	*modelist;	/* Pointer to space for modes */
};

/*
 * BSD ioctls (first few match HP-UX ioctl()s.  In case we want
 * compatibility later, start our own at 16).
 */
#define	GRFIOCON	_IO('G', 1)		/* turn graphics on */
#define	GRFIOCOFF	_IO('G', 2)		/* turn graphics off */
#define GRFIOCLISTMODES	_IOWR('G', 16, struct grfmodes) /* Get list of modes */
#define GRFIOCGETMODE	_IOR('G', 17, int)	/* Get list of modes */
#define GRFIOCSETMODE	_IOW('G', 18, int)	/* Set to mode_id mode */
#define GRFIOCGMODE	_IOR('G', 19, struct grfmode)	/* Get list of modes */

#endif	/* _MAC68K_GRFIOCTL_H_ */
