/*	$OpenBSD: src/sys/arch/i386/include/conf.h,v 1.13 2007/11/25 15:42:15 tedu Exp $	*/
/*	$NetBSD: conf.h,v 1.2 1996/05/05 19:28:34 christos Exp $	*/

/*
 * Copyright (c) 1996 Christos Zoulas.  All rights reserved.
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
 *	This product includes software developed by Christos Zoulas.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/conf.h>

#define	mmread	mmrw
#define	mmwrite	mmrw
cdev_decl(mm);
cdev_decl(pms);

bdev_decl(fd);
cdev_decl(fd);

/* open, close, read, write, ioctl, tty, mmap */
#define cdev_pc_init(c,n) { \
	dev_init(c,n,open), dev_init(c,n,close), dev_init(c,n,read), \
	dev_init(c,n,write), dev_init(c,n,ioctl), dev_init(c,n,stop), \
	dev_init(c,n,tty), ttpoll, dev_init(c,n,mmap), D_TTY }

cdev_decl(pc);


#define	cdev_acpiapm_init(c,n) {\
	dev_init(c,n,open), dev_init(c,n,close), (dev_type_read((*))) enodev, \
	(dev_type_write((*))) enodev, dev_init(c,n,ioctl), \
	(dev_type_stop((*))) enodev, 0, (dev_type_poll((*))) enodev, \
	(dev_type_mmap((*))) enodev, 0, D_KQFILTER, dev_init(c,n,kqfilter) }

cdev_decl(spkr);

cdev_decl(mms);

cdev_decl(lms);

cdev_decl(pms);

cdev_decl(joy);

#define biospoll seltrue
cdev_decl(bios);

cdev_decl(acpi);

cdev_decl(apm);

cdev_decl(acpiapm);

#define pctrpoll seltrue
cdev_decl(pctr);
