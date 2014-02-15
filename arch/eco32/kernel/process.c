/*
 * Copyright (C) 2014 Stefan Kristiansson, stefan.kristiansson@saunalahti.fi
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2.  This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */

#include <linux/sched.h>
#include <linux/ptrace.h>
#include <linux/uaccess.h> /* for USER_DS macros */
#include <linux/elfcore.h>

/* TODO: add proper implementations */
void machine_restart(void)
{
	pr_notice("Machine restart...\n");
	for(;;);
}

void machine_halt(void)
{
	pr_notice("Machine halt...\n");
	for(;;);
}

void machine_power_off(void)
{
	pr_notice("Machine power off...\n");
	for(;;);
}

void (*pm_power_off) (void) = machine_power_off;

void flush_thread(void)
{
}

void show_regs(struct pt_regs *regs)
{
	printk("CPU #: %d\n"
	       "   PC: %08lx    PSW: %08lx    SP: %08lx\n",
	       smp_processor_id(), regs->pc, regs->psw, regs->r29);
	printk("GPR00: %08lx GPR01: %08lx GPR02: %08lx GPR03: %08lx\n",
	       0L, regs->r1, regs->r2, regs->r3);
	printk("GPR04: %08lx GPR05: %08lx GPR06: %08lx GPR07: %08lx\n",
	       regs->r4, regs->r5, regs->r6, regs->r7);
	printk("GPR08: %08lx GPR09: %08lx GPR10: %08lx GPR11: %08lx\n",
	       regs->r8, regs->r9, regs->r10, regs->r11);
	printk("GPR12: %08lx GPR13: %08lx GPR14: %08lx GPR15: %08lx\n",
	       regs->r12, regs->r13, regs->r14, regs->r15);
	printk("GPR16: %08lx GPR17: %08lx GPR18: %08lx GPR19: %08lx\n",
	       regs->r16, regs->r17, regs->r18, regs->r19);
	printk("GPR20: %08lx GPR21: %08lx GPR22: %08lx GPR23: %08lx\n",
	       regs->r20, regs->r21, regs->r22, regs->r23);
	printk("GPR24: %08lx GPR25: %08lx GPR26: %08lx GPR27: %08lx\n",
	       regs->r24, regs->r25, regs->r26, regs->r27);
	printk("GPR28: %08lx GPR29: %08lx GPR30: %08lx GPR31: %08lx\n",
	       regs->r28, regs->r29, regs->pc, regs->r31);
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
			childregs->r29 = usp;
		childregs->r2 = 0; /* return 0 */
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

	regs->pc = pc;
	regs->psw = psw;
	regs->r29 = sp;
}

/* Fill in the fpu structure for a core dump.  */
int dump_fpu(struct pt_regs *regs, elf_fpregset_t * fpu)
{
	/* eco32 doesn't have any special fpu regs */
	return 0;
}

unsigned long get_wchan(struct task_struct *p)
{
	/* TODO */

	return 0;
}
