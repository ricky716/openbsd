/*      $OpenBSD: src/sys/arch/mac68k/dev/Attic/if_snvar.h,v 1.5 1997/04/13 14:14:52 briggs Exp $       */

/*
 * Copyright (c) 1991   Algorithmics Ltd (http://www.algor.co.uk)
 * You may use, copy, and modify this program so long as you retain the
 * copyright line.
 */

/*
 * if_snvar.h -- National Semiconductor DP83932BVF (SONIC) NetBSD/mac68k vars
 */

/*
 * Memory access macros. Since we handle SONIC in 16 bit mode (PB5X0)
 * and 32 bit mode (everything else) using a single GENERIC kernel
 * binary, all structures have to be accessed using macros which can
 * adjust the offsets appropriately.
 */
#define	SWO(m, a, o, x)	(m ? (*(u_int32_t *)((u_int32_t *)a + o) = (x)) : \
			     (*(u_int16_t *)((u_int16_t *)a + o) = (x)))
#define	SRO(m, a, o)	(m ? (*(u_int32_t *)((u_int32_t *)a + o) & 0xffff) : \
			     (*(u_int16_t *)((u_int16_t *)a + o) & 0xffff))

/*
 * Register access macros. We use bus_space_* to talk to the Sonic
 * registers. A mapping table is used in case a particular configuration
 * hooked the regs up at non-word offsets.
 */
#define	NIC_GET(sc, reg)	(bus_space_read_2((sc)->sc_regt,	\
					(sc)->sc_regh,			\
					((sc)->sc_reg_map[reg])))
#define	NIC_PUT(sc, reg, val)	(bus_space_write_2((sc)->sc_regt,	\
					(sc)->sc_regh,			\
					((sc)->sc_reg_map[reg]),	\
					(val)))
#define	SN_REGSIZE	SN_NREGS*4

/* mac68k does not have any write buffers to flush... */
#define	wbflush()

/*
 * buffer sizes in 32 bit mode
 * 1 TXpkt is 4 hdr words + (3 * FRAGMAX) + 1 link word
 * FRAGMAX == 16 => 54 words == 216 bytes
 *
 * 1 RxPkt is 7 words == 28 bytes
 * 1 Rda   is 4 words == 16 bytes
 */

#define NRBA    8		/* # receive buffers < NRRA */
#define RBAMASK (NRBA-1)
#define NRDA    NRBA*4
#define NTDA    4		/* # transmit descriptors */
#define NRRA    32		/* # receive resource descriptors */
#define RRAMASK (NRRA-1)	/* the reason why it must be power of two */

#define FCSSIZE 4		/* size of FCS appended to packets */

/*
 * maximum receive packet size plus 2 byte pad to make each
 * one aligned. 4 byte slop (required for eobc)
 */
#define RBASIZE(sc)	(sizeof(struct ether_header) + ETHERMTU + FCSSIZE + \
			 ((sc)->bitmode ? 6 : 2))

/*
 * transmit buffer area
 */
#define NTXB    10      /* Number of xmit buffers */
#define TXBSIZE 1536    /* 6*2^8 -- the same size as the 8390 TXBUF */

#define	SN_NPAGES	1 + 8 + 5

/*
 * Statistics collected over time
 */
struct sn_stats {
	int     ls_opacks;	/* packets transmitted */
	int     ls_ipacks;	/* packets received */
	int     ls_tdr;		/* contents of tdr after collision */
	int     ls_tdef;	/* packets where had to wait */
	int     ls_tone;	/* packets with one retry */
	int     ls_tmore;	/* packets with more than one retry */
	int     ls_tbuff;	/* transmit buff errors */
	int     ls_tuflo;       /* "      uflo  "     */
	int     ls_tlcol;
	int     ls_tlcar;
	int     ls_trtry;
	int     ls_rbuff;       /* receive buff errors */
	int     ls_rfram;       /* framing     */
	int     ls_roflo;       /* overflow    */
	int     ls_rcrc;
	int     ls_rrng;	/* rx ring sequence error */
	int     ls_babl;	/* chip babl error */
	int     ls_cerr;	/* collision error */
	int     ls_miss;	/* missed packet */
	int     ls_merr;	/* memory error */
	int     ls_copies;      /* copies due to out of range mbufs */
	int     ls_maxmbufs;    /* max mbufs on transmit */
	int     ls_maxslots;    /* max ring slots on transmit */
};

typedef struct mtd {
	void		*mtd_txp;
	int		mtd_vtxp;
	unsigned char	*mtd_buf;
} mtd_t;

/*
 * The sn_softc for Mac68k if_sn.
 */
typedef struct sn_softc {
	struct	device sc_dev;
	struct	arpcom sc_arpcom;
#define	sc_if		sc_arpcom.ac_if		/* network visible interface */
#define	sc_enaddr	sc_arpcom.ac_enaddr	/* hardware ethernet address */

	bus_space_tag_t		sc_regt;
	bus_space_handle_t	sc_regh;

	struct sn_stats	sc_sum;
	short		sc_iflags;
	unsigned short	bitmode;	/* 32 bit mode == 1, 16 == 0 */
	bus_size_t	sc_reg_map[SN_NREGS];	/* register offsets */

	u_int16_t	snr_dcr;	/* DCR for this instance */
	u_int16_t	snr_dcr2;	/* DCR2 for this instance */
	int	slotno;			/* Slot number */

	int	sc_rxmark;		/* pos. in rx ring for reading buffs */
	int	sc_rramark;		/* index into p_rra of wp */
	void *p_rra[NRRA];		/* RX resource descs */
	int	v_rra[NRRA];		/* DMA addresses of p_rra */
	int	v_rea;			/* ptr to the end of the rra space */

	int	sc_rdamark;
	void *p_rda[NRDA];
	int	v_rda[NRDA];

	caddr_t	rbuf[NRBA];

	int	sc_missed;		/* missed packet counter */

	int	txb_cnt;		/* total number of xmit buffers */
	int	txb_inuse;		/* number of active xmit buffers */
	int	txb_new;		/* index of next open slot. */

	struct mtd	mtda[NTDA];
	int		mtd_hw;		/* idx of first mtd given to hw */
	int		mtd_prev;	/* idx of last mtd given to hardware */
	int		mtd_free;	/* next free mtd to use */
	int		mtd_tlinko;	/*
					 * offset of tlink of last txp given
					 * to SONIC. Need to clear EOL on
					 * this word to add a desc.
					 */

	caddr_t		tbuf[NTXB];
	int		vtbuf[NTXB];	/* DMA address of tbuf */

	void		*p_cda;
	int		v_cda;

	unsigned char	*space;
} sn_softc_t;

/*
 * Accessing SONIC data structures and registers as 32 bit values
 * makes code endianess independent.  The SONIC is however always in
 * bigendian mode so it is necessary to ensure that data structures shared
 * between the CPU and the SONIC are always in bigendian order.
 */

/*
 * Receive Resource Descriptor
 * This structure describes the buffers into which packets
 * will be received.  Note that more than one packet may be
 * packed into a single buffer if constraints permit.
 */
#define	RXRSRC_PTRLO	0	/* buffer address LO */
#define	RXRSRC_PTRHI	1	/* buffer address HI */
#define	RXRSRC_WCLO	2	/* buffer size (16bit words) LO */
#define	RXRSRC_WCHI	3	/* buffer size (16bit words) HI */

#define	RXRSRC_SIZE(sc)	(sc->bitmode ? (4 * 4) : (4 * 2))

/*
 * Receive Descriptor
 * This structure holds information about packets received.
 */
#define	RXPKT_STATUS	0
#define	RXPKT_BYTEC	1
#define	RXPKT_PTRLO	2
#define	RXPKT_PTRHI	3
#define	RXPKT_SEQNO	4
#define	RXPKT_RLINK	5
#define	RXPKT_INUSE	6
#define	RXPKT_SIZE(sc)	(sc->bitmode ? (7 * 4) : (7 * 2))

#define RBASEQ(x) (((x)>>8)&0xff)
#define PSNSEQ(x) ((x) & 0xff)

/*
 * Transmit Descriptor
 * This structure holds information about packets to be transmitted.
 */
#define FRAGMAX	8		/* maximum number of fragments in a packet */

#define	TXP_STATUS	0	/* + transmitted packet status */
#define	TXP_CONFIG	1	/* transmission configuration */
#define	TXP_PKTSIZE	2	/* entire packet size in bytes */
#define	TXP_FRAGCNT	3	/* # fragments in packet */

#define	TXP_FRAGOFF	4	/* offset to first fragment */
#define	TXP_FRAGSIZE	3	/* size of each fragment desc */
#define	TXP_FPTRLO	0	/* ptr to packet fragment LO */
#define	TXP_FPTRHI	1	/* ptr to packet fragment HI */
#define	TXP_FSIZE	2	/* fragment size */

#define	TXP_WORDS	TXP_FRAGOFF + (FRAGMAX*TXP_FRAGSIZE) + 1	/* 1 for tlink */
#define	TXP_SIZE(sc)	((sc->bitmode) ? (TXP_WORDS*4) : (TXP_WORDS*2))

#define EOL	0x0001		/* end of list marker for link fields */

/*
 * CDA, the CAM descriptor area. The SONIC has a 16 entry CAM to
 * match incoming addresses against. It is programmed via DMA
 * from a memory region.
 */
#define MAXCAM	16	/* number of user entries in CAM */
#define	CDA_CAMDESC	4	/* # words i na descriptor */
#define	CDA_CAMEP	0	/* CAM Address Port 0 xx-xx-xx-xx-YY-YY */
#define	CDA_CAMAP0	1	/* CAM Address Port 1 xx-xx-YY-YY-xx-xx */
#define	CDA_CAMAP1	2	/* CAM Address Port 2 YY-YY-xx-xx-xx-xx */
#define	CDA_CAMAP2	3
#define	CDA_ENABLE	64	/* mask enabling CAM entries */
#define	CDA_SIZE(sc)	((4*16 + 1) * ((sc->bitmode) ? 4 : 2))

int	snsetup __P((struct sn_softc *sc));
void	sn_get_enaddr __P((bus_space_tag_t t, bus_space_handle_t h,
			   vm_offset_t o, u_char *dst));
void	snintr __P((void *, int));
