/*	$OpenBSD: src/sys/arch/sgi/sgi/ip27_machdep.c,v 1.10 2009/05/28 18:03:55 miod Exp $	*/

/*
 * Copyright (c) 2008, 2009 Miodrag Vallat.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * Origin 200 / Origin 2000 / Onyx 2 (IP27) specific code.
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/device.h>
#include <sys/malloc.h>
#include <sys/reboot.h>
#include <sys/tty.h>

#include <mips64/arcbios.h>
#include <mips64/archtype.h>

#include <machine/autoconf.h>
#include <machine/bus.h>
#include <machine/cpu.h>
#include <machine/memconf.h>
#include <machine/mnode.h>

#include <uvm/uvm_extern.h>

#include <sgi/sgi/ip27.h>
#include <sgi/xbow/hub.h>
#include <sgi/xbow/xbow.h>
#include <sgi/xbow/xbridgereg.h>

#include <dev/ic/comvar.h>

extern void (*md_halt)(int);

paddr_t	ip27_widget_short(int16_t, u_int);
paddr_t	ip27_widget_long(int16_t, u_int);
int	ip27_widget_id(int16_t, u_int, uint32_t *);

void	ip27_halt(int);

static paddr_t io_base;
static int ip35 = 0;

int	ip27_hub_intr_register(int, int, int *);
int	ip27_hub_intr_establish(int (*)(void *), void *, int, int,
	    const char *);
void	ip27_hub_intr_disestablish(int);
intrmask_t ip27_hub_intr_handler(intrmask_t, struct trap_frame *);
void	ip27_hub_intr_makemasks(void);
void	ip27_hub_do_pending_int(int);

void	ip27_nmi(void *);

void
ip27_setup()
{
	kl_nmi_t *nmi;

	uncached_base = PHYS_TO_XKPHYS_UNCACHED(0, SP_NC);
	io_base = PHYS_TO_XKPHYS_UNCACHED(0, SP_IO);

	ip35 = sys_config.system_type == SGI_O300;

	xbow_widget_short = ip27_widget_short;
	xbow_widget_long = ip27_widget_long;
	xbow_widget_id = ip27_widget_id;

	md_halt = ip27_halt;

	kl_init();
	if (kl_n_mode != 0)
		xbow_long_shift = 28;

	/*
	 * Scan this node's configuration to find out CPU and memory
	 * information.
	 */

	kl_scan_config(0);
	kl_scan_done();

	/*
	 * Initialize the early console parameters.
	 * This assumes IOC3 is accessible through a widget small window.
	 */

	xbow_build_bus_space(&sys_config.console_io, 0, 8 /* whatever */, 0);
	/* Constrain to a short window */
	sys_config.console_io.bus_base =
	    kl_get_console_base() & 0xffffffffff000000UL;

	comconsaddr = kl_get_console_base() & 0x0000000000ffffffUL;
	comconsfreq = 22000000 / 3;
	comconsiot = &sys_config.console_io;

	/*
	 * Force widget interrupts to run through us, unless a
	 * better interrupt master widget is found.
	 */

	xbow_intr_widget_intr_register = ip27_hub_intr_register;
	xbow_intr_widget_intr_establish = ip27_hub_intr_establish;
	xbow_intr_widget_intr_disestablish = ip27_hub_intr_disestablish;

	set_intr(INTPRI_XBOWMUX, CR_INT_0, ip27_hub_intr_handler);
	register_pending_int_handler(ip27_hub_do_pending_int);

	/*
	 * Disable all hardware interrupts.
	 */

	IP27_LHUB_S(HUB_CPU0_IMR0, 0);
	IP27_LHUB_S(HUB_CPU0_IMR1, 0);
	IP27_LHUB_S(HUB_CPU1_IMR0, 0);
	IP27_LHUB_S(HUB_CPU1_IMR1, 0);
	(void)IP27_LHUB_L(HUB_IR0);
	(void)IP27_LHUB_L(HUB_IR1);
	/* XXX do the other two cpus on IP35 */

	/*
	 * Setup NMI handler.
	 */
	nmi = IP27_KLNMI_HDR(0);
	nmi->magic = NMI_MAGIC;
	nmi->cb = (vaddr_t)ip27_nmi;
	nmi->cb_complement = ~nmi->cb;
	nmi->cb_arg = 0;

	/*
	 * Set up Node 0's HUB.
	 */
	IP27_LHUB_S(PI_REGION_PRESENT, 1);
	IP27_LHUB_S(PI_CALIAS_SIZE, PI_CALIAS_SIZE_0);
}

/*
 * Widget mapping.
 */

paddr_t
ip27_widget_short(int16_t nasid, u_int widget)
{
	/*
	 * A hardware bug on the PI side of the Hub chip (at least in
	 * earlier versions) causes accesses to the short window #0
	 * to be unreliable.
	 * The PROM implements a workaround by remapping it to
	 * big window #6 (the last programmable big window).
	 */
	if (widget == 0)
		return ip27_widget_long(nasid, 6);

	return ((uint64_t)(widget) << 24) | ((uint64_t)(nasid) << 32) | io_base;
}

paddr_t
ip27_widget_long(int16_t nasid, u_int widget)
{
	return ((uint64_t)(widget + 1) << xbow_long_shift) |
	    ((uint64_t)(nasid) << 32) | io_base;
}

/*
 * Widget enumeration
 */

int
ip27_widget_id(int16_t nasid, u_int widget, uint32_t *wid)
{
	paddr_t wpa;
	uint32_t id;

	if (widget != 0)
	{
		if (widget < WIDGET_MIN || widget > WIDGET_MAX)
			return EINVAL;
	}

	wpa = ip27_widget_short(nasid, widget);
	if (guarded_read_4(wpa + WIDGET_ID, &id) != 0)
		return ENXIO;

	if (wid != NULL)
		*wid = id;

	return 0;
}

/*
 * Reboot code
 */

void
ip27_halt(int howto)
{
	/*
	 * Even if ARCBios TLB and exception vectors are restored,
	 * returning to ARCBios doesn't work.
	 *
	 * So, instead, send a reset through the network interface
	 * of the Hub space.  Unfortunately there is no known way
	 * to tell the PROM which action we want it to take afterwards.
	 */

	if (howto & RB_HALT) {
		if (howto & RB_POWERDOWN)
			return;	/* caller will spin */
	}

	if (ip35) {
		IP27_LHUB_S(HUB_NI_IP35 + HUB_NI_RESET_ENABLE, RESET_ENABLE);
		IP27_LHUB_S(HUB_NI_IP35 + HUB_NI_RESET,
		    RESET_LOCAL | RESET_ACTION);
	} else {
		IP27_LHUB_S(HUB_NI_IP27 + HUB_NI_RESET_ENABLE, RESET_ENABLE);
		IP27_LHUB_S(HUB_NI_IP27 + HUB_NI_RESET,
		    RESET_LOCAL | RESET_ACTION);
	}
}

/*
 * Local HUB interrupt handling routines
 */

uint64_t ip27_hub_intrmask;

/*
 * Find a suitable interrupt bit for the given interrupt.
 */
int
ip27_hub_intr_register(int widget, int level, int *intrbit)
{
	int bit;

	/*
	 * All interrupts will be serviced at hardware level 0,
	 * so the `level' argument can be ignored.
	 * On HUB, the low 7 bits of the level 0 interrupt register
	 * are reserved.
	 */
	for (bit = SPL_CLOCK - 1; bit >= 7; bit--)
		if ((ip27_hub_intrmask & (1 << bit)) == 0)
			break;

	if (bit < 7)
		return EINVAL;

	*intrbit = bit;
	return 0;
}

/*
 * Register an interrupt handler for a given source, and enable it.
 */
int
ip27_hub_intr_establish(int (*func)(void *), void *arg, int intrbit,
    int level, const char *name)
{
	struct intrhand *ih;

#ifdef DIAGNOSTIC
	if (intrbit < 0 || intrbit >= SPL_CLOCK)
		return EINVAL;
#endif

	/*
	 * Widget interrupts are not supposed to be shared - the interrupt
	 * mask is large enough for all widgets.
	 */
	if (intrhand[intrbit] != NULL)
		return EEXIST;

	ih = malloc(sizeof(*ih), M_DEVBUF, M_NOWAIT);
	if (ih == NULL)
		return ENOMEM;

	ih->ih_next = NULL;
	ih->ih_fun = func;
	ih->ih_arg = arg;
	ih->ih_level = level;
	ih->ih_irq = intrbit;
	if (name != NULL)
		evcount_attach(&ih->ih_count, name, &ih->ih_level,
		    &evcount_intr);
	intrhand[intrbit] = ih;

	ip27_hub_intrmask |= 1UL << intrbit;
	ip27_hub_intr_makemasks();

	/* XXX this assumes we run on cpu0 */
	IP27_LHUB_S(HUB_CPU0_IMR0,
	    IP27_LHUB_L(HUB_CPU0_IMR0) | (1UL << intrbit));
	(void)IP27_LHUB_L(HUB_IR0);

	return 0;
}

void
ip27_hub_intr_disestablish(int intrbit)
{
	struct intrhand *ih;
	int s;

#ifdef DIAGNOSTIC
	if (intrbit < 0 || intrbit >= SPL_CLOCK)
		return;
#endif

	s = splhigh();

	if ((ih = intrhand[intrbit]) == NULL) {
		splx(s);
		return;
	}

	/* XXX this assumes we run on cpu0 */
	IP27_LHUB_S(HUB_CPU0_IMR0,
	    IP27_LHUB_L(HUB_CPU0_IMR0) & ~(1UL << intrbit));
	(void)IP27_LHUB_L(HUB_IR0);

	intrhand[intrbit] = NULL;

	ip27_hub_intrmask &= ~(1UL << intrbit);
	ip27_hub_intr_makemasks();

	free(ih, M_DEVBUF);

	splx(s);
}

/*
 * Recompute interrupt masks.
 */
void
ip27_hub_intr_makemasks()
{
	int irq, level;
	struct intrhand *q;
	intrmask_t intrlevel[INTMASKSIZE];

	/* First, figure out which levels each IRQ uses. */
	for (irq = 0; irq < INTMASKSIZE; irq++) {
		int levels = 0;
		for (q = intrhand[irq]; q; q = q->ih_next)
			levels |= 1 << q->ih_level;
		intrlevel[irq] = levels;
	}

	/* Then figure out which IRQs use each level. */
	for (level = IPL_NONE; level < NIPLS; level++) {
		int irqs = 0;
		for (irq = 0; irq < INTMASKSIZE; irq++)
			if (intrlevel[irq] & (1 << level))
				irqs |= 1 << irq;
		if (level != IPL_NONE)
			irqs |= SINT_ALLMASK;
		imask[level] = irqs;
	}

	/*
	 * There are tty, network and disk drivers that use free() at interrupt
	 * time, so vm > (tty | net | bio).
	 *
	 * Enforce a hierarchy that gives slow devices a better chance at not
	 * dropping data.
	 */
	imask[IPL_NET] |= imask[IPL_BIO];
	imask[IPL_TTY] |= imask[IPL_NET];
	imask[IPL_VM] |= imask[IPL_TTY];
	imask[IPL_CLOCK] |= imask[IPL_VM] | SPL_CLOCKMASK;

	/*
	 * These are pseudo-levels.
	 */
	imask[IPL_NONE] = 0;
	imask[IPL_HIGH] = -1;

	hw_setintrmask(0);
}

void
ip27_hub_do_pending_int(int newcpl)
{
	/* Update masks to new cpl. Order highly important! */
	__asm__ (" .set noreorder\n");
	cpl = newcpl;
	__asm__ (" sync\n .set reorder\n");
	hw_setintrmask(newcpl);
	/* If we still have softints pending trigger processing. */
	if (ipending & SINT_ALLMASK & ~newcpl)
		setsoftintr0();
}

intrmask_t
ip27_hub_intr_handler(intrmask_t hwpend, struct trap_frame *frame)
{
	uint64_t imr, isr;
	int icpl;
	int bit;
	intrmask_t mask;
	struct intrhand *ih;
	int rc;

	/* XXX this assumes we run on cpu0 */
	isr = IP27_LHUB_L(HUB_IR0);
	imr = IP27_LHUB_L(HUB_CPU0_IMR0);

	isr &= imr;
	if (isr == 0)
		return 0;	/* not for us */

	/*
	 * Mask all pending interrupts.
	 */
	IP27_LHUB_S(HUB_CPU0_IMR0, imr & ~isr);
	(void)IP27_LHUB_L(HUB_IR0);

	/*
	 * If interrupts are spl-masked, mark them as pending only.
	 */
	if ((mask = isr & frame->cpl) != 0) {
		atomic_setbits_int(&ipending, mask);
		isr &= ~mask;
		imr &= ~mask;
	}

	/*
	 * Now process unmasked interrupts.
	 */
	if (isr != 0) {
		atomic_clearbits_int(&ipending, isr);

		__asm__ (" .set noreorder\n");
		icpl = cpl;
		__asm__ (" sync\n .set reorder\n");

		/* XXX Rework this to dispatch in decreasing levels */
		for (bit = SPL_CLOCK - 1, mask = 1 << bit; bit >= 7;
		    bit--, mask >>= 1) {
			if ((isr & mask) == 0)
				continue;

			rc = 0;
			for (ih = intrhand[bit]; ih != NULL; ih = ih->ih_next) {
				splraise(imask[ih->ih_level]);
				ih->frame = frame;
				if ((*ih->ih_fun)(ih->ih_arg) != 0) {
					rc = 1;
					ih->ih_count.ec_count++;
				}
			}
			if (rc == 0)
				printf("spurious interrupt, source %d\n", bit);

			if ((isr ^= mask) == 0)
				break;
		}

		/*
		 * Reenable interrupts which have been serviced.
		 */
		IP27_LHUB_S(HUB_CPU0_IMR0, imr);
		(void)IP27_LHUB_L(HUB_IR0);
		
		__asm__ (" .set noreorder\n");
		cpl = icpl;
		__asm__ (" sync\n .set reorder\n");
	}

	return CR_INT_0;
}

void
hw_setintrmask(intrmask_t m)
{
	IP27_LHUB_S(HUB_CPU0_IMR0, ip27_hub_intrmask & ~((uint64_t)m));
	(void)IP27_LHUB_L(HUB_IR0);
}

void
ip27_nmi(void *arg)
{
	vaddr_t regs_offs;
	register_t *regs, epc;
	struct trap_frame nmi_frame;
	extern int kdb_trap(int, struct trap_frame *);

	/*
	 * Build a ddb frame from the registers saved in the NMI KREGS
	 * area.
	 */

	if (ip35)
		regs_offs = IP35_NMI_KREGS_BASE;	/* XXX assumes cpu0 */
	else
		regs_offs = IP27_NMI_KREGS_BASE;	/* XXX assumes cpu0 */
	regs = IP27_UNCAC_ADDR(register_t *, 0, regs_offs);

	memset(&nmi_frame, 0xff, sizeof nmi_frame);
	
	/* general registers */
	memcpy(&nmi_frame.zero, regs, 32 * sizeof(register_t));
	regs += 32;
	nmi_frame.sr = *regs++;		/* COP_0_STATUS_REG */
	nmi_frame.cause = *regs++;	/* COP_0_CAUSE_REG */
	nmi_frame.pc = *regs++;
	nmi_frame.badvaddr = *regs++;	/* COP_0_BAD_VADDR */
	epc = *regs++;			/* COP_0_EXC_PC */
	regs++;				/* COP_0_CACHE_ERR */
	regs++;				/* NMI COP_0_STATUS_REG */

	setsr(getsr() & ~SR_BOOT_EXC_VEC);
#ifdef DDB
	printf("NMI\n");
	(void)kdb_trap(-1, &nmi_frame);
#else
	printf("NMI, PC = %p RA = %p SR = %08x EPC = %p\n",
	    nmi_frame.pc, nmi_frame.ra, nmi_frame.sr, epc);
#endif
	printf("Resetting system...\n");
	boot(RB_USERREQ);
}
