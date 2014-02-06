/*
 * Copyright (C) 2014 Stefan Kristiansson, stefan.kristiansson@saunalahti.fi
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2.  This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */

#include <linux/sched.h>
#include <linux/signal.h>
#include <linux/tracehook.h>
#include <linux/unistd.h>
#include <asm/thread_info.h>
#include <asm/syscall.h>
#include <asm/uaccess.h>
#include <asm/ucontext.h>

#define INSN_ORI_R2	0x4c020000
#define INSN_TRAP	0xb8000000

struct rt_sigframe {
	struct siginfo info;
	struct ucontext uc;
	unsigned long retcode[2];
};


static int
restore_sigcontext(struct sigcontext __user *sc, struct pt_regs *regs)
{
	int err = 0;

#define COPY(x) err |= __get_user(regs->x, &sc->regs.x)
	COPY(r1);
	COPY(r2);
	COPY(r3);
	COPY(r4);
	COPY(r5);
	COPY(r6);
	COPY(r7);
	COPY(r8);
	COPY(r9);
	COPY(r10);
	COPY(r11);
	COPY(r12);
	COPY(r13);
	COPY(r14);
	COPY(r15);
	COPY(r16);
	COPY(r17);
	COPY(r18);
	COPY(r19);
	COPY(r20);
	COPY(r21);
	COPY(r22);
	COPY(r23);
	COPY(r24);
	COPY(r25);
	COPY(r26);
	COPY(r27);
	COPY(r28);
	COPY(r29);
	COPY(r31);

	COPY(psw);
	COPY(pc);

#undef COPY
	/* make sure we return in user mode */
	regs->psw |= SPR_PSW_UP;

	return err;
}

asmlinkage long do_sys_rt_sigreturn(struct pt_regs *regs)
{
	struct rt_sigframe __user *frame;

	/* Always make any pending restarted system calls return -EINTR */
	current_thread_info()->restart_block.fn = do_no_restart_syscall;

	/*
	 * Since we stacked the signal on a 64-bit boundary,
	 * then 'sp' should be word aligned here.  If it's
	 * not, then the user is trying to mess with us.
	 */
	if (regs->r29 & 3)
		goto badframe;

	frame = (struct rt_sigframe __user *)regs->r29;

	if (!access_ok(VERIFY_READ, frame, sizeof (*frame)))
		goto badframe;

	if (restore_sigcontext(&frame->uc.uc_mcontext, regs))
		goto badframe;

	if (restore_altstack(&frame->uc.uc_stack))
		goto badframe;

	return regs->r2;

badframe:
	force_sig(SIGSEGV, current);
	return 0;
}

static inline void __user *align_sigframe(unsigned long sp)
{
	return (void __user *)(sp & ~3UL);
}

static inline void __user *
get_sigframe(struct ksignal *ksig, struct pt_regs *regs, int framesize)
{
	void __user *frame;

	frame = align_sigframe(sigsp(regs->r29, ksig) - framesize);

	/*
	 * Check that we can actually write to the signal frame.
	 */
	if (!access_ok(VERIFY_WRITE, frame, framesize))
		frame = NULL;

	return frame;
}

static int setup_sigcontext(struct sigcontext *sc, struct pt_regs *regs)
{
	int err = 0;

#define COPY(x)	err |= __put_user(regs->x, &sc->regs.x)
	COPY(r1);
	COPY(r2);
	COPY(r3);
	COPY(r4);
	COPY(r5);
	COPY(r6);
	COPY(r7);
	COPY(r8);
	COPY(r9);
	COPY(r10);
	COPY(r11);
	COPY(r12);
	COPY(r13);
	COPY(r14);
	COPY(r15);
	COPY(r16);
	COPY(r17);
	COPY(r18);
	COPY(r19);
	COPY(r20);
	COPY(r21);
	COPY(r22);
	COPY(r23);
	COPY(r24);
	COPY(r25);
	COPY(r26);
	COPY(r27);
	COPY(r28);
	COPY(r29);
	COPY(r31);

	COPY(psw);
	COPY(pc);

#undef COPY

	return err;
}

/*
 * Setup a frame for a signal. Despite the name, this handles both realtime
 * signals and normal signals.
 */
static int
setup_rt_frame(struct ksignal *ksig, sigset_t *set, struct pt_regs *regs)
{
	int err = 0;
	unsigned long __user *retcode;
	struct rt_sigframe __user *frame =
		get_sigframe(ksig, regs, sizeof(*frame));

	printk("SJK DEBUG: %s: sig = %d\n", __func__, ksig->sig);

	if (!frame)
		return 1;

	if (ksig->ka.sa.sa_flags & SA_SIGINFO)
		err |= copy_siginfo_to_user(&frame->info, &ksig->info);

	/* Create the user context.  */
	err |= __put_user(0, &frame->uc.uc_flags);
	err |= __put_user(0, &frame->uc.uc_link);
	err |= __save_altstack(&frame->uc.uc_stack, regs->r29);
	err |= setup_sigcontext(&frame->uc.uc_mcontext, regs);
	err |= __copy_to_user(&frame->uc.uc_sigmask, set, sizeof(*set));

	/* Return from userspace. */
	retcode = (unsigned long __user *)&frame->retcode;
	err |= __put_user(INSN_ORI_R2 | __NR_rt_sigreturn,	retcode + 0);
	err |= __put_user(INSN_TRAP,				retcode + 1);

	regs->pc = (unsigned long)ksig->ka.sa.sa_handler;
	regs->r31 = (unsigned long)retcode;
	regs->r29 = (unsigned long)frame;

	/* Setup args. */
	regs->r4 = ksig->sig;
	regs->r5 = (unsigned long)&frame->info;
	regs->r6 = (unsigned long)&frame->uc;

	return err;
}

static void handle_signal(struct ksignal *ksig, struct pt_regs *regs)
{
	sigset_t *oldset = sigmask_to_save();
	int err;

	/*
	 * Set up the stack frame
         */
	err = setup_rt_frame(ksig, oldset, regs);

	signal_setup_done(err, ksig, 0);
}

void do_signal(struct pt_regs *regs)
{
	struct ksignal ksig;

	if (get_signal(&ksig)) {
		/* Whee! Actually deliver the signal.  */
		handle_signal(&ksig, regs);
		return;
	}

        /* Did we come from a system call? */
        if (syscall_get_nr(current, regs) >= 0) {
		/* Restart the system call - no handlers present */
		switch (syscall_get_error(current, regs)) {
		case -ERESTARTNOHAND:
		case -ERESTARTSYS:
                case -ERESTARTNOINTR:
			regs->r2 = regs->orig_r2;
			regs->pc -=4;
			break;

                case -ERESTART_RESTARTBLOCK:
			regs->r2 = __NR_restart_syscall;
			regs->pc -= 4;
			break;
		}
	}

	/*
         * If there's no signal to deliver, we just put the saved sigmask
         * back.
         */
	restore_saved_sigmask();
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
