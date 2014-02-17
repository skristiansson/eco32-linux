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

static int kstack_depth_to_print = 24;

/* TODO: fancy unwinding */
void show_stack(struct task_struct *task, unsigned long *sp)
{
	int i;
	unsigned long *stack;

	if (sp == 0)
		sp = (unsigned long*)&sp;

	stack = sp;

	printk("\nStack dump [0x%08lx]:\n", (unsigned long)stack);
	for (i = 0; i < kstack_depth_to_print; i++) {
		if (kstack_end(stack))
			break;
		if (i && ((i % 4) == 0))
			printk("\n");
		printk("%08lx ", *stack++);
	}
}

void die(const char *str, struct pt_regs *regs, long err)
{
	console_verbose();
	printk("\n%s#: %04lx\n", str, err & 0xffff);
	show_regs(regs);
	if (!user_mode(regs))
		show_stack(current, (unsigned long*)&regs->r29);
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

#ifdef DEBUG
extern void *sys_call_table[];
#include <linux/unistd.h>
void syscall_debug(struct pt_regs *regs)
{
	pr_debug("%s: syscall %d: func = %x, caller = %x, "
		 "args = %x %x %x %x %x %x\n",
		 __func__, regs->r2, *((u32 *)sys_call_table + regs->r2),
		 regs->pc - 4, regs->r4, regs->r5, regs->r6, regs->r7, regs->r8,
		 regs->r9);
}
#endif

void __init trap_init(void)
{
}
