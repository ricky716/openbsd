/*	$OpenBSD: src/sys/arch/mips64/include/spinlock.h,v 1.4 2013/06/01 20:47:40 tedu Exp $	*/
 /* Public domain */

#ifndef _MIPS64_SPINLOCK_H_
#define _MIPS64_SPINLOCK_H_

#define _ATOMIC_LOCK_UNLOCKED	(0)
#define _ATOMIC_LOCK_LOCKED	(1)
typedef int _atomic_lock_t;

#ifndef _KERNEL
int _atomic_lock(volatile _atomic_lock_t *);
#endif

#endif /* !_MIPS64_SPINLOCK_H_ */
