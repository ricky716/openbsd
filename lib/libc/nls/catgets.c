/*-
 * Copyright (c) 1996 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by J.T. Conklin.
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
 *        This product includes software developed by the NetBSD
 *        Foundation, Inc. and its contributors.
 * 4. Neither the name of The NetBSD Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#if defined(LIBC_SCCS) && !defined(lint)
static char rcsid[] = "$OpenBSD: src/lib/libc/nls/catgets.c,v 1.5 1996/09/15 09:31:22 tholo Exp $";
#endif /* LIBC_SCCS and not lint */

#define _NLS_PRIVATE

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <nl_types.h>

char *
_catgets(catd, set_id, msg_id, s)
	nl_catd catd;
	int set_id;
	int msg_id;
	const char *s;
{
	struct _nls_cat_hdr *cat_hdr;
	struct _nls_set_hdr *set_hdr;
	struct _nls_msg_hdr *msg_hdr;
	int l, u, i, r;

	if (catd == (nl_catd) -1) {
		errno = EBADF;
		return (char *) s;
	}

	cat_hdr = (struct _nls_cat_hdr *) catd->__data; 
	set_hdr = (struct _nls_set_hdr *) ((char *)catd->__data
		+ sizeof(struct _nls_cat_hdr));

	/* binary search, see knuth algorithm b */
	l = 0;
	u = ntohl(cat_hdr->__nsets) - 1;
	while (l <= u) {
		i = (l + u) / 2;
		r = set_id - ntohl(set_hdr[i].__setno);

		if (r == 0) {
			msg_hdr = (struct _nls_msg_hdr *) ((char *)catd->__data
				+ sizeof(struct _nls_cat_hdr)
				+ ntohl(cat_hdr->__msg_hdr_offset));

			l = ntohl(set_hdr[i].__index);
			u = l + ntohl(set_hdr[i].__nmsgs) - 1;
			while (l <= u) {
				i = (l + u) / 2;
				r = msg_id - ntohl(msg_hdr[i].__msgno);
				if (r == 0) {
					return (char *) catd->__data 
					    + sizeof(struct _nls_cat_hdr)
					    + ntohl(cat_hdr->__msg_txt_offset)
					    + ntohl(msg_hdr[i].__offset);
				} else if (r < 0) {
					u = i - 1;
				} else {
					l = i + 1;
				}
			}

			/* not found */
			return (char *) s;

		} else if (r < 0) {
			u = i - 1;
		} else {
			l = i + 1;
		}
	}

	/* not found */
	return (char *) s;
}
