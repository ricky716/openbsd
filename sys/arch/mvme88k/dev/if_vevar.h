/*	$OpenBSD: src/sys/arch/mvme88k/dev/Attic/if_vevar.h,v 1.3 2002/03/14 01:26:39 millert Exp $	*/

/*
 * Copyright (c) 1995 Charles M. Hannum.  All rights reserved.
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
 *	This product includes software developed by Charles M. Hannum.
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

#define LEDEBUG 1

/*
 * Ethernet software status per device.
 *
 * Each interface is referenced by a network interface structure,
 * arpcom.ac_if, which the routing code uses to locate the interface. 
 * This structure contains the output queue for the interface, its address, ...
 *
 * NOTE: this structure MUST be the first element in machine-dependent
 * le_softc structures!  This is designed SPECIFICALLY to make it possible
 * to simply cast a "void *" to "struct le_softc *" or to
 * "struct ve_softc *".  Among other things, this saves a lot of hair
 * in the interrupt handlers.
 */
struct vam7990_softc {
	struct	device sc_dev;		/* base device glue */
	struct	arpcom sc_arpcom;	/* Ethernet common part */

	/*
	 * Memory functions:
	 *
	 *	copy to/from descriptor
	 *	copy to/from buffer
	 *	zero bytes in buffer
	 */
	void	(*sc_copytodesc)(struct vam7990_softc *, void *, int, int);
	void	(*sc_copyfromdesc)(struct vam7990_softc *, void *, int, int);
	void	(*sc_copytobuf)(struct vam7990_softc *, void *, int, int);
	void	(*sc_copyfrombuf)(struct vam7990_softc *, void *, int, int);
	void	(*sc_zerobuf)(struct vam7990_softc *, int, int);

	/*
	 * Machine-dependent functions:
	 *
	 *	read/write CSR
	 *	hardware reset hook - may be NULL
	 *	hardware init hook - may be NULL
	 *	no carrier hook - may be NULL
	 */
	u_int16_t (*sc_rdcsr)(struct vam7990_softc *, u_int16_t);
	void	(*sc_wrcsr)(struct vam7990_softc *, u_int16_t, u_int16_t);
	void	(*sc_hwreset)(struct vam7990_softc *);
	void	(*sc_hwinit)(struct vam7990_softc *);
	void	(*sc_nocarrier)(struct vam7990_softc *);

	int	sc_hasifmedia;
	struct	ifmedia sc_ifmedia;

	void	*sc_sh;		/* shutdownhook cookie */

	u_int16_t sc_conf3;	/* CSR3 value */

	void	*sc_mem;	/* base address of RAM -- CPU's view */
	u_long	sc_addr;	/* base address of RAM -- LANCE's view */

	u_long	sc_memsize;	/* size of RAM */

	int	sc_nrbuf;	/* number of receive buffers */
	int	sc_ntbuf;	/* number of transmit buffers */
	int	sc_last_rd;
	int	sc_first_td, sc_last_td, sc_no_td;

	int	sc_initaddr;
	int	sc_rmdaddr;
	int	sc_tmdaddr;
	int	sc_rbufaddr;
	int	sc_tbufaddr;
   unsigned short csr;
#ifdef LEDEBUG
	int	sc_debug;
#endif
};

/* Export this to machine-dependent drivers. */
extern struct cfdriver ve_cd;

void ve_config(struct vam7990_softc *);
void ve_init(struct vam7990_softc *);
int ve_ioctl(struct ifnet *, u_long, caddr_t);
void ve_meminit(struct vam7990_softc *);
void ve_reset(struct vam7990_softc *);
void ve_setladrf(struct arpcom *, u_int16_t *);
void ve_start(struct ifnet *);
void ve_stop(struct vam7990_softc *);
void ve_watchdog(struct ifnet *);
int ve_intr(void *);

/*
 * The following functions are only useful on certain cpu/bus
 * combinations.  They should be written in assembly language for
 * maximum efficiency, but machine-independent versions are provided
 * for drivers that have not yet been optimized.
 */
void ve_copytobuf_contig(struct vam7990_softc *, void *, int, int);
void ve_copyfrombuf_contig(struct vam7990_softc *, void *, int, int);
void ve_zerobuf_contig(struct vam7990_softc *, int, int);



