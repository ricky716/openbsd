/*
 * Copyright (c) 1999 Kungliga Tekniska H�gskolan
 * (Royal Institute of Technology, Stockholm, Sweden). 
 * All rights reserved. 
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met: 
 *
 * 1. Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer. 
 *
 * 2. Redistributions in binary form must reproduce the above copyright 
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the distribution. 
 *
 * 3. Neither the name of the Institute nor the names of its contributors 
 *    may be used to endorse or promote products derived from this software 
 *    without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND 
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE 
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS 
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY 
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF 
 * SUCH DAMAGE. 
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
RCSID("$KTH: parse_bytes.c,v 1.3 2001/09/04 09:56:00 assar Exp $");
#endif

#include <parse_units.h>
#include "parse_bytes.h"

static units bytes_units[] = {
    { "gigabyte", 1024 * 1024 * 1024 },
    { "gbyte", 1024 * 1024 * 1024 },
    { "GB", 1024 * 1024 * 1024 },
    { "megabyte", 1024 * 1024 },
    { "mbyte", 1024 * 1024 },
    { "MB", 1024 * 1024 },
    { "kilobyte", 1024 },
    { "KB", 1024 },
    { "byte", 1 },
    { NULL, 0 }
};

static units bytes_short_units[] = {
    { "GB", 1024 * 1024 * 1024 },
    { "MB", 1024 * 1024 },
    { "KB", 1024 },
    { NULL, 0 }
};

int
parse_bytes (const char *s, const char *def_unit)
{
    return parse_units (s, bytes_units, def_unit);
}

int
unparse_bytes (int t, char *s, size_t len)
{
    return unparse_units (t, bytes_units, s, len);
}

int
unparse_bytes_short (int t, char *s, size_t len)
{
    return unparse_units_approx (t, bytes_short_units, s, len);
}
