/*	$OpenBSD: src/usr.bin/lex/libmain.c,v 1.7 2012/12/05 23:20:25 deraadt Exp $	*/

/* libmain - flex run-time support library "main" function */

/* $Header: /cvs/src/usr.bin/lex/libmain.c,v 1.6 2003/07/28 20:38:31 deraadt Exp $ */


int yylex(void);
int main(int, char **);

/* ARGSUSED */
int
main(int argc, char *argv[])
{
	while (yylex() != 0)
		;

	return 0;
}
