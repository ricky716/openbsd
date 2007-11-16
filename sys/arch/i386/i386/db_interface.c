/*	$OpenBSD: src/sys/arch/i386/i386/db_interface.c,v 1.25 2007/11/16 16:16:06 deraadt Exp $	*/
/*	$NetBSD: db_interface.c,v 1.22 1996/05/03 19:42:00 christos Exp $	*/

/*
 * Mach Operating System
 * Copyright (c) 1991,1990 Carnegie Mellon University
 * All Rights Reserved.
 *
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 *
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 *
 * Carnegie Mellon requests users of this software to return to
 *
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 *
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 *
 *	db_interface.c,v 2.4 1991/02/05 17:11:13 mrt (CMU)
 */

/*
 * Interface to new debugger.
 */
#include <sys/param.h>
#include <sys/proc.h>
#include <sys/reboot.h>
#include <sys/systm.h>
#include <sys/mutex.h>

#include <uvm/uvm_extern.h>

#include <dev/cons.h>

#include <machine/db_machdep.h>

#include <ddb/db_sym.h>
#include <ddb/db_command.h>
#include <ddb/db_extern.h>
#include <ddb/db_access.h>
#include <ddb/db_output.h>
#include <ddb/db_run.h>
#include <ddb/db_var.h>

#include "acpi.h"
#if NACPI > 0
#include <dev/acpi/acpidebug.h>
#endif /* NACPI > 0 */

extern label_t	*db_recover;
extern char *trap_type[];
extern int trap_types;

#ifdef MULTIPROCESSOR
extern volatile int ddb_state;
boolean_t	 db_switch_cpu;
long		 db_switch_to_cpu;
#endif

db_regs_t	ddb_regs;
int	db_active = 0;

void kdbprinttrap(int, int);
void db_sysregs_cmd(db_expr_t, int, db_expr_t, char *);
#ifdef MULTIPROCESSOR
void db_cpuinfo_cmd(db_expr_t, int, db_expr_t, char *);
void db_startproc_cmd(db_expr_t, int, db_expr_t, char *);
void db_stopproc_cmd(db_expr_t, int, db_expr_t, char *);
void db_ddbproc_cmd(db_expr_t, int, db_expr_t, char *);
int db_cpuid2apic(int);
#endif /* MULTIPROCESSOR */

/*
 * Print trap reason.
 */
void
kdbprinttrap(int type, int code)
{
	db_printf("kernel: ");
	if (type >= trap_types || type < 0)
		db_printf("type %d", type);
	else
		db_printf("%s", trap_type[type]);
	db_printf(" trap, code=%x\n", code);
}

/*
 *  kdb_trap - field a TRACE or BPT trap
 */
int
kdb_trap(int type, int code, db_regs_t *regs)
{
	int s;

	switch (type) {
	case T_BPTFLT:	/* breakpoint */
	case T_TRCTRAP:	/* single_step */
	case T_NMI:	/* NMI */
	case -1:	/* keyboard interrupt */
		break;
	default:
		if (!db_panic)
			return (0);

		kdbprinttrap(type, code);
		if (db_recover != 0) {
			db_error("Faulted in DDB; continuing...\n");
			/*NOTREACHED*/
		}
	}

#ifdef MULTIPROCESSOR
	mtx_enter(&ddb_mp_mutex);
	if (ddb_state == DDB_STATE_EXITING)
		ddb_state = DDB_STATE_NOT_RUNNING;
	mtx_leave(&ddb_mp_mutex);
	while (db_enter_ddb()) {
#endif /* MULTIPROCESSOR */

	/* XXX Should switch to kdb`s own stack here. */

	ddb_regs = *regs;
	if (KERNELMODE(regs->tf_cs, regs->tf_eflags)) {
		/*
		 * Kernel mode - esp and ss not saved
		 */
		ddb_regs.tf_esp = (int)&regs->tf_esp;	/* kernel stack pointer */
		__asm__("movw %%ss,%w0" : "=r" (ddb_regs.tf_ss));
	}

	s = splhigh();
	db_active++;
	cnpollc(TRUE);
	db_trap(type, code);
	cnpollc(FALSE);
	db_active--;
	splx(s);

	regs->tf_fs     = ddb_regs.tf_fs & 0xffff;
	regs->tf_gs     = ddb_regs.tf_gs & 0xffff;
	regs->tf_es     = ddb_regs.tf_es & 0xffff;
	regs->tf_ds     = ddb_regs.tf_ds & 0xffff;
	regs->tf_edi    = ddb_regs.tf_edi;
	regs->tf_esi    = ddb_regs.tf_esi;
	regs->tf_ebp    = ddb_regs.tf_ebp;
	regs->tf_ebx    = ddb_regs.tf_ebx;
	regs->tf_edx    = ddb_regs.tf_edx;
	regs->tf_ecx    = ddb_regs.tf_ecx;
	regs->tf_eax    = ddb_regs.tf_eax;
	regs->tf_eip    = ddb_regs.tf_eip;
	regs->tf_cs     = ddb_regs.tf_cs & 0xffff;
	regs->tf_eflags = ddb_regs.tf_eflags;
	if (!KERNELMODE(regs->tf_cs, regs->tf_eflags)) {
		/* ring transit - saved esp and ss valid */
		regs->tf_esp    = ddb_regs.tf_esp;
		regs->tf_ss     = ddb_regs.tf_ss & 0xffff;
	}


#ifdef MULTIPROCESSOR
		if (!db_switch_cpu)
			ddb_state = DDB_STATE_EXITING;
	}
#endif /* MULTIPROCESSOR */
	return (1);
}

void
db_sysregs_cmd(db_expr_t addr, int have_addr, db_expr_t count, char *modif)
{
	int64_t idtr, gdtr;
	uint32_t cr;
	uint16_t ldtr, tr;

	__asm__ __volatile__("sidt %0" : "=m" (idtr));
	db_printf("idtr:   0x%08x/%04x\n",
	    (unsigned int)(idtr >> 16), idtr & 0xffff);

	__asm__ __volatile__("sgdt %0" : "=m" (gdtr));
	db_printf("gdtr:   0x%08x/%04x\n",
	    (unsigned int)(gdtr >> 16), gdtr & 0xffff);

	__asm__ __volatile__("sldt %0" : "=g" (ldtr));
	db_printf("ldtr:   0x%04x\n", ldtr);

	__asm__ __volatile__("str %0" : "=g" (tr));
	db_printf("tr:     0x%04x\n", tr);

	__asm__ __volatile__("movl %%cr0,%0" : "=r" (cr));
	db_printf("cr0:    0x%08x\n", cr);

	__asm__ __volatile__("movl %%cr2,%0" : "=r" (cr));
	db_printf("cr2:    0x%08x\n", cr);

	__asm__ __volatile__("movl %%cr3,%0" : "=r" (cr));
	db_printf("cr3:    0x%08x\n", cr);

	__asm__ __volatile__("movl %%cr4,%0" : "=r" (cr));
	db_printf("cr4:    0x%08x\n", cr);
}

#ifdef MULTIPROCESSOR
int
db_cpuid2apic(int id)
{
	int apic;

	for (apic = 0; apic < MAXCPUS; apic++) {
		if (cpu_info[apic] != NULL &&
		    cpu_info[apic]->ci_dev.dv_unit == id)
			return (apic);
	}
	return (-1);
}

void
db_cpuinfo_cmd(db_expr_t addr, int have_addr, db_expr_t count, char *modif)
{
	int i;

	for (i = 0; i < MAXCPUS; i++) {
		if (cpu_info[i] != NULL) {
			db_printf("%c%4d: ", (i == cpu_number()) ? '*' : ' ',
			    cpu_info[i]->ci_dev.dv_unit);
			switch(cpu_info[i]->ci_ddb_paused) {
			case CI_DDB_RUNNING:
				db_printf("running\n");
				break;
			case CI_DDB_SHOULDSTOP:
				db_printf("stopping\n");
				break;
			case CI_DDB_STOPPED:
				db_printf("stopped\n");
				break;
			case CI_DDB_ENTERDDB:
				db_printf("entering ddb\n");
				break;
			case CI_DDB_INDDB:
				db_printf("ddb\n");
				break;
			default:
				db_printf("? (%d)\n",
				    cpu_info[i]->ci_ddb_paused);
				break;
			}
		}
	}
}

void
db_startproc_cmd(db_expr_t addr, int have_addr, db_expr_t count, char *modif)
{
	int apic;

	if (have_addr) {
		apic = db_cpuid2apic(addr);
		if (apic >= 0 && apic < MAXCPUS &&
		    cpu_info[apic] != NULL && apic != cpu_number())
			db_startcpu(apic);
		else
			db_printf("Invalid cpu %d\n", (int)addr);
	} else {
		for (apic = 0; apic < MAXCPUS; apic++) {
			if (cpu_info[apic] != NULL && apic != cpu_number()) {
				db_startcpu(apic);
			}
		}
	}
}

void
db_stopproc_cmd(db_expr_t addr, int have_addr, db_expr_t count, char *modif)
{
	int apic;

	if (have_addr) {
		apic = db_cpuid2apic(addr);
		if (apic >= 0 && apic < MAXCPUS &&
		    cpu_info[apic] != NULL && apic != cpu_number())
			db_stopcpu(apic);
		else
			db_printf("Invalid cpu %d\n", (int)addr);
	} else {
		for (apic = 0; apic < MAXCPUS; apic++) {
			if (cpu_info[apic] != NULL && apic != cpu_number()) {
				db_stopcpu(apic);
			}
		}
	}
}

void
db_ddbproc_cmd(db_expr_t addr, int have_addr, db_expr_t count, char *modif)
{
	int apic;

	if (have_addr) {
		apic = db_cpuid2apic(addr);
		if (apic >= 0 && apic < MAXCPUS &&
		    cpu_info[apic] != NULL && apic != cpu_number()) {
			db_stopcpu(apic);
			db_switch_to_cpu = apic;
			db_switch_cpu = 1;
			db_cmd_loop_done = 1;
		} else {
			db_printf("Invalid cpu %d\n", (int)addr);
		}
	} else {
		db_printf("CPU not specified\n");
	}
}
#endif /* MULTIPROCESSOR */

#if NACPI > 0
struct db_command db_acpi_cmds[] = {
	{ "disasm",	db_acpi_disasm,		CS_OWN,	NULL },
	{ "showval",	db_acpi_showval,	CS_OWN,	NULL },
	{ "tree",	db_acpi_tree,		0,	NULL },
	{ "trace",	db_acpi_trace,		0,	NULL },
	{ NULL,		NULL,			0,	NULL }
};
#endif /* NACPI > 0 */

struct db_command db_machine_command_table[] = {
	{ "sysregs",	db_sysregs_cmd,		0,	0 },
#ifdef MULTIPROCESSOR
	{ "cpuinfo",	db_cpuinfo_cmd,		0,	0 },
	{ "startcpu",	db_startproc_cmd,	0,	0 },
	{ "stopcpu",	db_stopproc_cmd,	0,	0 },
	{ "ddbcpu",	db_ddbproc_cmd,		0,	0 },
#endif /* MULTIPROCESSOR */
#if NACPI > 0
	{ "acpi",	NULL,			0,	db_acpi_cmds },
#endif /* NACPI > 0 */
	{ (char *)0, }
};

void
db_machine_init(void)
{
#ifdef MULTIPROCESSOR
	int i;
#endif /* MULTIPROCESSOR */

	db_machine_commands_install(db_machine_command_table);
#ifdef MULTIPROCESSOR
	for (i = 0; i < MAXCPUS; i++) {
		if (cpu_info[i] != NULL)
			cpu_info[i]->ci_ddb_paused = CI_DDB_RUNNING;
	}
#endif /* MULTIPROCESSOR */
}

void
Debugger(void)
{
	__asm__("int $3");
}
