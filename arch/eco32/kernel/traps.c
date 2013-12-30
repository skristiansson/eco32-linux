/*
 * Copyright (C) 2014 Stefan Kristiansson, stefan.kristiansson@saunalahti.fi
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2.  This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */

#include <linux/init.h>
#include <linux/ptrace.h>

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

void __init trap_init(void)
{
}
