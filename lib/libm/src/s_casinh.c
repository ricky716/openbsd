/*	$OpenBSD: src/lib/libm/src/s_casinh.c,v 1.5 2013/03/28 18:09:38 martynas Exp $	*/
/*
 * Copyright (c) 2008 Stephen L. Moshier <steve@moshier.net>
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

/*							casinh
 *
 *	Complex inverse hyperbolic sine
 *
 *
 *
 * SYNOPSIS:
 *
 * double complex casinh();
 * double complex z, w;
 *
 * w = casinh (z);
 *
 *
 *
 * DESCRIPTION:
 *
 * casinh z = -i casin iz .
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE      -10,+10     30000       1.8e-14     2.6e-15
 *
 */

#include <complex.h>
#include <float.h>
#include <math.h>

double complex
casinh(double complex z)
{
	double complex w;

	w = -1.0 * I * casin (z * I);
	return (w);
}

#if	LDBL_MANT_DIG == 53
__strong_alias(casinhl, casinh);
#endif	/* LDBL_MANT_DIG == 53 */
