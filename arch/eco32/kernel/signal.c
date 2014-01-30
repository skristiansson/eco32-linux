/*
 * Copyright (C) 2014 Stefan Kristiansson, stefan.kristiansson@saunalahti.fi
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2.  This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */

#include <linux/sched.h>
#include <linux/signal.h>
#include <asm/thread_info.h>
#include <linux/tracehook.h>

void do_signal(struct pt_regs *regs)
{
	BUG(); /* SJK TODO */
}

void do_work_pending(struct pt_regs *regs, unsigned int thread_info_flags)
{
	pr_debug("SJK DEBUG: %s: thread_info_flags = %x\n",
		 __func__, thread_info_flags);

        if (thread_info_flags & _TIF_NEED_RESCHED) {
		schedule();
		return;
	}

	local_irq_enable();

        if (thread_info_flags & _TIF_SIGPENDING) {
		do_signal(regs);
		return;
	}

	if (thread_info_flags & _TIF_NOTIFY_RESUME) {
		clear_thread_flag(TIF_NOTIFY_RESUME);
		tracehook_notify_resume(regs);
		return;
	}

	BUG(); /* SJK TODO */
}
