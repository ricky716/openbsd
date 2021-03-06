divert(-1)
#
# Copyright (c) 1998, 1999 Proofpoint, Inc. and its suppliers.
#	All rights reserved.
# Copyright (c) 1983 Eric P. Allman.  All rights reserved.
# Copyright (c) 1988, 1993
#	The Regents of the University of California.  All rights reserved.
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the sendmail distribution.
#
#
# Definitions for UXP/DS (Fujitsu/ICL DS/90 series)
# Diego R. Lopez, CICA (Seville). 1995
#

divert(0)
VERSIONID(`$Sendmail: uxpds.m4,v 8.17 2013/11/22 20:51:15 ca Exp $')

define(`confDEF_GROUP_ID', `6')
define(`LOCAL_MAILER_PATH', `/usr/ucblib/binmail')dnl
define(`LOCAL_SHELL_FLAGS', `ehuP')dnl
define(`UUCP_MAILER_ARGS', `uux - -r -a$f -gmedium $h!rmail ($u)')dnl
define(`confEBINDIR', `/usr/ucblib')dnl
