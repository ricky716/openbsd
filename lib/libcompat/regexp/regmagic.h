/*	$OpenBSD: src/lib/libcompat/regexp/Attic/regmagic.h,v 1.2 1996/07/24 05:39:12 downsj Exp $	*/
/*	$Id: regmagic.h,v 1.1.1.1 1995/10/18 08:42:35 deraadt Exp $ */

/*
 * The first byte of the regexp internal "program" is actually this magic
 * number; the start node begins in the second byte.
 */
#define	MAGIC	0234
