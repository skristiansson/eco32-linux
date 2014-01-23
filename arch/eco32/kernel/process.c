/*
 * Copyright (C) 2014 Stefan Kristiansson, stefan.kristiansson@saunalahti.fi
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2.  This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */

/* SJK check if all those includes are really necessary... */
#define __KERNEL_SYSCALLS__
#include <stdarg.h>

#include <linux/errno.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mm.h>
#include <linux/stddef.h>
#include <linux/unistd.h>
#include <linux/ptrace.h>
#include <linux/slab.h>
#include <linux/elfcore.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/init_task.h>
#include <linux/mqueue.h>
#include <linux/fs.h>

#include <asm/uaccess.h>
#include <asm/pgtable.h>
#include <asm/io.h>
#include <asm/processor.h>

#include <linux/smp.h>

#include <asm/spr.h>

void machine_restart(void)
{
	/* SJK TODO */
}

void machine_halt(void)
{
	/* SJK TODO */
}

void machine_power_off(void)
{
	/* SJK TODO */
}

void (*pm_power_off) (void) = machine_power_off;

void flush_thread(void)
{
}

void show_regs(struct pt_regs *regs)
{
	printk("CPU #: %d\n"
	       "   PC: %08lx    PSW: %08lx    SP: %08lx\n",
	       smp_processor_id(), regs->pc, regs->psw, regs->gpr[29]);
	printk("GPR00: %08lx GPR01: %08lx GPR02: %08lx GPR03: %08lx\n",
	       0L, regs->gpr[1], regs->gpr[2], regs->gpr[3]);
	printk("GPR04: %08lx GPR05: %08lx GPR06: %08lx GPR07: %08lx\n",
	       regs->gpr[4], regs->gpr[5], regs->gpr[6], regs->gpr[7]);
	printk("GPR08: %08lx GPR09: %08lx GPR10: %08lx GPR11: %08lx\n",
	       regs->gpr[8], regs->gpr[9], regs->gpr[10], regs->gpr[11]);
	printk("GPR12: %08lx GPR13: %08lx GPR14: %08lx GPR15: %08lx\n",
	       regs->gpr[12], regs->gpr[13], regs->gpr[14], regs->gpr[15]);
	printk("GPR16: %08lx GPR17: %08lx GPR18: %08lx GPR19: %08lx\n",
	       regs->gpr[16], regs->gpr[17], regs->gpr[18], regs->gpr[19]);
	printk("GPR20: %08lx GPR21: %08lx GPR22: %08lx GPR23: %08lx\n",
	       regs->gpr[20], regs->gpr[21], regs->gpr[22], regs->gpr[23]);
	printk("GPR24: %08lx GPR25: %08lx GPR26: %08lx GPR27: %08lx\n",
	       regs->gpr[24], regs->gpr[25], regs->gpr[26], regs->gpr[27]);
	printk("GPR28: %08lx GPR29: %08lx GPR30: %08lx GPR31: %08lx\n",
	       regs->gpr[28], regs->gpr[29], regs->gpr[30], regs->gpr[31]);
	printk("  RES: %08lx\n",
	       regs->gpr[31]);
}

unsigned long thread_saved_pc(struct task_struct *t)
{
	return (unsigned long)user_regs(t->stack)->pc;
}

void release_thread(struct task_struct *dead_task)
{
}

extern asmlinkage void ret_from_fork(void);
extern asmlinkage void ret_from_kernel_thread(void);

int
copy_thread(unsigned long clone_flags, unsigned long usp,
	    unsigned long arg, struct task_struct *p)
{
	struct thread_info *ti = task_thread_info(p);
	struct pt_regs *childregs = task_pt_regs(p);

	memset(&ti->cpu_context, 0, sizeof(struct cpu_context));

	/* Create a kernel thread */
	if (unlikely(p->flags & PF_KTHREAD)) {
		memset(childregs, 0, sizeof(struct pt_regs));
		ti->cpu_context.r16 = usp;
		ti->cpu_context.r17 = arg;
		ti->cpu_context.r31 = (unsigned long)ret_from_kernel_thread;
	} else {
		*childregs = *current_pt_regs();
		if (usp)
			childregs->gpr[29] = usp;
		childregs->gpr[2] = 0; /* return 0 */
		ti->cpu_context.r31 = (unsigned long)ret_from_fork;
	}

	ti->cpu_context.r29 = (unsigned long)childregs;

	return 0;
}

/*
 * Set up a thread for executing a new program
 */
void start_thread(struct pt_regs *regs, unsigned long pc, unsigned long sp)
{
	unsigned long psw = mvfs(SPR_PSW) | SPR_PSW_UP;
	set_fs(USER_DS);
	memset(regs, 0, sizeof(struct pt_regs));

	pr_info("SJK DEBUG: %s: sp = %x, regs = %x\n",
		__func__, sp, regs);

	regs->pc = pc;
	regs->psw = psw;
	regs->gpr[29] = sp;
}

/* Fill in the fpu structure for a core dump.  */
int dump_fpu(struct pt_regs *regs, elf_fpregset_t * fpu)
{
	/* eco32 doesn't have any special fpu regs */
	return 0;
}

/*
 * Write out registers in core dump format, as defined by the
 * struct user_regs_struct
 */
void dump_elf_thread(elf_greg_t *dest, struct pt_regs* regs)
{
	memcpy(dest, regs->gpr, 32*sizeof(unsigned long));
	dest[32] = regs->pc;
	dest[33] = regs->psw;
}

unsigned long get_wchan(struct task_struct *p)
{
	/* TODO */

	return 0;
}
