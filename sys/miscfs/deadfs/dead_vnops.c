/*	$OpenBSD: src/sys/miscfs/deadfs/dead_vnops.c,v 1.21 2008/05/08 17:45:45 thib Exp $	*/
/*	$NetBSD: dead_vnops.c,v 1.16 1996/02/13 13:12:48 mycroft Exp $	*/

/*
 * Copyright (c) 1989, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)dead_vnops.c	8.2 (Berkeley) 11/21/94
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/time.h>
#include <sys/vnode.h>
#include <sys/errno.h>
#include <sys/namei.h>
#include <sys/buf.h>
#include <sys/proc.h>
#include <sys/poll.h>

/*
 * Prototypes for dead operations on vnodes.
 */
int	dead_badop(void *);
int	dead_ebadf(void *);

int	dead_open(void *);
int	dead_read(void *);
int	dead_write(void *);
int	dead_ioctl(void *);
int	dead_poll(void *);
int	dead_lock(void *);
int	dead_bmap(void *);
int	dead_strategy(void *);
int	dead_print(void *);

int	chkvnlock(struct vnode *);

int (**dead_vnodeop_p)(void *);

struct vnodeopv_entry_desc dead_vnodeop_entries[] = {
	{ &vop_default_desc, eopnotsupp },
	{ &vop_lookup_desc, vop_generic_lookup },	/* lookup */
	{ &vop_create_desc, dead_badop },	/* create */
	{ &vop_mknod_desc, dead_badop },	/* mknod */
	{ &vop_open_desc, dead_open },		/* open */
	{ &vop_close_desc, nullop },		/* close */
	{ &vop_access_desc, dead_ebadf },	/* access */
	{ &vop_getattr_desc, dead_ebadf },	/* getattr */
	{ &vop_setattr_desc, dead_ebadf },	/* setattr */
	{ &vop_read_desc, dead_read },		/* read */
	{ &vop_write_desc, dead_write },	/* write */
	{ &vop_ioctl_desc, dead_ioctl },	/* ioctl */
	{ &vop_poll_desc, dead_poll },		/* poll */
	{ &vop_fsync_desc, nullop },		/* fsync */
	{ &vop_remove_desc, dead_badop },	/* remove */
	{ &vop_link_desc, dead_badop },		/* link */
	{ &vop_rename_desc, dead_badop },	/* rename */
	{ &vop_mkdir_desc, dead_badop },	/* mkdir */
	{ &vop_rmdir_desc, dead_badop },	/* rmdir */
	{ &vop_symlink_desc, dead_badop },	/* symlink */
	{ &vop_readdir_desc, dead_ebadf },	/* readdir */
	{ &vop_readlink_desc, dead_ebadf },	/* readlink */
	{ &vop_abortop_desc, dead_badop },	/* abortop */
	{ &vop_inactive_desc, nullop },		/* inactive */
	{ &vop_reclaim_desc, nullop },		/* reclaim */
	{ &vop_lock_desc, dead_lock },		/* lock */
	{ &vop_unlock_desc, vop_generic_unlock },	/* unlock */
	{ &vop_bmap_desc, dead_bmap },		/* bmap */
	{ &vop_strategy_desc, dead_strategy },	/* strategy */
	{ &vop_print_desc, dead_print },	/* print */
	{ &vop_islocked_desc, vop_generic_islocked },	/* islocked */
	{ &vop_pathconf_desc, dead_ebadf },	/* pathconf */
	{ &vop_advlock_desc, dead_ebadf },	/* advlock */
	{ &vop_bwrite_desc, nullop },		/* bwrite */
	{ (struct vnodeop_desc*)NULL, (int(*)(void *))NULL }
};
struct vnodeopv_desc dead_vnodeop_opv_desc =
	{ &dead_vnodeop_p, dead_vnodeop_entries };

/*
 * Open always fails as if device did not exist.
 */
/* ARGSUSED */
int
dead_open(void *v)
{
	return (ENXIO);
}

/*
 * Vnode op for read
 */
/* ARGSUSED */
int
dead_read(void *v)
{
	struct vop_read_args *ap = v;

	if (chkvnlock(ap->a_vp))
		panic("dead_read: lock");
	/*
	 * Return EOF for tty devices, EIO for others
	 */
	if ((ap->a_vp->v_flag & VISTTY) == 0)
		return (EIO);
	return (0);
}

/*
 * Vnode op for write
 */
/* ARGSUSED */
int
dead_write(void *v)
{
	struct vop_write_args *ap = v;

	if (chkvnlock(ap->a_vp))
		panic("dead_write: lock");
	return (EIO);
}

/*
 * Device ioctl operation.
 */
/* ARGSUSED */
int
dead_ioctl(void *v)
{
	struct vop_ioctl_args *ap = v;

	if (!chkvnlock(ap->a_vp))
		return (EBADF);
	return (VCALL(ap->a_vp, VOFFSET(vop_ioctl), ap));
}

/* ARGSUSED */
int
dead_poll(void *v)
{
#if 0
	struct vop_poll_args *ap = v;
#endif

	/*
	 * Let the user find out that the descriptor is gone.
	 */
	return (POLLHUP);
}

/*
 * Just call the device strategy routine
 */
int
dead_strategy(void *v)
{
	struct vop_strategy_args *ap = v;
	int s;

	if (ap->a_bp->b_vp == NULL || !chkvnlock(ap->a_bp->b_vp)) {
		ap->a_bp->b_flags |= B_ERROR;
		s = splbio();
		biodone(ap->a_bp);
		splx(s);
		return (EIO);
	}
	return (VOP_STRATEGY(ap->a_bp));
}

/*
 * Wait until the vnode has finished changing state.
 */
int
dead_lock(void *v)
{
	struct vop_lock_args *ap = v;
	struct vnode *vp = ap->a_vp;

	if (ap->a_flags & LK_DRAIN || !chkvnlock(vp))
		return (0);

	return (VCALL(vp, VOFFSET(vop_lock), ap));
}

/*
 * Wait until the vnode has finished changing state.
 */
int
dead_bmap(void *v)
{
	struct vop_bmap_args *ap = v;

	if (!chkvnlock(ap->a_vp))
		return (EIO);
	return (VOP_BMAP(ap->a_vp, ap->a_bn, ap->a_vpp, ap->a_bnp, ap->a_runp));
}

/*
 * Print out the contents of a dead vnode.
 */
/* ARGSUSED */
int
dead_print(void *v)
{
	printf("tag VT_NON, dead vnode\n");
	return 0;
}

/*
 * Empty vnode failed operation
 */
/*ARGSUSED*/
int
dead_ebadf(void *v)
{
	return (EBADF);
}

/*
 * Empty vnode bad operation
 */
/*ARGSUSED*/
int
dead_badop(void *v)
{
	panic("dead_badop called");
	/* NOTREACHED */
}

/*
 * We have to wait during times when the vnode is
 * in a state of change.
 */
int
chkvnlock(struct vnode *vp)
{
	int locked = 0;

	while (vp->v_flag & VXLOCK) {
		vp->v_flag |= VXWANT;
		tsleep(vp, PINOD, "chkvnlock", 0);
		locked = 1;
	}
	return (locked);
}
