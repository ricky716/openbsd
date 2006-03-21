/*	$OpenBSD: src/sys/crypto/michael.c,v 1.1 2006/03/21 18:40:54 reyk Exp $	*/

/*
 * Copyright (c) 2005, 2006 Reyk Floeter <reyk@openbsd.org>
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

/*
 * Implementation of the Michael MIC as defined in IEEE 802.11i for TKIP.
 * The MIC generates a 64bit digest, which shouldn't be used for any other
 * applications except TKIP.
 */

#include <sys/param.h>
#include <sys/systm.h>

#include <crypto/michael.h>

#define ROL(n, x)	(((x) << (n)) | ((x) >> (32 - (n))))
#define ROR(n, x)	(((x) >> (n)) | ((x) << (32 - (n))))
#define VAL32(x)	(*((u_int32_t *)(x)))
#define XSWAP(x)	(((x) & 0xff00ff00UL) >> 8) | ((((x) & 0x00ff00ffUL) << 8))

#define MICHAEL_BLOCK(l, r) do {						\
	r ^= ROL(17, l);							\
	l += r;									\
	r ^= XSWAP(l);								\
	l += r;									\
	r ^= ROL(3, l);								\
	l += r;									\
	r ^= ROR(2, l);								\
	l += r;									\
} while (0)

void
michael_init(MICHAEL_CTX *ctx)
{
	bzero(ctx, sizeof(MICHAEL_CTX));
}

void
michael_update(MICHAEL_CTX *ctx, const u_int8_t *data, u_int len)
{
	int i;

	for (i = 0; i < len; i++) {
		ctx->michael_state |= data[i] << (ctx->michael_count << 3);
		ctx->michael_count++;

		if (ctx->michael_count >= MICHAEL_RAW_BLOCK_LENGTH) {
			ctx->michael_l ^= ctx->michael_state;
			MICHAEL_BLOCK(ctx->michael_l, ctx->michael_r);
			ctx->michael_state = ctx->michael_count = 0;
		}
	}
}

void
michael_final(u_int8_t digest[MICHAEL_DIGEST_LENGTH], MICHAEL_CTX *ctx)
{
	u_int8_t pad[] = { 0x5a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

	michael_update(ctx, pad, sizeof(pad));

	VAL32(digest) = letoh32(ctx->michael_l);
	VAL32(digest + MICHAEL_RAW_BLOCK_LENGTH) = letoh32(ctx->michael_r);
}

void
michael_key(const u_int8_t *key, MICHAEL_CTX *ctx)
{
	ctx->michael_l = ctx->michael_key[0] =
	    htole32(VAL32(key));
	ctx->michael_r = ctx->michael_key[1] =
	    htole32(VAL32(key + MICHAEL_RAW_BLOCK_LENGTH));
}
