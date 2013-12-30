/*
 * Copyright (C) 2014 Stefan Kristiansson, stefan.kristiansson@saunalahti.fi
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2.  This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */

#ifndef __ASM_ECO32_THREAD_INFO_H
#define __ASM_ECO32_THREAD_INFO_H

#ifdef __KERNEL__

#ifndef __ASSEMBLY__
#include <asm/types.h>
#include <asm/processor.h>
#endif


/* eco32 have 8k threads */
#define THREAD_SIZE_ORDER	1
#define THREAD_SIZE		(PAGE_SIZE << THREAD_SIZE_ORDER)

/*
 * low level task data that entry.S needs immediate access to
 * - this struct should fit entirely inside of one cache line
 * - this struct shares the supervisor stack pages
 * - if the contents of this structure are changed, the assembly constants
 *   must also be changed
 */
#ifndef __ASSEMBLY__

typedef unsigned long mm_segment_t;

struct cpu_context {
	/* callee-saved registers */
	__u32	r16;
	__u32	r17;
	__u32	r18;
	__u32	r19;
	__u32	r20;
	__u32	r21;
	__u32	r22;
	__u32	r23;
	/* frame pointer */
	__u32	r28;
	/* stack pointer */
	__u32	r29;
	/* return address */
	__u32	r31;
};


struct thread_info {
	struct task_struct	*task;		/* main task structure */
	struct exec_domain	*exec_domain;	/* execution domain */
	unsigned long		flags;		/* low level flags */
	__u32			cpu;		/* current CPU */
	__s32			preempt_count; /* 0 => preemptable, <0 => BUG */

	mm_segment_t		addr_limit;	/* thread address space */
	struct restart_block    restart_block;
	struct cpu_context	cpu_context;
};
#endif

/*
 * macros/functions for gaining access to the thread information structure
 *
 * preempt_count needs to be 1 initially, until the scheduler is functional.
 */
#ifndef __ASSEMBLY__
#define INIT_THREAD_INFO(tsk)				\
{							\
	.task		= &tsk,				\
	.exec_domain	= &default_exec_domain,		\
	.flags		= 0,				\
	.cpu		= 0,				\
	.preempt_count	= 1,				\
	.addr_limit	= KERNEL_DS,			\
	.restart_block  = {				\
			  .fn = do_no_restart_syscall,	\
	},						\
}

#define init_thread_info	(init_thread_union.thread_info)
#define init_stack		(init_thread_union.stack)

/* how to get the thread information struct from C */
#if 0 /* SJK FIXME: gcc chokes on this... */
register struct thread_info *current_thread_info_reg asm("$24");
#define current_thread_info()   (current_thread_info_reg)
#else
static inline struct thread_info *current_thread_info(void)
{
	struct thread_info *info;
	__asm__ __volatile__("ori %0, $24, 0" : "=r"(info));

	return info;
}
#endif

/* SJK: DEBUG functions, remove at some point */
static inline u32 get_r29(void)
{
	u32 r29;

	__asm__ __volatile__("ori %0, $29, 0" : "=r"(r29));

	return r29;
}

static inline u32 get_r28(void)
{
	u32 r28;

	__asm__ __volatile__("ori %0, $28, 0" : "=r"(r28));

	return r28;
}

#define get_thread_info(ti) get_task_struct((ti)->task)
#define put_thread_info(ti) put_task_struct((ti)->task)

#endif /* !__ASSEMBLY__ */
/* SJK: the flags blow are copied from OpenRISC, needs revising */
/*
 * thread information flags
 *   these are process state flags that various assembly files may need to
 *   access
 *   - pending work-to-be-done flags are in LSW
 *   - other flags in MSW
 */
#define TIF_SYSCALL_TRACE	0	/* syscall trace active */
#define TIF_NOTIFY_RESUME	1	/* resumption notification requested */
#define TIF_SIGPENDING		2	/* signal pending */
#define TIF_NEED_RESCHED	3	/* rescheduling necessary */
#define TIF_SINGLESTEP		4	/* restore singlestep on return to user
					 * mode
					 */
#define TIF_SYSCALL_TRACEPOINT  8       /* for ftrace syscall instrumentation */
#define TIF_RESTORE_SIGMASK     9
#define TIF_POLLING_NRFLAG	16	/* true if poll_idle() is polling						 * TIF_NEED_RESCHED
					 */
#define TIF_MEMDIE              17

#define _TIF_SYSCALL_TRACE	(1<<TIF_SYSCALL_TRACE)
#define _TIF_NOTIFY_RESUME	(1<<TIF_NOTIFY_RESUME)
#define _TIF_SIGPENDING		(1<<TIF_SIGPENDING)
#define _TIF_NEED_RESCHED	(1<<TIF_NEED_RESCHED)
#define _TIF_SINGLESTEP		(1<<TIF_SINGLESTEP)
#define _TIF_POLLING_NRFLAG	(1<<TIF_POLLING_NRFLAG)


/* Work to do when returning from interrupt/exception */
/* For ECO32, this is anything in the LSW other than syscall trace */
#define _TIF_WORK_MASK (0xff & ~(_TIF_SYSCALL_TRACE|_TIF_SINGLESTEP))

#endif /* __KERNEL__ */

#endif /* __ASM_ECO32_THREAD_INFO_H */
