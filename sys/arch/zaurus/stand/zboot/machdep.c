/*	$OpenBSD: src/sys/arch/zaurus/stand/zboot/machdep.c,v 1.3 2005/05/12 05:10:30 uwe Exp $	*/

/*
 * Copyright (c) 2004 Tom Cosgrove
 * Copyright (c) 1997-1999 Michael Shalayeff
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR OR HIS RELATIVES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF MIND, USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "libsa.h"

struct termios otc;

char _alloc_heap[4 * 1024 * 1024];

int debug;

void
machdep(void)
{
	int i, j;
	struct zaurus_boot_probes *pr;
	struct termios t;

	if (tcgetattr(0, &t) == 0) {
		otc = t;
		cfmakeraw(&t);
		(void)tcsetattr(0, TCSAFLUSH, &t);
	}

	/*
	 * The list of probe routines is now in conf.c.
	 */
	for (i = 0; i < nibprobes; i++) {
		pr = &probe_list[i];
		if (pr != NULL) {
			printf("%s: ", pr->name);

			for (j = 0; j < pr->count; j++)
				(*(pr->probes)[j])();
			printf("\n");
		}
	}
}
