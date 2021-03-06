/*
 * Copyright (C) 2014 Stefan Kristiansson, stefan.kristiansson@saunalahti.fi
 *
 * Based on the OpenRISC implementation:
 * Copyright (C) 2003 Matjaz Breskvar <phoenix@bsemi.com>
 * Copyright (C) 2005 Gyorgy Jeney <nog@bsemi.com>
 * Copyright (C) 2010-2011 Jonas Bonn <jonas@southpole.se>
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2, or (at your option) any later version.
 * This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */

#include <stddef.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/string.h>

#include <linux/mm.h>
#include <linux/errno.h>
#include <linux/ptrace.h>
#include <linux/audit.h>
#include <linux/regset.h>
#include <linux/tracehook.h>
#include <linux/elf.h>

#include <asm/asm-offsets.h>
#include <asm/thread_info.h>
#include <asm/segment.h>
#include <asm/page.h>
#include <asm/pgtable.h>

/*
 * Copy the thread state to a regset that can be interpreted by userspace.
 *
 * (Each item is a 32-bit word)
 * $0 = 0 (exported for clarity)
 * 31 GPRS $1-$31
 * PSW (Processor Status Word)
 */
static int genregs_get(struct task_struct *target,
		       const struct user_regset *regset,
		       unsigned int pos, unsigned int count,
		       void *kbuf, void __user * ubuf)
{
	const struct pt_regs *regs = task_pt_regs(target);
	int ret;

	/* $0 */
	ret = user_regset_copyout_zero(&pos, &count, &kbuf, &ubuf,
				       PT_GPR0, PT_GPR1);
	if (!ret)
		ret = user_regset_copyout(&pos, &count, &kbuf, &ubuf,
					  &regs->r1, PT_GPR1, PT_ORIG_GPR2);
	if (!ret)
		ret = user_regset_copyout_zero(&pos, &count, &kbuf, &ubuf,
					       PT_ORIG_GPR2, -1);

	return ret;
}

/*
 * Set the thread state from a regset passed in via ptrace
 */
static int genregs_set(struct task_struct *target,
		       const struct user_regset *regset,
		       unsigned int pos, unsigned int count,
		       const void *kbuf, const void __user * ubuf)
{
	struct pt_regs *regs = task_pt_regs(target);
	int ret;

	/* ignore $0 */
	ret = user_regset_copyin_ignore(&pos, &count, &kbuf, &ubuf, PT_GPR0,
					PT_GPR1);
	/* $1 - $31 */
	if (!ret)
		ret = user_regset_copyin(&pos, &count, &kbuf, &ubuf,
					 &regs->r1, PT_GPR1, PT_GPR31);
	/*
	 * Skip PSW, userspace isn't allowed to changes bits in
	 * the Supervision register
	 */

	if (!ret)
		ret = user_regset_copyin_ignore(&pos, &count, &kbuf, &ubuf,
						PT_ORIG_GPR2, -1);

	return ret;
}

/*
 * Define the register sets available on eco32 under Linux
 */
enum eco32_regset {
	REGSET_GENERAL,
};

static const struct user_regset eco32_regsets[] = {
	[REGSET_GENERAL] = {
			    .core_note_type = NT_PRSTATUS,
			    .n = ELF_NGREG,
			    .size = sizeof(long),
			    .align = sizeof(long),
			    .get = genregs_get,
			    .set = genregs_set,
			    },
};

static const struct user_regset_view user_eco32_native_view = {
	.name = "or1k",
	.e_machine = EM_ECO32,
	.regsets = eco32_regsets,
	.n = ARRAY_SIZE(eco32_regsets),
};

const struct user_regset_view *task_user_regset_view(struct task_struct *task)
{
	return &user_eco32_native_view;
}

/*
 * does not yet catch signals sent when the child dies.
 * in exit.c or in signal.c.
 */


/*
 * Called by kernel/ptrace.c when detaching..
 *
 * Make sure the single step bit is not set.
 */
void ptrace_disable(struct task_struct *child)
{
	pr_debug("ptrace_disable(): TODO\n");

	user_disable_single_step(child);
	clear_tsk_thread_flag(child, TIF_SYSCALL_TRACE);
}

long arch_ptrace(struct task_struct *child, long request, unsigned long addr,
		 unsigned long data)
{
	int ret;

	switch (request) {
	default:
		ret = ptrace_request(child, request, addr, data);
		break;
	}

	return ret;
}

/*
 * Notification of system call entry/exit
 * - triggered by current->work.syscall_trace
 */
asmlinkage long do_syscall_trace_enter(struct pt_regs *regs)
{
	long ret = 0;

	if (test_thread_flag(TIF_SYSCALL_TRACE) &&
	    tracehook_report_syscall_entry(regs))
		/*
		 * Tracing decided this syscall should not happen.
		 * We'll return a bogus call number to get an ENOSYS
		 * error, but leave the original number in <something>.
		 */
		ret = -1L;

	audit_syscall_entry(AUDIT_ARCH_ECO32, regs->r2,
			    regs->r4, regs->r5,
			    regs->r6, regs->r7);

	return ret ? : regs->r2;
}

asmlinkage void do_syscall_trace_leave(struct pt_regs *regs)
{
	int step;

	audit_syscall_exit(regs);

	step = test_thread_flag(TIF_SINGLESTEP);
	if (step || test_thread_flag(TIF_SYSCALL_TRACE))
		tracehook_report_syscall_exit(regs, step);
}
