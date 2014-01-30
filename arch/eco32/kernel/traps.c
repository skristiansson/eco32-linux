/*
 * Copyright (C) 2014 Stefan Kristiansson, stefan.kristiansson@saunalahti.fi
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2.  This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */

#include <linux/init.h>
#include <linux/ptrace.h>
#include <asm/irq.h>
#include <asm/spr.h>

extern void do_page_fault(struct pt_regs *regs, unsigned long address);

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

/*
 * Dispatch exceptions, traps (syscalls) take a different path.
 */
void do_exception(struct pt_regs *regs)
{
	siginfo_t info;
	int eid = (regs->psw >> SPR_PSW_EID_BIT) & 0x1f;

	/* External interrupts are present as the lower numbers of eid */
	if (eid < NR_IRQS) {
		do_IRQ(eid, regs);
		return;
	}

	switch (eid) {
	case EID_TLB_MISS:
	case EID_TLB_WRITE:
	case EID_TLB_INVALID:
		do_page_fault(regs, mvfs(SPR_TLB_BAD_ADDRESS));
		return;

	case EID_ILLEGAL_INSN:
		if (user_mode(regs)) {
			info.si_signo = SIGILL;
			info.si_errno = 0;
			info.si_code = ILL_ILLOPC;
			info.si_addr = (void *)regs->pc;
			force_sig_info(SIGBUS, &info, current);
			return;
		}
	default:
		break;
	}

	die("Unhandled exception", regs, eid);
}

/* SJK DEBUG */
extern void *sys_call_table[];
void syscall_debug(struct pt_regs *regs)
{
	pr_debug("SJK DEBUG: syscall %d: func = %x, caller = %x, "
		 "args = %x %x %x %x %x %x\n",
		 regs->r2, *((u32 *)sys_call_table + regs->r2),
		 regs->pc, regs->r4, regs->r5, regs->r6,
		 regs->r7, regs->r8, regs->r9);
}
/* SJK DEBUG END */

void __init trap_init(void)
{
}
