/*	$OpenBSD: src/sys/arch/amd64/amd64/mutex.S,v 1.9 2013/06/02 01:55:52 kettenis Exp $	*/

/*
 * Copyright (c) 2004 Artur Grabowski <art@openbsd.org>
 * All rights reserved. 
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met: 
 *
 * 1. Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer. 
 * 2. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL  DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#include "assym.h"

#include <machine/param.h>
#include <machine/asm.h>
#include <machine/segments.h>
#include <machine/specialreg.h>
#include <machine/trap.h>
#include <machine/frameasm.h>

/*
 * Yeah, we don't really need to implement mtx_init here, but let's keep
 * all the functions in the same place.
 */
ENTRY(__mtx_init)
	movl	%esi, MTX_WANTIPL(%rdi)
	movl	$0, MTX_OLDIPL(%rdi)
	movq	$0, MTX_OWNER(%rdi)
	ret

ENTRY(mtx_enter)
1:	movl	MTX_WANTIPL(%rdi), %eax
	movq	CPUVAR(SELF), %rcx
	movl	CPU_INFO_ILEVEL(%rcx), %edx	# oipl = cpl;
	cmpl	%eax, %edx			# if (cpl < mtx->mtx_wantipl)
	cmovge	%edx, %eax
	movl	%eax, CPU_INFO_ILEVEL(%rcx)	#	cpl = mtx->mtx_wantipl;
	/*
	 * %edx - the old ipl
	 * %rcx - curcpu()
	 */
	xorq	%rax, %rax
#ifdef MULTIPROCESSOR
	lock
#endif
	cmpxchgq	%rcx, MTX_OWNER(%rdi)	# test_and_set(mtx->mtx_owner)
	jne	2f
	movl	%edx, MTX_OLDIPL(%rdi)
#ifdef DIAGNOSTIC
	incl	CPU_INFO_MUTEX_LEVEL(%rcx)
#endif
	ret

	/* We failed to obtain the lock. splx, spin and retry. */
2:	pushq	%rdi
	movl	%edx, %edi
	call	_C_LABEL(spllower)
	popq	%rdi
#ifdef DIAGNOSTIC
	movq	CPUVAR(SELF), %rcx
	cmpq	MTX_OWNER(%rdi), %rcx
	je	4f
#endif
3:
	movq	MTX_OWNER(%rdi), %rax
	testq	%rax, %rax
	jz	1b
	jmp	3b
#ifdef DIAGNOSTIC
4:	movq	$5f, %rdi
	call	_C_LABEL(panic)
5:	.asciz	"mtx_enter: locking against myself"
#endif

ENTRY(mtx_enter_try)
1:	movl	MTX_WANTIPL(%rdi), %eax
	movq	CPUVAR(SELF), %rcx
	movl	CPU_INFO_ILEVEL(%rcx), %edx	# oipl = cpl;
	cmpl	%eax, %edx			# if (cpl < mtx->mtx_wantipl)
	cmovge	%edx, %eax
	movl	%eax, CPU_INFO_ILEVEL(%rcx)	#	cpl = mtx->mtx_wantipl;
	/*
	 * %edx - the old ipl
	 * %rcx - curcpu()
	 */
	xorq	%rax, %rax
#ifdef MULTIPROCESSOR
	lock
#endif
	cmpxchgq	%rcx, MTX_OWNER(%rdi)	# test_and_set(mtx->mtx_owner)
	jne	2f
	movl	%edx, MTX_OLDIPL(%rdi)
#ifdef DIAGNOSTIC
	incl	CPU_INFO_MUTEX_LEVEL(%rcx)
#endif
	movq	$1, %rax
	ret

	/* We failed to obtain the lock. splx and return 0. */
2:	pushq	%rdi
	movl	%edx, %edi
	call	_C_LABEL(spllower)
	popq	%rdi
#ifdef DIAGNOSTIC
	movq	CPUVAR(SELF), %rcx
	cmpq	MTX_OWNER(%rdi), %rcx
	je	3f
#endif
	xorq	%rax, %rax
	ret

#ifdef DIAGNOSTIC
3:	movq	$4f, %rdi
	call	_C_LABEL(panic)
4:	.asciz	"mtx_enter_try: locking against myself"
#endif


ENTRY(mtx_leave)
	movq	%rdi, %rax
#ifdef DIAGNOSTIC
	movq	CPUVAR(SELF), %rcx
	cmpq	MTX_OWNER(%rax), %rcx
	jne	2f
	decl	CPU_INFO_MUTEX_LEVEL(%rcx)
#endif
	xorq	%rcx, %rcx
	movl	MTX_OLDIPL(%rax), %edi
	movl	%ecx, MTX_OLDIPL(%rax)
	movq	%rcx, MTX_OWNER(%rax)
	cmpl	%edi, CPUVAR(ILEVEL)
	je	1f
	call	_C_LABEL(spllower)
1:
	ret

#ifdef DIAGNOSTIC
2:	movq	$3f, %rdi
	call	_C_LABEL(panic)
3:	.asciz	"mtx_leave: lock not held"
#endif
