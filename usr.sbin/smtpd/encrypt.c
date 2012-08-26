/*	$OpenBSD: src/usr.sbin/smtpd/Attic/encrypt.c,v 1.2 2012/08/26 10:17:13 chl Exp $	*/

/*
 * Copyright (c) 2012 Charles Longeau <chl@openbsd.org>
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
#include <sys/queue.h>
#include <sys/tree.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/stat.h>

#include <imsg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "smtpd.h"
#include "log.h"


int
encrypt_file(int fdin, int fdout)
{
	return (1);
}

int
decrypt_file(int fdin, int fdout)
{
	return (1);
}

size_t
encrypt_buffer(const char *ib, size_t iblen, char *ob, size_t oblen)
{
	return (1);
}

size_t
decrypt_buffer(const char *ib, size_t iblen, char *ob, size_t oblen)
{
	return (1);
}
