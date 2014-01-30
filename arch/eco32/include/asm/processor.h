/*
 * Copyright (C) 2014 Stefan Kristiansson, stefan.kristiansson@saunalahti.fi
 *
 * Based on the OpenRISC implementation:
 * Copyright (C) 2003 Matjaz Breskvar <phoenix@bsemi.com>
 * Copyright (C) 2010-2011 Jonas Bonn <jonas@southpole.se>
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2, or (at your option) any later version.
 * This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */

#ifndef __ASM_ECO32_PROCESSOR_H
#define __ASM_ECO32_PROCESSOR_H

#include <asm/page.h>
#include <asm/ptrace.h>

#define STACK_TOP	TASK_SIZE
#define STACK_TOP_MAX	STACK_TOP

/*
 * Default implementation of macro that returns current
 * instruction pointer ("program counter").
 */
#define current_text_addr() ({ __label__ _l; _l: &&_l; })

/*
 * User space process size. This is hardcoded into a few places,
 * so don't change it unless you know what you are doing.
 */

#define TASK_SIZE       (0x80000000UL)

/* This decides where the kernel will search for a free chunk of vm
 * space during mmap's.
 */
#define TASK_UNMAPPED_BASE      (TASK_SIZE / 8 * 3)

#ifndef __ASSEMBLY__

struct task_struct;

struct thread_struct {
};

/*
 * At user->kernel entry, the pt_regs struct is stacked on the top of the
 * kernel-stack.  This macro allows us to find those regs for a task.
 * Notice that subsequent pt_regs stackings, like recursive interrupts
 * occurring while we're in the kernel, won't affect this - only the first
 * user->kernel transition registers are reached by this (i.e. not regs
 * for running signal handler)
 */
#define user_regs(thread_info)	(((struct pt_regs *) \
				  ((unsigned long) \
				   (thread_info) + THREAD_SIZE)) - 1)

/*
 * Dito but for the currently running task
 */

#define task_pt_regs(task) user_regs(task_thread_info(task))

#define INIT_SP         (sizeof(init_stack) + (unsigned long) &init_stack)

#define INIT_THREAD  { }


#define KSTK_EIP(tsk)   (task_pt_regs(tsk)->pc)
#define KSTK_ESP(tsk)   (task_pt_regs(tsk)->r29)


void start_thread(struct pt_regs *regs, unsigned long nip, unsigned long sp);
void release_thread(struct task_struct *);
unsigned long get_wchan(struct task_struct *p);

/*
 * Free current thread data structures etc..
 */

extern inline void exit_thread(void)
{
	/* Nothing needs to be done.  */
}

/*
 * Return saved PC of a blocked thread. For now, this is the "user" PC
 */
extern unsigned long thread_saved_pc(struct task_struct *t);

#define init_stack      (init_thread_union.stack)

#define cpu_relax()     barrier()

#endif /* __ASSEMBLY__ */
#endif /* __ASM_ECO32_PROCESSOR_H */
