/*	$OpenBSD: src/usr.bin/logname/logname.c,v 1.6 2003/06/10 22:20:47 deraadt Exp $	*/
/*	$NetBSD: logname.c,v 1.6 1994/12/22 06:39:32 jtc Exp $	*/

/*-
 * Copyright (c) 1991, 1993, 1994
 *	The Regents of the University of California.  All rights reserved.
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
 */

#ifndef lint
static char copyright[] =
"@(#) Copyright (c) 1991, 1993, 1994\n\
	The Regents of the University of California.  All rights reserved.\n";
#endif /* not lint */

#ifndef lint
#if 0
static char sccsid[] = "@(#)logname.c	8.2 (Berkeley) 4/3/94";
#endif
static char rcsid[] = "$OpenBSD: src/usr.bin/logname/logname.c,v 1.6 2003/06/10 22:20:47 deraadt Exp $";
#endif /* not lint */

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <unistd.h>
#include <err.h>

void usage(void);

int
main(int argc, char *argv[])
{
	int ch;
	char *p;

	setlocale(LC_ALL, "");

	while ((ch = getopt(argc, argv, "")) != -1)
		switch (ch) {
		case '?':
		default:
			usage();
			/* NOTREACHED */
		}

	if (argc != optind) {
		usage();
		/* NOTREACHED */
	}

	if ((p = getlogin()) == NULL)
		err(1, NULL);
	(void)printf("%s\n", p);
	exit(0);
}

void
usage(void)
{
	(void)fprintf(stderr, "usage: logname\n");
	exit(1);
}
