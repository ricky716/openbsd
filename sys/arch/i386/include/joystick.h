/*	$OpenBSD: src/sys/arch/i386/include/joystick.h,v 1.3 1996/05/30 09:30:10 deraadt Exp $ */
/*	$NetBSD: joystick.h,v 1.1 1996/03/27 19:18:56 perry Exp $	*/

#ifndef _JOY_IOCTL_H_
#define _JOY_IOCTL_H_

#include <sys/types.h>
#include <sys/ioctl.h>

struct joystick {
    int x;
    int y;
    int b1;
    int b2;
};

#define JOY_SETTIMEOUT    _IOW('J', 1, int)    /* set timeout */
#define JOY_GETTIMEOUT    _IOR('J', 2, int)    /* get timeout */
#define JOY_SET_X_OFFSET  _IOW('J', 3, int)    /* set offset on X-axis */
#define JOY_SET_Y_OFFSET  _IOW('J', 4, int)    /* set offset on X-axis */
#define JOY_GET_X_OFFSET  _IOR('J', 5, int)    /* get offset on X-axis */
#define JOY_GET_Y_OFFSET  _IOR('J', 6, int)    /* get offset on Y-axis */

#endif /* _JOY_IOCTL_H_ */
