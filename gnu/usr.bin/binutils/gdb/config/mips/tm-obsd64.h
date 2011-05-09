/* Target-dependent definitions for OpenBSD/mips64.

   Copyright 2001, 2002, 2004 Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#ifndef TM_OBSD64_H
#define TM_OBSD64_H

#include "solib.h"

#undef IN_SOLIB_DYNSYM_RESOLVE_CODE
#define IN_SOLIB_DYNSYM_RESOLVE_CODE(PC) mips64obsd_in_dynsym_resolve_code (PC)
int mips64obsd_in_dynsym_resolve_code (CORE_ADDR pc);

#endif /* TM_OBSD64_H */
