/*	$OpenBSD: src/sys/arch/arm/xscale/pxa2x0_apm.c,v 1.10 2005/03/30 14:24:39 dlg Exp $	*/

/*-
 * Copyright (c) 2001 Alexander Guy.  All rights reserved.
 * Copyright (c) 1998-2001 Michael Shalayeff. All rights reserved.
 * Copyright (c) 1995 John T. Kohl.  All rights reserved.
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
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF MIND, USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/kthread.h>
#include <sys/lock.h>
#include <sys/mount.h>		/* for vfs_syncwait() */
#include <sys/proc.h>
#include <sys/device.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <sys/event.h>

#include <machine/conf.h>
#include <machine/cpu.h>
#include <machine/apmvar.h>

#include <arm/xscale/pxa2x0reg.h>
#include <arm/xscale/pxa2x0var.h>
#include <arm/xscale/pxa2x0_apm.h>
#include <arm/xscale/pxa2x0_gpio.h>

#if defined(APMDEBUG)
#define DPRINTF(x)	printf x
#else
#define	DPRINTF(x)	/**/
#endif

#define APM_LOCK(sc)    lockmgr(&(sc)->sc_lock, LK_EXCLUSIVE, NULL, curproc)
#define APM_UNLOCK(sc)  lockmgr(&(sc)->sc_lock, LK_RELEASE, NULL, curproc)

struct cfdriver apm_cd = {
	NULL, "apm", DV_DULL
};

#define	APMUNIT(dev)	(minor(dev)&0xf0)
#define	APMDEV(dev)	(minor(dev)&0x0f)
#define APMDEV_NORMAL	0
#define APMDEV_CTL	8

int	apm_userstandbys;
int	apm_suspends;
int	apm_battlow;

/* battery percentage at where we get verbose in our warnings.  This
   value can be changed using sysctl(8), value machdep.apmwarn.
   Setting it to zero kills all warnings */
int	cpu_apmwarn = 10;

void	apm_power_print(struct pxa2x0_apm_softc *, struct apm_power_info *);
void	apm_power_info(struct pxa2x0_apm_softc *, struct apm_power_info *);
void	apm_suspend(struct pxa2x0_apm_softc *);
void	apm_resume(struct pxa2x0_apm_softc *);
int	apm_get_event(struct pxa2x0_apm_softc *, u_long *);
int	apm_handle_event(struct pxa2x0_apm_softc *, u_long);
void	apm_periodic_check(struct pxa2x0_apm_softc *);
void	apm_thread_create(void *);
void	apm_thread(void *);

void	filt_apmrdetach(struct knote *kn);
int	filt_apmread(struct knote *kn, long hint);
int	apmkqfilter(dev_t dev, struct knote *kn);

struct filterops apmread_filtops =
	{ 1, NULL, filt_apmrdetach, filt_apmread};

/*
 * Flags to control kernel display
 *	SCFLAG_NOPRINT:		do not output APM power messages due to
 *				a power change event.
 *
 *	SCFLAG_PCTPRINT:	do not output APM power messages due to
 *				to a power change event unless the battery
 *				percentage changes.
 */

#define SCFLAG_NOPRINT	0x0008000
#define SCFLAG_PCTPRINT	0x0004000
#define SCFLAG_PRINT	(SCFLAG_NOPRINT|SCFLAG_PCTPRINT)

#define	SCFLAG_OREAD 	(1 << 0)
#define	SCFLAG_OWRITE	(1 << 1)
#define	SCFLAG_OPEN	(SCFLAG_OREAD|SCFLAG_OWRITE)

/* This structure must be keept in sync with pxa2x0_apm_asm.S. */
struct pxa2x0_memcfg {
	/* SDRAM refresh */
	u_int32_t mdrefr_high;		/* 0x00 */
	u_int32_t mdrefr_low;		/* 0x04 */
	u_int32_t mdrefr_low2;		/* 0x08 */
	/* Synchronous, static, or VLIO interfaces */
	u_int32_t msc_high[3];		/* 0x0c */
	u_int32_t msc_low[3];		/* 0x18 */
	/* XXX move up */
	u_int32_t mdrefr_91;		/* 0x24 */
};

/* XXX */
#define MDREFR_C3000	(MDREFR_K0DB2 | MDREFR_E1PIN | MDREFR_K1RUN | \
			 MDREFR_K1DB2 | MDREFR_K2DB2 | MDREFR_APD)
#define MSC0_HIGH \
		( 7 << MSC_RRR_SHIFT << 16) | \
		(15 << MSC_RDN_SHIFT << 16) | \
		(15 << MSC_RDF_SHIFT << 16) | \
		(MSC_RT_NONBURST     << 16) | \
		( 2 << MSC_RRR_SHIFT)       | \
		(13 << MSC_RDN_SHIFT)       | \
		(13 << MSC_RDF_SHIFT)       | \
		MSC_RBW	/* PXA271 */        | \
		MSC_RT_NONBURST
#define MSC1_HIGH \
		( 7 << MSC_RRR_SHIFT << 16) | \
		(15 << MSC_RDN_SHIFT << 16) | \
		(15 << MSC_RDF_SHIFT << 16) | \
		(MSC_RT_VLIO         << 16) | \
		( 3 << MSC_RRR_SHIFT)       | \
		( 4 << MSC_RDN_SHIFT)       | \
		(13 << MSC_RDF_SHIFT)       | \
		MSC_RT_VLIO
#define MSC2_HIGH \
		( 7 << MSC_RRR_SHIFT << 16) | \
		(15 << MSC_RDN_SHIFT << 16) | \
		(15 << MSC_RDF_SHIFT << 16) | \
		(MSC_RT_NONBURST     << 16) | \
		( 3 << MSC_RRR_SHIFT)       | \
		( 4 << MSC_RDN_SHIFT)       | \
		(13 << MSC_RDF_SHIFT)       | \
		MSC_RT_VLIO
#define MSC0_LOW \
		( 7 << MSC_RRR_SHIFT << 16) | \
		(15 << MSC_RDN_SHIFT << 16) | \
		(15 << MSC_RDF_SHIFT << 16) | \
		(MSC_RT_NONBURST     << 16) | \
		( 1 << MSC_RRR_SHIFT)       | \
		( 8 << MSC_RDN_SHIFT)       | \
		( 8 << MSC_RDF_SHIFT)       | \
		MSC_RBW	/* PXA271 */        | \
		MSC_RT_NONBURST
#define MSC1_LOW \
		( 7 << MSC_RRR_SHIFT << 16) | \
		(15 << MSC_RDN_SHIFT << 16) | \
		(15 << MSC_RDF_SHIFT << 16) | \
		(MSC_RT_VLIO         << 16) | \
		( 1 << MSC_RRR_SHIFT)       | \
		( 2 << MSC_RDN_SHIFT)       | \
		( 6 << MSC_RDF_SHIFT)       | \
		MSC_RT_VLIO
#define MSC2_LOW \
		( 7 << MSC_RRR_SHIFT << 16) | \
		(15 << MSC_RDN_SHIFT << 16) | \
		(15 << MSC_RDF_SHIFT << 16) | \
		(MSC_RT_NONBURST     << 16) | \
		( 1 << MSC_RRR_SHIFT)       | \
		( 2 << MSC_RDN_SHIFT)       | \
		( 6 << MSC_RDF_SHIFT)       | \
		MSC_RT_VLIO
struct pxa2x0_memcfg pxa2x0_memcfg = {
	(MDREFR_C3000 | 0x030),
	(MDREFR_C3000 | 0x00b),
	(MDREFR_C3000 | 0x017),
	{ MSC0_HIGH, MSC1_HIGH, MSC2_HIGH },
	{ MSC1_LOW, MSC1_LOW, MSC2_LOW },
	(MDREFR_C3000 | 0x013)
};

#define PI2C_RETRY_COUNT	10
/* XXX varies depending on voltage regulator IC. */
#define PI2C_VOLTAGE_LOW	0x13	/* 1.00V */
#define PI2C_VOLTAGE_HIGH	0x1a	/* 1.35V */

void	pxa2x0_apm_sleep(struct pxa2x0_apm_softc *);

void	pxa2x0_pi2c_open(bus_space_tag_t, bus_space_handle_t);
void	pxa2x0_pi2c_close(bus_space_tag_t, bus_space_handle_t);
int	pxa2x0_pi2c_read(bus_space_tag_t, bus_space_handle_t, u_char, u_char *);
int	pxa2x0_pi2c_write(bus_space_tag_t, bus_space_handle_t, u_char, u_char);
int	pxa2x0_pi2c_getvoltage(bus_space_tag_t, bus_space_handle_t, u_char *);
int	pxa2x0_pi2c_setvoltage(bus_space_tag_t, bus_space_handle_t, u_char);
#if 0
void	pxa2x0_pi2c_print(struct pxa2x0_apm_softc *);
#endif

/* XXX used in pxa2x0_apm_asm.S */
bus_space_handle_t pxa2x0_gpio_ioh;
bus_space_handle_t pxa2x0_clkman_ioh;
bus_space_handle_t pxa2x0_memctl_ioh;

/* pxa2x0_apm_asm.S */
void	pxa27x_run_mode(void);
void	pxa27x_fastbus_run_mode(int, u_int32_t);
void	pxa27x_frequency_change(int, int, struct pxa2x0_memcfg *);
void	pxa2x0_cpu_suspend(void);
void	pxa2x0_cpu_resume(void);
void	pxa27x_cpu_speed_high(void);
void	pxa27x_cpu_speed_low(void);
void	pxa27x_cpu_speed_91(void);
void	pxa27x_cpu_speed_208(void);

/* XXX */
void	scoop_check_mcr(void);
void	scoop_suspend(void);
void	scoop_resume(void);

void
apm_power_print(struct pxa2x0_apm_softc *sc, struct apm_power_info *powerp)
{

	if (powerp->battery_life != APM_BATT_LIFE_UNKNOWN)
		printf("%s: battery life expectancy %d%%\n",
		    sc->sc_dev.dv_xname, powerp->battery_life);

	printf("%s: AC ", sc->sc_dev.dv_xname);
	switch (powerp->ac_state) {
	case APM_AC_OFF:
		printf("off,");
		break;
	case APM_AC_ON:
		printf("on,");
		break;
	case APM_AC_BACKUP:
		printf("backup power,");
		break;
	default:
	case APM_AC_UNKNOWN:
		printf("unknown,");
		break;
	}

	printf(" battery is ");
	switch (powerp->battery_state) {
	case APM_BATT_HIGH:
		printf("high");
		break;
	case APM_BATT_LOW:
		printf("low");
		break;
	case APM_BATT_CRITICAL:
		printf("CRITICAL");
		break;
	case APM_BATT_CHARGING:
		printf("charging");
		break;
	case APM_BATT_UNKNOWN:
		printf("unknown");
		break;
	default:
		printf("undecoded (%x)", powerp->battery_state);
		break;
	}

	printf("\n");
}

void
apm_power_info(struct pxa2x0_apm_softc *sc,
    struct apm_power_info *power)
{

	power->ac_state = APM_AC_UNKNOWN;
	power->battery_state = APM_BATT_UNKNOWN;;
	power->battery_life = 0 /* APM_BATT_LIFE_UNKNOWN */;
	power->minutes_left = 0;

	if (sc->sc_power_info != NULL)
		sc->sc_power_info(sc, power);
}

void
apm_suspend(struct pxa2x0_apm_softc *sc)
{

	dopowerhooks(PWR_SUSPEND);

	if (cold)
		vfs_syncwait(0);

	pxa2x0_apm_sleep((struct pxa2x0_apm_softc *)sc);
}

void
apm_resume(struct pxa2x0_apm_softc *sc)
{
	dopowerhooks(PWR_RESUME);

	/*
	 * Clear the OTG Peripheral hold after running the pxaudc and pxaohci
	 * powerhooks to re-enable their operation. See 3.8.1.2
	 */
	/* XXX ifdef NPXAUDC > 0 */
	bus_space_write_4(sc->sc_iot, sc->sc_pm_ioh, POWMAN_PSSR, PSSR_OTGPH);
}

int
apm_get_event(struct pxa2x0_apm_softc *sc, u_long *event_type)
{
	struct	apm_power_info power;

	/* Periodic callbacks could be replaced with a machine-dependant
	   get_event function. */
	if (sc->sc_periodic_check != NULL)
		sc->sc_periodic_check(sc);

	apm_power_info(sc, &power);
	if (power.ac_state != sc->sc_ac_state ||
	    power.battery_life != sc->sc_batt_life ||
	    power.battery_state != sc->sc_batt_state) {
		sc->sc_ac_state = power.ac_state;
		sc->sc_batt_life = power.battery_life;
		sc->sc_batt_state = power.battery_state;
		*event_type = APM_POWER_CHANGE;
		return 0;
	}

	*event_type = APM_NOEVENT;
	return 1;
}

int
apm_handle_event(struct pxa2x0_apm_softc *sc, u_long event_type)
{
	struct	apm_power_info power;
	int	ret = 0;

	switch (event_type) {
	case APM_NOEVENT:
		ret = 1;
		break;
	case APM_POWER_CHANGE:
		apm_power_info(sc, &power);
		if (power.battery_life != APM_BATT_LIFE_UNKNOWN &&
		    power.battery_life < cpu_apmwarn &&
		    (sc->sc_flags & SCFLAG_PRINT) != SCFLAG_NOPRINT &&
		    ((sc->sc_flags & SCFLAG_PRINT) != SCFLAG_PCTPRINT ||
		    sc->sc_prev_batt_life != power.battery_life)) {
			sc->sc_prev_batt_life = power.battery_life;
			apm_power_print(sc, &power);
		}
		break;
	default:
		DPRINTF(("apm_handle_event: unsupported event, code %d\n",
		    event_type));
	}

	return (ret);
}

void
apm_periodic_check(struct pxa2x0_apm_softc *sc)
{
	u_long	event_type;

	while (1) {
		if (apm_get_event(sc, &event_type) != 0)
			break;
		if (apm_handle_event(sc, event_type) != 0)
			break;
	}

	if (apm_battlow || apm_suspends || apm_userstandbys) {
		apm_suspend(sc);
		apm_resume(sc);
	}
	apm_battlow = apm_suspends = apm_userstandbys = 0;
}

void
apm_thread_create(void *v)
{
	struct pxa2x0_apm_softc *sc = v;

	if (kthread_create(apm_thread, sc, &sc->sc_thread,
	    "%s", sc->sc_dev.dv_xname)) {
		/* apm_disconnect(sc); */
		printf("%s: failed to create kernel thread, disabled",
		    sc->sc_dev.dv_xname);
	}
}

void
apm_thread(void *v)
{
	struct pxa2x0_apm_softc *sc = v;

	for (;;) {
		APM_LOCK(sc);
		apm_periodic_check(sc);
		APM_UNLOCK(sc);
		tsleep(&lbolt, PWAIT, "apmev", 0);
	}
}

int
apmopen(dev_t dev, int flag, int mode, struct proc *p)
{
	struct pxa2x0_apm_softc *sc;
	int error = 0;

	/* apm0 only */
	if (!apm_cd.cd_ndevs || APMUNIT(dev) != 0 ||
	    !(sc = apm_cd.cd_devs[APMUNIT(dev)]))
		return ENXIO;

	DPRINTF(("apmopen: dev %d pid %d flag %x mode %x\n",
	    APMDEV(dev), p->p_pid, flag, mode));

	switch (APMDEV(dev)) {
	case APMDEV_CTL:
		if (!(flag & FWRITE)) {
			error = EINVAL;
			break;
		}
		if (sc->sc_flags & SCFLAG_OWRITE) {
			error = EBUSY;
			break;
		}
		sc->sc_flags |= SCFLAG_OWRITE;
		break;
	case APMDEV_NORMAL:
		if (!(flag & FREAD) || (flag & FWRITE)) {
			error = EINVAL;
			break;
		}
		sc->sc_flags |= SCFLAG_OREAD;
		break;
	default:
		error = ENXIO;
		break;
	}
	return error;
}

int
apmclose(dev_t dev, int flag, int mode, struct proc *p)
{
	struct pxa2x0_apm_softc *sc;

	/* apm0 only */
	if (!apm_cd.cd_ndevs || APMUNIT(dev) != 0 ||
	    !(sc = apm_cd.cd_devs[APMUNIT(dev)]))
		return ENXIO;

	DPRINTF(("apmclose: pid %d flag %x mode %x\n", p->p_pid, flag, mode));

	switch (APMDEV(dev)) {
	case APMDEV_CTL:
		sc->sc_flags &= ~SCFLAG_OWRITE;
		break;
	case APMDEV_NORMAL:
		sc->sc_flags &= ~SCFLAG_OREAD;
		break;
	}
	return 0;
}

int
apmioctl(dev_t dev, u_long cmd, caddr_t data, int flag, struct proc *p)
{
	struct pxa2x0_apm_softc *sc;
	struct apm_power_info *power;
	int error = 0;

	/* apm0 only */
	if (!apm_cd.cd_ndevs || APMUNIT(dev) != 0 ||
	    !(sc = apm_cd.cd_devs[APMUNIT(dev)]))
		return ENXIO;

	switch (cmd) {
		/* some ioctl names from linux */
	case APM_IOC_STANDBY:
		if ((flag & FWRITE) == 0)
			error = EBADF;
		else
			apm_userstandbys++;
		break;
	case APM_IOC_SUSPEND:
		if ((flag & FWRITE) == 0)
			error = EBADF;
		else
			apm_suspends++;	/* XXX */
		break;
	case APM_IOC_PRN_CTL:
		if ((flag & FWRITE) == 0)
			error = EBADF;
		else {
			int flag = *(int *)data;
			DPRINTF(( "APM_IOC_PRN_CTL: %d\n", flag ));
			switch (flag) {
			case APM_PRINT_ON:	/* enable printing */
				sc->sc_flags &= ~SCFLAG_PRINT;
				break;
			case APM_PRINT_OFF: /* disable printing */
				sc->sc_flags &= ~SCFLAG_PRINT;
				sc->sc_flags |= SCFLAG_NOPRINT;
				break;
			case APM_PRINT_PCT: /* disable some printing */
				sc->sc_flags &= ~SCFLAG_PRINT;
				sc->sc_flags |= SCFLAG_PCTPRINT;
				break;
			default:
				error = EINVAL;
				break;
			}
		}
		break;
	case APM_IOC_DEV_CTL:
		if ((flag & FWRITE) == 0)
			error = EBADF;
		break;
	case APM_IOC_GETPOWER:
	        power = (struct apm_power_info *)data;
		apm_power_info(sc, power);
		break;

	default:
		error = ENOTTY;
	}

	return (error);
}

void
filt_apmrdetach(struct knote *kn)
{
	struct pxa2x0_apm_softc *sc =
	    (struct pxa2x0_apm_softc *)kn->kn_hook;

	SLIST_REMOVE(&sc->sc_note, kn, knote, kn_selnext);
}

int
filt_apmread(struct knote *kn, long hint)
{
	/* XXX weird kqueue_scan() semantics */
	if (hint && !kn->kn_data)
		kn->kn_data = (int)hint;

	return (1);
}

int
apmkqfilter(dev_t dev, struct knote *kn)
{
	struct pxa2x0_apm_softc *sc;

	/* apm0 only */
	if (!apm_cd.cd_ndevs || APMUNIT(dev) != 0 ||
	    !(sc = apm_cd.cd_devs[APMUNIT(dev)]))
		return ENXIO;

	switch (kn->kn_filter) {
	case EVFILT_READ:
		kn->kn_fop = &apmread_filtops;
		break;
	default:
		return (1);
	}

	kn->kn_hook = (caddr_t)sc;
	SLIST_INSERT_HEAD(&sc->sc_note, kn, kn_selnext);

	return (0);
}

void
pxa2x0_apm_attach_sub(struct pxa2x0_apm_softc *sc)
{

	sc->sc_iot = &pxa2x0_bs_tag;

	if (bus_space_map(sc->sc_iot, PXA2X0_POWMAN_BASE,
	    PXA2X0_POWMAN_SIZE, 0, &sc->sc_pm_ioh)) {
		printf("pxa2x0_apm_attach_sub: failed to map POWMAN\n");
		return;
	}

	lockinit(&sc->sc_lock, PWAIT, "apmlk", 0, 0);

	kthread_create_deferred(apm_thread_create, sc);

	printf("\n");

	if (bus_space_map(sc->sc_iot, PXA2X0_CLKMAN_BASE, PXA2X0_CLKMAN_SIZE,
	    0, &pxa2x0_clkman_ioh)) {
		printf("%s: failed to map CLKMAN\n", sc->sc_dev.dv_xname);
		return;
	}

	if (bus_space_map(sc->sc_iot, PXA2X0_MEMCTL_BASE, PXA2X0_MEMCTL_SIZE,
	    0, &pxa2x0_memctl_ioh)) {
		printf("%s: failed to map MEMCTL\n", sc->sc_dev.dv_xname);
		return;
	}

	if (bus_space_map(sc->sc_iot, PXA2X0_GPIO_BASE, PXA2X0_GPIO_SIZE,
	    0, &pxa2x0_gpio_ioh)) {
		printf("pxa2x0_apm_sleep: can't map GPIO\n");
		return;
	}
}

void
pxa2x0_wakeup_config(u_int32_t wsrc, int enable)
{
	struct pxa2x0_apm_softc *sc;
	u_int32_t prer;
	u_int32_t pfer;
	u_int32_t pkwr;

	if (apm_cd.cd_ndevs < 1 || apm_cd.cd_devs[0] == NULL)
		return;
	sc = apm_cd.cd_devs[0];

	prer = pfer = pkwr = 0;

	if ((wsrc & PXA2X0_WAKEUP_POWERON) != 0) {
		prer |= (1<<0);
		pfer |= (1<<0);
		pkwr |= (1<<12);
	}

	if ((wsrc & PXA2X0_WAKEUP_GPIORST) != 0)
		pfer |= (1<<1);
	if ((wsrc & PXA2X0_WAKEUP_SD) != 0)
		prer |= (1<<9);
	if ((wsrc & PXA2X0_WAKEUP_RC) != 0)
		prer |= (1<<13);
	if ((wsrc & PXA2X0_WAKEUP_SYNC) != 0)
		pkwr |= (1<<1);
	if ((wsrc & PXA2X0_WAKEUP_KEYNS0) != 0)
		prer |= (1<<12);
	if ((wsrc & PXA2X0_WAKEUP_KEYNS1) != 0)
		pkwr |= (1<<2);
	if ((wsrc & PXA2X0_WAKEUP_KEYNS2) != 0)
		pkwr |= (1<<9);
	if ((wsrc & PXA2X0_WAKEUP_KEYNS3) != 0)
		pkwr |= (1<<3);
	if ((wsrc & PXA2X0_WAKEUP_KEYNS4) != 0)
		pkwr |= (1<<4);
	if ((wsrc & PXA2X0_WAKEUP_KEYNS5) != 0)
		pkwr |= (1<<6);
	if ((wsrc & PXA2X0_WAKEUP_KEYNS6) != 0)
		pkwr |= (1<<7);
	if ((wsrc & PXA2X0_WAKEUP_CF0) != 0)
		pkwr |= (1<<11);
	if ((wsrc & PXA2X0_WAKEUP_CF1) != 0)
		pkwr |= (1<<10);
	if ((wsrc & PXA2X0_WAKEUP_USBD) != 0)
		prer |= (1<<24);

	if ((wsrc & PXA2X0_WAKEUP_LOCKSW) != 0) {
		prer |= (1<<15);
		pfer |= (1<<15);
	}

	if ((wsrc & PXA2X0_WAKEUP_JACKIN) != 0) {
		prer |= (1<<23);
		pfer |= (1<<23);
	}

	if ((wsrc & PXA2X0_WAKEUP_CHRGFULL) != 0)
		pkwr |= (1<<18);
	if ((wsrc & PXA2X0_WAKEUP_RTC) != 0)
		prer |= (1<<31);

	if (enable) {
		prer |= bus_space_read_4(sc->sc_iot, sc->sc_pm_ioh,
		    POWMAN_PRER);
		pfer |= bus_space_read_4(sc->sc_iot, sc->sc_pm_ioh,
		    POWMAN_PFER);
		pkwr |= bus_space_read_4(sc->sc_iot, sc->sc_pm_ioh,
		    POWMAN_PKWR);
	} else {
		prer = bus_space_read_4(sc->sc_iot, sc->sc_pm_ioh,
		    POWMAN_PRER) & ~prer;
		pfer = bus_space_read_4(sc->sc_iot, sc->sc_pm_ioh,
		    POWMAN_PFER) & ~pfer;
		pkwr = bus_space_read_4(sc->sc_iot, sc->sc_pm_ioh,
		    POWMAN_PKWR) & ~pkwr;
	}

	bus_space_write_4(sc->sc_iot, sc->sc_pm_ioh, POWMAN_PRER, prer);
	bus_space_write_4(sc->sc_iot, sc->sc_pm_ioh, POWMAN_PFER, pfer);
	bus_space_write_4(sc->sc_iot, sc->sc_pm_ioh, POWMAN_PWER,
	    prer | pfer);
	bus_space_write_4(sc->sc_iot, sc->sc_pm_ioh, POWMAN_PEDR,
	    0xffffffff);
	bus_space_write_4(sc->sc_iot, sc->sc_pm_ioh, POWMAN_PKWR, pkwr);
	bus_space_write_4(sc->sc_iot, sc->sc_pm_ioh, POWMAN_PKSR,
	    0xffffffff);

	/* XXX do that just before suspend. */
	pxa2x0_clkman_config(CKEN_KEY,
	    (wsrc & PXA2X0_WAKEUP_KEYNS_ALL) != 0);
}

struct pxa2x0_sleep_data {
	/* OS timer registers */
	u_int32_t sd_osmr0, sd_osmr1, sd_osmr2, sd_osmr3;
	u_int32_t sd_oscr0;
	u_int32_t sd_oier;
	/* GPIO registers */
	u_int32_t sd_gpdr0, sd_gpdr1, sd_gpdr2, sd_gpdr3;
	u_int32_t sd_grer0, sd_grer1, sd_grer2, sd_grer3;
	u_int32_t sd_gfer0, sd_gfer1, sd_gfer2, sd_gfer3;
	u_int32_t sd_gafr0_l, sd_gafr1_l, sd_gafr2_l, sd_gafr3_l;
	u_int32_t sd_gafr0_u, sd_gafr1_u, sd_gafr2_u, sd_gafr3_u;
	u_int32_t sd_gplr0, sd_gplr1, sd_gplr2, sd_gplr3;
	/* Interrupt controller registers */
	u_int32_t sd_iclr;
	u_int32_t sd_icmr;
	u_int32_t sd_iccr;
	/* Memory controller registers */
	u_int32_t sd_mecr;
	u_int32_t sd_mcmem0, sd_mcmem1;
	u_int32_t sd_mcatt0, sd_mcatt1;
	u_int32_t sd_mcio0, sd_mcio1;
	/* Clocks manager registers */
	u_int32_t sd_cken;
};

void
pxa2x0_apm_sleep(struct pxa2x0_apm_softc *sc)
{
	struct pxa2x0_sleep_data sd;
	bus_space_handle_t ost_ioh;
	bus_space_handle_t intctl_ioh;
	int save;
	u_int32_t rv;

	ost_ioh = (bus_space_handle_t)0;
	intctl_ioh = (bus_space_handle_t)0;

	if (bus_space_map(sc->sc_iot, PXA2X0_OST_BASE, PXA2X0_OST_SIZE, 0,
	    &ost_ioh)) {
		printf("pxa2x0_apm_sleep: can't map OST\n");
		goto out;
	}

	if (bus_space_map(sc->sc_iot, PXA2X0_INTCTL_BASE,
	    PXA2X0_INTCTL_SIZE, 0, &intctl_ioh)) {
		printf("pxa2x0_apm_sleep: can't map INTCTL\n");
		goto out;
	}

	save = disable_interrupts(I32_bit|F32_bit);

	sd.sd_oscr0 = bus_space_read_4(sc->sc_iot, ost_ioh, OST_OSCR0);
	sd.sd_osmr0 = bus_space_read_4(sc->sc_iot, ost_ioh, OST_OSMR0);
	sd.sd_osmr1 = bus_space_read_4(sc->sc_iot, ost_ioh, OST_OSMR1);
	sd.sd_osmr2 = bus_space_read_4(sc->sc_iot, ost_ioh, OST_OSMR2);
	sd.sd_osmr3 = bus_space_read_4(sc->sc_iot, ost_ioh, OST_OSMR3);
	sd.sd_oier = bus_space_read_4(sc->sc_iot, ost_ioh, OST_OIER);

	/* Bring the PXA27x into 416Mhz turbo mode. */
        if ((cputype & ~CPU_ID_XSCALE_COREREV_MASK) == CPU_ID_PXA27X &&
	    bus_space_read_4(sc->sc_iot, pxa2x0_clkman_ioh, CLKMAN_CCCR) !=
	    (CCCR_A | CCCR_TURBO_X2 | CCCR_RUN_X16)) {
#if 0
		pxa27x_cpu_speed_high();
#else
#define CLKCFG_T		(1<<0)	/* turbo */
#define CLKCFG_F		(1<<1)	/* frequency change */
#define CLKCFG_B		(1<<3)	/* fast-bus */
		pxa27x_frequency_change(CCCR_A | CCCR_TURBO_X2 |
		    CCCR_RUN_X16, CLKCFG_B | CLKCFG_F | CLKCFG_T,
		    &pxa2x0_memcfg);
#endif
		delay(500000); /* XXX */
	}

	scoop_check_mcr();

	/* XXX control battery charging in sleep mode. */

	resettodr();

	/* XXX schedule RTC alarm to check the battery, or schedule
	   XXX wake-up shortly before an already programmed alarm? */

	pxa2x0_wakeup_config(PXA2X0_WAKEUP_ALL, 1);

	pxa27x_run_mode();
#define MDREFR_LOW		(MDREFR_C3000 | 0x00b)
	pxa27x_fastbus_run_mode(0, MDREFR_LOW);
	delay(1);
#if 1
	pxa27x_cpu_speed_91();
#else
	pxa27x_frequency_change(CCCR_TURBO_X1 | CCCR_RUN_X7, CLKCFG_F,
	    &pxa2x0_memcfg);
#endif
	pxa2x0_pi2c_setvoltage(sc->sc_iot, sc->sc_pm_ioh, PI2C_VOLTAGE_LOW);

	scoop_check_mcr();
	scoop_suspend();

	sd.sd_gpdr0 = bus_space_read_4(sc->sc_iot, pxa2x0_gpio_ioh, GPIO_GPDR0);
	sd.sd_gpdr1 = bus_space_read_4(sc->sc_iot, pxa2x0_gpio_ioh, GPIO_GPDR1);
	sd.sd_gpdr2 = bus_space_read_4(sc->sc_iot, pxa2x0_gpio_ioh, GPIO_GPDR2);
	sd.sd_gpdr3 = bus_space_read_4(sc->sc_iot, pxa2x0_gpio_ioh, GPIO_GPDR3);

	sd.sd_grer0 = bus_space_read_4(sc->sc_iot, pxa2x0_gpio_ioh, GPIO_GRER0);
	sd.sd_grer1 = bus_space_read_4(sc->sc_iot, pxa2x0_gpio_ioh, GPIO_GRER1);
	sd.sd_grer2 = bus_space_read_4(sc->sc_iot, pxa2x0_gpio_ioh, GPIO_GRER2);
	sd.sd_grer3 = bus_space_read_4(sc->sc_iot, pxa2x0_gpio_ioh, GPIO_GRER3);

	sd.sd_gfer0 = bus_space_read_4(sc->sc_iot, pxa2x0_gpio_ioh, GPIO_GFER0);
	sd.sd_gfer1 = bus_space_read_4(sc->sc_iot, pxa2x0_gpio_ioh, GPIO_GFER1);
	sd.sd_gfer2 = bus_space_read_4(sc->sc_iot, pxa2x0_gpio_ioh, GPIO_GFER2);
	sd.sd_gfer3 = bus_space_read_4(sc->sc_iot, pxa2x0_gpio_ioh, GPIO_GFER3);

	sd.sd_gafr0_l = bus_space_read_4(sc->sc_iot, pxa2x0_gpio_ioh, GPIO_GAFR0_L);
	sd.sd_gafr1_l = bus_space_read_4(sc->sc_iot, pxa2x0_gpio_ioh, GPIO_GAFR1_L);
	sd.sd_gafr2_l = bus_space_read_4(sc->sc_iot, pxa2x0_gpio_ioh, GPIO_GAFR2_L);
	sd.sd_gafr3_l = bus_space_read_4(sc->sc_iot, pxa2x0_gpio_ioh, GPIO_GAFR3_L);

	sd.sd_gafr0_u = bus_space_read_4(sc->sc_iot, pxa2x0_gpio_ioh, GPIO_GAFR0_U);
	sd.sd_gafr1_u = bus_space_read_4(sc->sc_iot, pxa2x0_gpio_ioh, GPIO_GAFR1_U);
	sd.sd_gafr2_u = bus_space_read_4(sc->sc_iot, pxa2x0_gpio_ioh, GPIO_GAFR2_U);
	sd.sd_gafr3_u = bus_space_read_4(sc->sc_iot, pxa2x0_gpio_ioh, GPIO_GAFR3_U);

	sd.sd_gplr0 = bus_space_read_4(sc->sc_iot, pxa2x0_gpio_ioh, GPIO_GPLR0);
	sd.sd_gplr1 = bus_space_read_4(sc->sc_iot, pxa2x0_gpio_ioh, GPIO_GPLR1);
	sd.sd_gplr2 = bus_space_read_4(sc->sc_iot, pxa2x0_gpio_ioh, GPIO_GPLR2);
	sd.sd_gplr3 = bus_space_read_4(sc->sc_iot, pxa2x0_gpio_ioh, GPIO_GPLR3);

	sd.sd_iclr = bus_space_read_4(sc->sc_iot, intctl_ioh, INTCTL_ICLR);
	sd.sd_icmr = bus_space_read_4(sc->sc_iot, intctl_ioh, INTCTL_ICMR);
	sd.sd_iccr = bus_space_read_4(sc->sc_iot, intctl_ioh, INTCTL_ICCR);
	bus_space_write_4(sc->sc_iot, intctl_ioh, INTCTL_ICMR, 0);

	sd.sd_mecr = bus_space_read_4(sc->sc_iot, pxa2x0_memctl_ioh,
	    MEMCTL_MECR);
	sd.sd_mcmem0 = bus_space_read_4(sc->sc_iot, pxa2x0_memctl_ioh,
	    MEMCTL_MCMEM(0));
	sd.sd_mcmem1 = bus_space_read_4(sc->sc_iot, pxa2x0_memctl_ioh,
	    MEMCTL_MCMEM(1));
	sd.sd_mcatt0 = bus_space_read_4(sc->sc_iot, pxa2x0_memctl_ioh,
	    MEMCTL_MCATT(0));
	sd.sd_mcatt1 = bus_space_read_4(sc->sc_iot, pxa2x0_memctl_ioh,
	    MEMCTL_MCATT(1));
	sd.sd_mcio0 = bus_space_read_4(sc->sc_iot, pxa2x0_memctl_ioh,
	    MEMCTL_MCIO(0));
	sd.sd_mcio1 = bus_space_read_4(sc->sc_iot, pxa2x0_memctl_ioh,
	    MEMCTL_MCIO(1));

	sd.sd_cken = bus_space_read_4(sc->sc_iot, pxa2x0_clkman_ioh,
	    CLKMAN_CKEN);
	bus_space_write_4(sc->sc_iot, pxa2x0_clkman_ioh, CLKMAN_CKEN,
	    CKEN_MEM | CKEN_KEY);

	/* Disable nRESET_OUT. */
	rv = bus_space_read_4(sc->sc_iot, sc->sc_pm_ioh, POWMAN_PSLR);
#define  PSLR_SL_ROD	(1<<20)
	bus_space_write_4(sc->sc_iot, sc->sc_pm_ioh, POWMAN_PSLR,
	    rv | PSLR_SL_ROD);

	/* Clear reset controller status. */
	bus_space_write_4(sc->sc_iot, sc->sc_pm_ioh, POWMAN_RCSR,
	    RCSR_HWR | RCSR_WDR | RCSR_SMR | RCSR_GPR);
	
	/* Stop 3/13Mhz oscillator, do not float PCMCIA and chip-selects. */
	rv = PCFR_OPDE;
        if ((cputype & ~CPU_ID_XSCALE_COREREV_MASK) == CPU_ID_PXA27X)
		/* Enable nRESET_GPIO as a GPIO reset input. */
		rv |= PCFR_GPR_EN;
	bus_space_write_4(sc->sc_iot, sc->sc_pm_ioh, POWMAN_PCFR, rv);

	/* XXX C3000 */
#define	GPIO_G0_STROBE_BIT		0x0f800000
#define	GPIO_G1_STROBE_BIT		0x00100000
#define	GPIO_G2_STROBE_BIT		0x01000000
#define	GPIO_G3_STROBE_BIT		0x00041880
#define	GPIO_KEY_STROBE0		88
	bus_space_write_4(sc->sc_iot, sc->sc_pm_ioh, POWMAN_PGSR0,
	    0x00144018);
	bus_space_write_4(sc->sc_iot, sc->sc_pm_ioh, POWMAN_PGSR1,
	    0x00ef0000);
	bus_space_write_4(sc->sc_iot, sc->sc_pm_ioh, POWMAN_PGSR2,
	    0x0121c000);
	bus_space_write_4(sc->sc_iot, sc->sc_pm_ioh, POWMAN_PGSR3,
	    0x00600000);
	bus_space_write_4(sc->sc_iot, sc->sc_pm_ioh, POWMAN_PGSR0,
	    0x00144018 & ~GPIO_G0_STROBE_BIT);
	bus_space_write_4(sc->sc_iot, sc->sc_pm_ioh, POWMAN_PGSR1,
	    0x00ef0000 & ~GPIO_G1_STROBE_BIT);
	bus_space_write_4(sc->sc_iot, sc->sc_pm_ioh, POWMAN_PGSR2,
	    0x0121c000 & ~GPIO_G2_STROBE_BIT);
	bus_space_write_4(sc->sc_iot, sc->sc_pm_ioh, POWMAN_PGSR3,
	    0x00600000 & ~GPIO_G3_STROBE_BIT);
	bus_space_write_4(sc->sc_iot, sc->sc_pm_ioh, POWMAN_PGSR2,
	    (0x0121c000 & ~GPIO_G2_STROBE_BIT) |
	    GPIO_BIT(GPIO_KEY_STROBE0));

	/* C3000 */
#define GPIO_EXT_BUS_READY	18
	pxa2x0_gpio_set_function(GPIO_EXT_BUS_READY, GPIO_SET | GPIO_OUT);
	bus_space_write_4(sc->sc_iot, pxa2x0_gpio_ioh, GPIO_GPDR0, 0xd01c4418);
	bus_space_write_4(sc->sc_iot, pxa2x0_gpio_ioh, GPIO_GPDR1, 0xfcefbd21);
	bus_space_write_4(sc->sc_iot, pxa2x0_gpio_ioh, GPIO_GPDR2, 0x13a5ffff);
	bus_space_write_4(sc->sc_iot, pxa2x0_gpio_ioh, GPIO_GPDR3, 0x01e3e10c);

	bus_space_write_4(sc->sc_iot, sc->sc_pm_ioh, POWMAN_PSPR,
	    (u_int32_t)&pxa2x0_cpu_resume - 0xc0200000 + 0xa0200000);

	pxa2x0_cpu_suspend();

	bus_space_write_4(sc->sc_iot, sc->sc_pm_ioh, POWMAN_PSPR, 0);

	pxa2x0_clkman_config(CKEN_SSP|CKEN_PWM0|CKEN_PWM1, 1);
	pxa2x0_clkman_config(CKEN_KEY, 0);

	/* Clear all GPIO interrupt sources. */
	bus_space_write_4(sc->sc_iot, pxa2x0_gpio_ioh, GPIO_GEDR0, 0xffffffff);
	bus_space_write_4(sc->sc_iot, pxa2x0_gpio_ioh, GPIO_GEDR1, 0xffffffff);
	bus_space_write_4(sc->sc_iot, pxa2x0_gpio_ioh, GPIO_GEDR2, 0xffffffff);
	bus_space_write_4(sc->sc_iot, pxa2x0_gpio_ioh, GPIO_GPDR0, sd.sd_gpdr0);
	bus_space_write_4(sc->sc_iot, pxa2x0_gpio_ioh, GPIO_GPDR1, sd.sd_gpdr1);
	bus_space_write_4(sc->sc_iot, pxa2x0_gpio_ioh, GPIO_GPDR2, sd.sd_gpdr2);
	bus_space_write_4(sc->sc_iot, pxa2x0_gpio_ioh, GPIO_GRER0, sd.sd_grer0);
	bus_space_write_4(sc->sc_iot, pxa2x0_gpio_ioh, GPIO_GRER1, sd.sd_grer1);
	bus_space_write_4(sc->sc_iot, pxa2x0_gpio_ioh, GPIO_GRER2, sd.sd_grer2);
	bus_space_write_4(sc->sc_iot, pxa2x0_gpio_ioh, GPIO_GFER0, sd.sd_gfer0);
	bus_space_write_4(sc->sc_iot, pxa2x0_gpio_ioh, GPIO_GFER1, sd.sd_gfer1);
	bus_space_write_4(sc->sc_iot, pxa2x0_gpio_ioh, GPIO_GFER2, sd.sd_gfer2);
	bus_space_write_4(sc->sc_iot, pxa2x0_gpio_ioh, GPIO_GAFR0_L, sd.sd_gafr0_l);
	bus_space_write_4(sc->sc_iot, pxa2x0_gpio_ioh, GPIO_GAFR1_L, sd.sd_gafr1_l);
	bus_space_write_4(sc->sc_iot, pxa2x0_gpio_ioh, GPIO_GAFR2_L, sd.sd_gafr2_l);
	bus_space_write_4(sc->sc_iot, pxa2x0_gpio_ioh, GPIO_GAFR0_U, sd.sd_gafr0_u);
	bus_space_write_4(sc->sc_iot, pxa2x0_gpio_ioh, GPIO_GAFR1_U, sd.sd_gafr1_u);
	bus_space_write_4(sc->sc_iot, pxa2x0_gpio_ioh, GPIO_GAFR2_U, sd.sd_gafr2_u);
	bus_space_write_4(sc->sc_iot, pxa2x0_gpio_ioh, GPIO_GPSR0, sd.sd_gplr0 &
	    sd.sd_gpdr0);
	bus_space_write_4(sc->sc_iot, pxa2x0_gpio_ioh, GPIO_GPSR1, sd.sd_gplr1 &
	    sd.sd_gpdr1);
	bus_space_write_4(sc->sc_iot, pxa2x0_gpio_ioh, GPIO_GPSR2, sd.sd_gplr2 &
	    sd.sd_gpdr2);
	bus_space_write_4(sc->sc_iot, pxa2x0_gpio_ioh, GPIO_GPCR0, ~sd.sd_gplr0 &
	    sd.sd_gpdr0);
	bus_space_write_4(sc->sc_iot, pxa2x0_gpio_ioh, GPIO_GPCR1, ~sd.sd_gplr1 &
	    sd.sd_gpdr1);
	bus_space_write_4(sc->sc_iot, pxa2x0_gpio_ioh, GPIO_GPCR2, ~sd.sd_gplr2 &
	    sd.sd_gpdr2);

	/* PXA27x */
#if 0
	bus_space_write_4(sc->sc_iot, pxa2x0_gpio_ioh, GPIO_GEDR3, 0xffffffff);
#endif
	bus_space_write_4(sc->sc_iot, pxa2x0_gpio_ioh, GPIO_GPDR3, sd.sd_gpdr3);
	bus_space_write_4(sc->sc_iot, pxa2x0_gpio_ioh, GPIO_GRER3, sd.sd_grer3);
	bus_space_write_4(sc->sc_iot, pxa2x0_gpio_ioh, GPIO_GFER3, sd.sd_gfer3);
	bus_space_write_4(sc->sc_iot, pxa2x0_gpio_ioh, GPIO_GAFR3_L, sd.sd_gafr3_l);
	bus_space_write_4(sc->sc_iot, pxa2x0_gpio_ioh, GPIO_GAFR3_U, sd.sd_gafr3_u);
	bus_space_write_4(sc->sc_iot, pxa2x0_gpio_ioh, GPIO_GPSR3, sd.sd_gplr3 &
	    sd.sd_gpdr3);
	bus_space_write_4(sc->sc_iot, pxa2x0_gpio_ioh, GPIO_GPCR3, ~sd.sd_gplr3 &
	    sd.sd_gpdr3);

	bus_space_write_4(sc->sc_iot, pxa2x0_memctl_ioh, MEMCTL_MECR,
	    sd.sd_mecr);
	bus_space_write_4(sc->sc_iot, pxa2x0_memctl_ioh, MEMCTL_MCMEM(0),
	    sd.sd_mcmem0);
	bus_space_write_4(sc->sc_iot, pxa2x0_memctl_ioh, MEMCTL_MCMEM(1),
	    sd.sd_mcmem1);
	bus_space_write_4(sc->sc_iot, pxa2x0_memctl_ioh, MEMCTL_MCATT(0),
	    sd.sd_mcatt0);
	bus_space_write_4(sc->sc_iot, pxa2x0_memctl_ioh, MEMCTL_MCATT(1),
	    sd.sd_mcatt1);
	bus_space_write_4(sc->sc_iot, pxa2x0_memctl_ioh, MEMCTL_MCIO(0),
	    sd.sd_mcio0);
	bus_space_write_4(sc->sc_iot, pxa2x0_memctl_ioh, MEMCTL_MCIO(1),
	    sd.sd_mcio1);

	bus_space_write_4(sc->sc_iot, pxa2x0_clkman_ioh, CLKMAN_CKEN,
	    sd.sd_cken);

	bus_space_write_4(sc->sc_iot, intctl_ioh, INTCTL_ICLR, sd.sd_iclr);
	bus_space_write_4(sc->sc_iot, intctl_ioh, INTCTL_ICCR, sd.sd_iccr);
	bus_space_write_4(sc->sc_iot, intctl_ioh, INTCTL_ICMR, sd.sd_icmr);

	if ((bus_space_read_4(sc->sc_iot, intctl_ioh, INTCTL_ICIP) & 0x1) != 0)
		bus_space_write_4(sc->sc_iot, sc->sc_pm_ioh, POWMAN_PEDR, 0x1);

	scoop_check_mcr();
	scoop_resume();

	pxa2x0_pi2c_setvoltage(sc->sc_iot, sc->sc_pm_ioh, PI2C_VOLTAGE_HIGH);

	/* Change to 208Mhz run mode with fast-bus still disabled. */
	pxa27x_frequency_change(CCCR_A | CCCR_TURBO_X2 | CCCR_RUN_X16,
	    CLKCFG_F, &pxa2x0_memcfg);
	delay(1); /* XXX is the delay long enough, and necessary at all? */
	pxa27x_fastbus_run_mode(1, pxa2x0_memcfg.mdrefr_high);

	bus_space_write_4(sc->sc_iot, sc->sc_pm_ioh, POWMAN_PMCR, 0);

	/* Change to 416Mhz turbo mode with fast-bus enabled. */
	pxa27x_frequency_change(CCCR_A | CCCR_TURBO_X2 | CCCR_RUN_X16,
	    CLKCFG_B | CLKCFG_F | CLKCFG_T, &pxa2x0_memcfg);

	bus_space_write_4(sc->sc_iot, ost_ioh, OST_OSMR0, sd.sd_osmr0);
	bus_space_write_4(sc->sc_iot, ost_ioh, OST_OSMR1, sd.sd_osmr1);
	bus_space_write_4(sc->sc_iot, ost_ioh, OST_OSMR2, sd.sd_osmr2);
	bus_space_write_4(sc->sc_iot, ost_ioh, OST_OSMR3, sd.sd_osmr3);
	bus_space_write_4(sc->sc_iot, ost_ioh, OST_OSCR0, sd.sd_oscr0);
	bus_space_write_4(sc->sc_iot, ost_ioh, OST_OIER, sd.sd_oier);

	inittodr(0);

	restore_interrupts(save);

 out:
	if (ost_ioh != (bus_space_handle_t)0)
		bus_space_unmap(sc->sc_iot, ost_ioh, PXA2X0_OST_SIZE);
	if (intctl_ioh != (bus_space_handle_t)0)
		bus_space_unmap(sc->sc_iot, intctl_ioh, PXA2X0_INTCTL_SIZE);
}

void
pxa2x0_pi2c_open(bus_space_tag_t iot, bus_space_handle_t ioh)
{
	u_int32_t rv;

	/* Enable the I2C unit, and disable automatic voltage change. */
	rv = bus_space_read_4(iot, ioh, POWMAN_PCFR);
	bus_space_write_4(iot, ioh, POWMAN_PCFR, rv | PCFR_PI2C_EN);
	rv = bus_space_read_4(iot, ioh, POWMAN_PCFR);
	bus_space_write_4(iot, ioh, POWMAN_PCFR, rv & ~PCFR_FVC);
	delay(1);

	/* Enable the clock to the power manager I2C unit. */
	pxa2x0_clkman_config(CKEN_PI2C, 1);
	delay(1);
}

void
pxa2x0_pi2c_close(bus_space_tag_t iot, bus_space_handle_t ioh)
{
	u_int32_t rv;

	bus_space_write_4(iot, ioh, POWMAN_PICR, PICR_UR);
	bus_space_write_4(iot, ioh, POWMAN_PISAR, 0);
	delay(1);

	/* Disable the clock to the power manager I2C unit. */
	pxa2x0_clkman_config(CKEN_PI2C, 0);
	delay(1);

	/* Disable the I2C unit, and disable automatic voltage change. */
	rv = bus_space_read_4(iot, ioh, POWMAN_PCFR);
	bus_space_write_4(iot, ioh, POWMAN_PCFR,
	    rv & ~(PCFR_PI2C_EN | PCFR_FVC));
	delay(1);
}

int
pxa2x0_pi2c_read(bus_space_tag_t iot, bus_space_handle_t ioh,
    u_char slave, u_char *valuep)
{
	u_int32_t rv;
	int timeout;
	int tries = PI2C_RETRY_COUNT;

retry:

	bus_space_write_4(iot, ioh, POWMAN_PICR, PICR_UR);
	bus_space_write_4(iot, ioh, POWMAN_PISAR, 0x00);
	delay(1);
	bus_space_write_4(iot, ioh, POWMAN_PICR, PICR_IUE | PICR_SCLE);

	/* Write slave device address. */
	bus_space_write_4(iot, ioh, POWMAN_PIDBR, (slave<<1) | 0x1);
	rv = bus_space_read_4(iot, ioh, POWMAN_PICR);
	bus_space_write_4(iot, ioh, POWMAN_PICR, rv | PICR_START);
	rv = bus_space_read_4(iot, ioh, POWMAN_PICR);
	bus_space_write_4(iot, ioh, POWMAN_PICR, rv & ~PICR_STOP);
	rv = bus_space_read_4(iot, ioh, POWMAN_PICR);
	bus_space_write_4(iot, ioh, POWMAN_PICR, rv | PICR_TB);

	timeout = 10000;
	while ((bus_space_read_4(iot, ioh, POWMAN_PISR) & PISR_ITE) == 0) {
		if (timeout-- == 0) {
			bus_space_write_4(iot, ioh, POWMAN_PISR, PISR_ITE);
			goto err;
		}
		delay(1);
	}

	bus_space_write_4(iot, ioh, POWMAN_PISR, PISR_ITE);

	rv = bus_space_read_4(iot, ioh, POWMAN_PICR);
	bus_space_write_4(iot, ioh, POWMAN_PICR, rv & ~PICR_START);

	/* Read data value. */
	rv = bus_space_read_4(iot, ioh, POWMAN_PICR);
	bus_space_write_4(iot, ioh, POWMAN_PICR, rv |
	    (PICR_STOP | PICR_ACKNAK));
	rv = bus_space_read_4(iot, ioh, POWMAN_PICR);
	bus_space_write_4(iot, ioh, POWMAN_PICR, rv | PICR_TB);

	timeout = 10000;
	while ((bus_space_read_4(iot, ioh, POWMAN_PISR) & PISR_IRF) == 0) {
		if (timeout-- == 0) {
			bus_space_write_4(iot, ioh, POWMAN_PISR, PISR_IRF);
			goto err;
		}
		delay(1);
	}

	bus_space_write_4(iot, ioh, POWMAN_PISR, PISR_IRF);
	rv = bus_space_read_4(iot, ioh, POWMAN_PIDBR);
	*valuep = (u_char)rv;
	rv = bus_space_read_4(iot, ioh, POWMAN_PICR);
	bus_space_write_4(iot, ioh, POWMAN_PICR, rv &
	    ~(PICR_STOP | PICR_ACKNAK));

	return (0);
err:
	if (tries-- >= 0)
		goto retry;

	bus_space_write_4(iot, ioh, POWMAN_PICR, PICR_UR);
	bus_space_write_4(iot, ioh, POWMAN_PISAR, 0x00);
	bus_space_write_4(iot, ioh, POWMAN_PICR, PICR_IUE | PICR_SCLE);

	return (-EIO);
}

int
pxa2x0_pi2c_write(bus_space_tag_t iot, bus_space_handle_t ioh,
    u_char slave, u_char value)
{
	u_int32_t rv;
	int timeout;
	int tries = PI2C_RETRY_COUNT;

retry:

	bus_space_write_4(iot, ioh, POWMAN_PICR, PICR_UR);
	bus_space_write_4(iot, ioh, POWMAN_PISAR, 0x00);
	delay(1);
	bus_space_write_4(iot, ioh, POWMAN_PICR, PICR_IUE | PICR_SCLE);

	/* Write slave device address. */
	bus_space_write_4(iot, ioh, POWMAN_PIDBR, (slave<<1));
	rv = bus_space_read_4(iot, ioh, POWMAN_PICR);
	bus_space_write_4(iot, ioh, POWMAN_PICR, rv | PICR_START);
	rv = bus_space_read_4(iot, ioh, POWMAN_PICR);
	bus_space_write_4(iot, ioh, POWMAN_PICR, rv & ~PICR_STOP);
	rv = bus_space_read_4(iot, ioh, POWMAN_PICR);
	bus_space_write_4(iot, ioh, POWMAN_PICR, rv | PICR_TB);

	timeout = 10000;
	while ((bus_space_read_4(iot, ioh, POWMAN_PISR) & PISR_ITE) == 0) {
		if (timeout-- == 0) {
			bus_space_write_4(iot, ioh, POWMAN_PISR, PISR_ITE);
			goto err;
		}
		delay(1);
	}
	if ((bus_space_read_4(iot, ioh, POWMAN_PISR) & PISR_ACKNAK) != 0)
		goto err;
	bus_space_write_4(iot, ioh, POWMAN_PISR, PISR_ITE);

	/* Write data. */
	rv = bus_space_read_4(iot, ioh, POWMAN_PICR);
	bus_space_write_4(iot, ioh, POWMAN_PICR, rv & ~PICR_START);
	rv = bus_space_read_4(iot, ioh, POWMAN_PICR);
	bus_space_write_4(iot, ioh, POWMAN_PICR, rv | PICR_STOP);
	bus_space_write_4(iot, ioh, POWMAN_PIDBR, value);
	rv = bus_space_read_4(iot, ioh, POWMAN_PICR);
	bus_space_write_4(iot, ioh, POWMAN_PICR, rv | PICR_TB);

	timeout = 10000;
	while ((bus_space_read_4(iot, ioh, POWMAN_PISR) & PISR_ITE) == 0) {
		if (timeout-- == 0) {
#if 0
			bus_space_write_4(iot, ioh, POWMAN_PISR, PISR_ITE);
#endif
			goto err;
		}
		delay(1);
	}
	if ((bus_space_read_4(iot, ioh, POWMAN_PISR) & PISR_ACKNAK) != 0)
		goto err;
	bus_space_write_4(iot, ioh, POWMAN_PISR, PISR_ITE);

	rv = bus_space_read_4(iot, ioh, POWMAN_PICR);
	bus_space_write_4(iot, ioh, POWMAN_PICR, rv & ~PICR_STOP);

	return (0);
err:
	bus_space_write_4(iot, ioh, POWMAN_PISR, PISR_ITE);
	if (tries-- >= 0)
		goto retry;

	bus_space_write_4(iot, ioh, POWMAN_PICR, PICR_UR);
	bus_space_write_4(iot, ioh, POWMAN_PISAR, 0x00);
	bus_space_write_4(iot, ioh, POWMAN_PICR, PICR_IUE | PICR_SCLE);

	return (-EIO);
}

int
pxa2x0_pi2c_getvoltage(bus_space_tag_t iot, bus_space_handle_t ioh,
    u_char *valuep)
{
	int res;

	pxa2x0_pi2c_open(iot, ioh);
	res = pxa2x0_pi2c_read(iot, ioh, 0x0c, valuep);
	pxa2x0_pi2c_close(iot, ioh);
	return (res);
}

int
pxa2x0_pi2c_setvoltage(bus_space_tag_t iot, bus_space_handle_t ioh,
    u_char value)
{
	int res;

	pxa2x0_pi2c_open(iot, ioh);
	res = pxa2x0_pi2c_write(iot, ioh, 0x0c, value);
	pxa2x0_pi2c_close(iot, ioh);
	return (res);
}

#if 0
void
pxa2x0_pi2c_print(struct pxa2x0_apm_softc *sc)
{
	u_char value = 0;

	(void)pxa2x0_pi2c_getvoltage(sc->sc_iot, sc->sc_pm_ioh, &value);
	printf("xscale core voltage: %s\n", value == PI2C_VOLTAGE_HIGH ?
	    "high" : (value == PI2C_VOLTAGE_LOW ? "low" : "unkown"));
}
#endif

