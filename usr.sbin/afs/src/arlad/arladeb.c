/*	$OpenBSD: src/usr.sbin/afs/src/arlad/Attic/arladeb.c,v 1.2 1999/04/30 01:59:06 art Exp $	*/
/*
 * Copyright (c) 1995, 1996, 1997, 1998 Kungliga Tekniska H�gskolan
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
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by the Kungliga Tekniska
 *      H�gskolan and its contributors.
 * 
 * 4. Neither the name of the Institute nor the names of its contributors
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

#include "arla_local.h"

RCSID("$KTH: arladeb.c,v 1.15 1999/04/20 20:58:07 map Exp $");

static Log_method* arla_log_method = NULL;

#define all (ADEBERROR | ADEBWARN | ADEBDISCONN | ADEBFBUF |		\
	     ADEBMSG | ADEBKERNEL | ADEBCLEANER | ADEBCALLBACK |	\
	     ADEBCM | ADEBVOLCACHE | ADEBFCACHE | ADEBINIT |		\
	     ADEBCONN | ADEBMISC | ADEBVLOG)

#define DEFAULT_LOG (ADEBWARN | ADEBERROR)

struct units arla_deb_units[] = {
    { "all",		all},
    { "almost-all",	all & ~ADEBCLEANER},
    { "errors",		ADEBERROR },
    { "warnings",	ADEBWARN },
    { "disconn",	ADEBDISCONN },
    { "fbuf",		ADEBFBUF },
    { "messages",	ADEBMSG },
    { "kernel",		ADEBKERNEL },
    { "cleaner",	ADEBCLEANER },
    { "callbacks",	ADEBCALLBACK },
    { "cache-manager",	ADEBCM },
    { "volume-cache",	ADEBVOLCACHE },
    { "file-cache",	ADEBFCACHE },
    { "initialization",	ADEBINIT },
    { "connection",	ADEBCONN },
    { "miscellaneous",	ADEBMISC },
    { "venuslog",	ADEBVLOG },
    { "default",	DEFAULT_LOG },
    { "none",		0 },
    { NULL }
};

void
arla_log(unsigned level, char *fmt, ...)
{
    va_list args;
    
    assert (arla_log_method);
    
    va_start(args, fmt);
    log_vlog(arla_log_method, level, fmt, args);
    va_end(args);
}

void
arla_loginit(char *log)
{
    assert (log);
    
    arla_log_method = log_open("arla", log);
    log_set_mask(arla_log_method, DEFAULT_LOG);
}

int
arla_log_set_level (const char *s)
{
    int ret;

    ret = parse_flags (s, arla_deb_units, log_get_mask(arla_log_method));
    if (ret < 0)
	return ret;
    else {
	log_set_mask (arla_log_method, ret);
	return 0;
    }
}

void
arla_log_set_level_num (unsigned level)
{
    log_set_mask (arla_log_method, level);
}

void
arla_log_get_level (char *s, size_t len)
{
    unparse_flags (log_get_mask (arla_log_method),
		   arla_deb_units, s, len);
}

unsigned
arla_log_get_level_num (void)
{
    return log_get_mask (arla_log_method);
}

void
arla_log_print_levels (FILE *f)
{
    print_flags_table (arla_deb_units, f);
}

/*
 *
 */

void
arla_err (int eval, unsigned level, int error, const char *fmt, ...)
{
    va_list args;

    va_start (args, fmt);
    arla_verr (eval, level, error, fmt, args);
    va_end (args);
}

void
arla_verr (int eval, unsigned level, int error, const char *fmt, va_list args)
{
    char *s;

    vasprintf (&s, fmt, args);
    if (s == NULL) {
	log_log (arla_log_method, level,
		 "Sorry, no memory to print `%s'...", fmt);
	exit (eval);
    }
    log_log (arla_log_method, level, "%s: %s", s, koerr_gettext (error));
    free (s);
    exit (eval);
}

void
arla_errx (int eval, unsigned level, const char *fmt, ...)
{
    va_list args;

    va_start (args, fmt);
    arla_verrx (eval, level, fmt, args);
    va_end (args);
}

void
arla_verrx (int eval, unsigned level, const char *fmt, va_list args)
{
    log_vlog (arla_log_method, level, fmt, args);
    exit (eval);
}

void
arla_warn (unsigned level, int error, const char *fmt, ...)
{
    va_list args;

    va_start (args, fmt);
    arla_vwarn (level, error, fmt, args);
    va_end (args);
}

void
arla_vwarn (unsigned level, int error, const char *fmt, va_list args)
{
    char *s;

    vasprintf (&s, fmt, args);
    if (s == NULL) {
	log_log (arla_log_method, level,
		 "Sorry, no memory to print `%s'...", fmt);
	return;
    }
    log_log (arla_log_method, level, "%s: %s", s, koerr_gettext (error));
    free (s);
}

void
arla_warnx (unsigned level, const char *fmt, ...)
{
    va_list args;

    va_start (args, fmt);
    arla_vwarnx (level, fmt, args);
    va_end (args);
}

void
arla_vwarnx (unsigned level, const char *fmt, va_list args)
{
    log_vlog (arla_log_method, level, fmt, args);
}
