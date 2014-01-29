/*
 * Copyright (C) 2014 Stefan Kristiansson, stefan.kristiansson@saunalahti.fi
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2.  This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */

#include <linux/init.h>
#include <linux/ptrace.h>
#include <asm/spr.h>

void show_trace(struct task_struct *task, unsigned long *stack)
{
	/* SJK TODO */
}

void show_stack(struct task_struct *task, unsigned long *esp)
{
	/* SJK TODO */
}

void show_trace_task(struct task_struct *tsk)
{
	/* SJK TODO */
}

void die(const char *str, struct pt_regs *regs, long err)
{
	console_verbose();
	printk("\n%s#: %04lx\n", str, err & 0xffff);
	show_regs(regs);
	do_exit(SIGSEGV);
}

void die_if_kernel(const char *str, struct pt_regs *regs, long err)
{
	if (user_mode(regs))
		return;

	die(str, regs, err);
}

void do_exception(struct pt_regs *regs)
{
	die("Unhandled exception", regs, (regs->psw >> SPR_PSW_EID_BIT) & 0x1f);
}

/* SJK DEBUG */
extern void *sys_call_table[];
void syscall_debug(struct pt_regs *regs)
{
	pr_debug("SJK DEBUG: syscall %d: func = %x, caller = %x, "
		 "args = %x %x %x %x %x %x\n",
		 regs->gpr[25], *((u32 *)sys_call_table + regs->gpr[25]),
		 regs->pc, regs->gpr[4], regs->gpr[5], regs->gpr[6],
		 regs->gpr[7], regs->gpr[8], regs->gpr[9]);
}
/* SJK DEBUG END */

void __init trap_init(void)
{
}
