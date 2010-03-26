/*	$Id: man_hash.c,v 1.7 2009/10/19 10:20:24 schwarze Exp $ */
/*
 * Copyright (c) 2008, 2009 Kristaps Dzonsons <kristaps@kth.se>
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
#include <sys/types.h>

#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include "libman.h"

#define	HASH_DEPTH	 6

#define	HASH_ROW(x) do { \
		if ('.' == (x)) \
			(x) = 26; \
		else if (isupper((u_char)(x))) \
			(x) -= 65; \
		else \
			(x) -= 97; \
		(x) *= HASH_DEPTH; \
	} while (/* CONSTCOND */ 0)

/*
 * Lookup table is indexed first by lower-case first letter (plus one
 * for the period, which is stored in the last row), then by lower or
 * uppercase second letter.  Buckets correspond to the index of the
 * macro (the integer value of the enum stored as a char to save a bit
 * of space).
 */
static	u_char		 table[27 * HASH_DEPTH];

/*
 * XXX - this hash has global scope, so if intended for use as a library
 * with multiple callers, it will need re-invocation protection.
 */
void
man_hash_init(void)
{
	int		 i, j, x;

	memset(table, UCHAR_MAX, sizeof(table));

	assert(/* CONSTCOND */ MAN_MAX < UCHAR_MAX);

	for (i = 0; i < MAN_MAX; i++) {
		x = man_macronames[i][0];

		assert(isalpha((u_char)x) || '.' == x);

		HASH_ROW(x);

		for (j = 0; j < HASH_DEPTH; j++)
			if (UCHAR_MAX == table[x + j]) {
				table[x + j] = (u_char)i;
				break;
			}

		assert(j < HASH_DEPTH);
	}
}


enum mant
man_hash_find(const char *tmp)
{
	int		 x, y, i;
	enum mant	 tok;

	if ('\0' == (x = tmp[0]))
		return(MAN_MAX);
	if ( ! (isalpha((u_char)x) || '.' == x))
		return(MAN_MAX);

	HASH_ROW(x);

	for (i = 0; i < HASH_DEPTH; i++) {
		if (UCHAR_MAX == (y = table[x + i]))
			return(MAN_MAX);

		tok = (enum mant)y;
		if (0 == strcmp(tmp, man_macronames[tok]))
			return(tok);
	}

	return(MAN_MAX);
}
