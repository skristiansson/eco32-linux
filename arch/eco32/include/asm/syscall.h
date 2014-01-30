/*
 * Copyright (C) 2014 Stefan Kristiansson, stefan.kristiansson@saunalahti.fi
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2.  This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */

#ifndef __ASM_ECO32_SYSCALL_H__
#define __ASM_ECO32_SYSCALL_H__

#include <linux/err.h>
#include <linux/sched.h>

static inline int
syscall_get_nr(struct task_struct *task, struct pt_regs *regs)
{
	return regs->orig_r2;
}

static inline void
syscall_rollback(struct task_struct *task, struct pt_regs *regs)
{
	regs->r2 = regs->orig_r2;
}

static inline long
syscall_get_error(struct task_struct *task, struct pt_regs *regs)
{
	return IS_ERR_VALUE(regs->r2) ? regs->r2 : 0;
}

static inline long
syscall_get_return_value(struct task_struct *task, struct pt_regs *regs)
{
	return regs->r2;
}

static inline void
syscall_set_return_value(struct task_struct *task, struct pt_regs *regs,
			 int error, long val)
{
	regs->r2 = (long) error ?: val;
}

static inline void
syscall_get_arguments(struct task_struct *task, struct pt_regs *regs,
		      unsigned int i, unsigned int n, unsigned long *args)
{
	BUG_ON(i + n > 4);

	memcpy(args, &regs->r4 + i, n * sizeof(args[0]));
}

static inline void
syscall_set_arguments(struct task_struct *task, struct pt_regs *regs,
		      unsigned int i, unsigned int n, const unsigned long *args)
{
	BUG_ON(i + n > 4);

	memcpy(&regs->r4 + i, args, n * sizeof(args[0]));
}

#endif
