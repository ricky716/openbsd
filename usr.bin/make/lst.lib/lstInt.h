/*	$OpenBSD: src/usr.bin/make/lst.lib/lstInt.h,v 1.11 2000/06/17 14:43:40 espie Exp $	*/
/*	$NetBSD: lstInt.h,v 1.7 1996/11/06 17:59:44 christos Exp $	*/

/*
 * Copyright (c) 1988, 1989, 1990, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Adam de Boor.
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
 *	from: @(#)lstInt.h	8.1 (Berkeley) 6/6/93
 */

/*-
 * lstInt.h --
 *	Internals for the list library
 */
#ifndef _LSTINT_H_
#define _LSTINT_H_

#include	  "make.h"
#include	  "lst.h"

/*
 * Flags required for synchronization
 */
#define LN_DELETED  	0x0001      /* List node should be removed when done */


/*
 * PAlloc (var, ptype) --
 *	Allocate a pointer-typedef structure 'ptype' into the variable 'var'
 */
#define	PAlloc(var,ptype)	var = (ptype) emalloc (sizeof (*var))

/*
 * LstNodeValid (ln, l) --
 *	Return TRUE if the LstNode ln is valid with respect to l
 */
#define LstNodeValid(ln, l)	((ln) == NULL ? FALSE : TRUE)

/*
 * LstIsEmpty (l) --
 *	TRUE if the list l is empty.
 */
#define LstIsEmpty(l)	((l)->firstPtr == NULL)

#endif /* _LSTINT_H_ */
