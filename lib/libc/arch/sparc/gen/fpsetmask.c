/*
 * Written by J.T. Conklin, Apr 10, 1995
 * Public domain.
 */

#if defined(LIBC_SCCS) && !defined(lint)
static char rcsid[] = "$OpenBSD: src/lib/libc/arch/sparc/gen/fpsetmask.c,v 1.2 1996/08/19 08:17:32 tholo Exp $";
#endif /* LIBC_SCCS and not lint */

#include <ieeefp.h>

fp_except
fpsetmask(mask)
	fp_except mask;
{
	fp_except old;
	fp_except new;

	__asm__("st %%fsr,%0" : "=m" (*&old));

	new = old;
	new &= ~(0x1f << 23); 
	new |= ((mask & 0x1f) << 23);

	__asm__("ld %0,%%fsr" : : "m" (*&new));

	return (old >> 23) & 0x1f;
}
