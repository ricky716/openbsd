/*	$OpenBSD: src/usr.sbin/relayctl/parser.h,v 1.1 2006/12/16 11:45:07 reyk Exp $	*/

/*
 * Copyright (c) 2006 Pierre-Yves Ritschard <pyr@spootnik.org>
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

enum actions {
	NONE,
	SHOW_SUM,
	SERV_DISABLE,
	SERV_ENABLE,
	TABLE_DISABLE,
	TABLE_ENABLE,
	HOST_DISABLE,
	HOST_ENABLE,
	SHUTDOWN,
	RELOAD
};

struct parse_result {
	objid_t		id;
	enum actions	action;
};

struct parse_result	*parse(int, char *[]);
const struct token      *match_token(const char *, const struct token []);
void                     show_valid_args(const struct token []);
