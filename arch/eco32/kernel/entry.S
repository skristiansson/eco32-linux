/*
 * Copyright (C) 2014 Stefan Kristiansson, stefan.kristiansson@saunalahti.fi
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2.  This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */

#include <linux/linkage.h>
#include <asm/thread_info.h>
#include <asm/asm-offsets.h>
#include <asm/spr.h>

#define KERNEL_PSW	(SPR_PSW_V)

#define SET_PSW(x) \
	mvfs	$26,SPR_PSW 		;\
	/* mask IEN bits */		;\
	andi	$26,$26,0xff		;\
	ori	$26,$26,x		;\
	mvts	$26,SPR_PSW

#define SWITCH_TO_KERNEL_STACK \
	mvfs	$26,SPR_PSW				;\
	slri	$26,$26,25	/* PSW_UP */		;\
	andi	$26,$26,1				;\
	beq	$26,$0,1f	/* check mode */	;\
	/* user mode */					;\
	ori	$26,$29,0	/* save old $29 to $26 */;\
	addi	$29,$24,THREAD_SIZE /* current_pt_regs() */;\
	j	2f					;\
1:	/* kernel mode */				;\
	ori	$26,$29,0				;\
2:							;\
	subi	$29,$29,PT_SIZE

#define SAVE_REGS \
	stw	$26,$29,PT_GPR29 /* sp is in $26 */	;\
	stw	$1,$29,PT_GPR1				;\
	stw	$2,$29,PT_GPR2				;\
	stw	$3,$29,PT_GPR3				;\
	stw	$4,$29,PT_GPR4				;\
	stw	$5,$29,PT_GPR5				;\
	stw	$6,$29,PT_GPR6				;\
	stw	$7,$29,PT_GPR7				;\
	stw	$8,$29,PT_GPR8				;\
	stw	$9,$29,PT_GPR9				;\
	stw	$10,$29,PT_GPR10			;\
	stw	$11,$29,PT_GPR11			;\
	stw	$12,$29,PT_GPR12			;\
	stw	$13,$29,PT_GPR13			;\
	stw	$14,$29,PT_GPR14			;\
	stw	$15,$29,PT_GPR15			;\
	stw	$16,$29,PT_GPR16			;\
	stw	$17,$29,PT_GPR17			;\
	stw	$18,$29,PT_GPR18			;\
	stw	$19,$29,PT_GPR19			;\
	stw	$20,$29,PT_GPR20			;\
	stw	$21,$29,PT_GPR21			;\
	stw	$22,$29,PT_GPR22			;\
	stw	$23,$29,PT_GPR23			;\
	stw	$24,$29,PT_GPR24			;\
	stw	$25,$29,PT_GPR25			;\
	stw	$26,$29,PT_GPR26			;\
	stw	$27,$29,PT_GPR27			;\
	stw	$28,$29,PT_GPR28			;\
	stw	$30,$29,PT_PC				;\
	stw	$31,$29,PT_GPR31			;\
	addi	$26,$0,-1				;\
	stw	$26,$29,PT_ORIG_GPR2			;\
	mvfs	$26,SPR_PSW				;\
	stw	$26,$29,PT_PSW

#define RESTORE_REGS \
	ldw	$26,$29,PT_PSW				;\
	mvts	$26,SPR_PSW				;\
	ldw	$1,$29,PT_GPR1				;\
	ldw	$2,$29,PT_GPR2				;\
	ldw	$3,$29,PT_GPR3				;\
	ldw	$4,$29,PT_GPR4				;\
	ldw	$5,$29,PT_GPR5				;\
	ldw	$6,$29,PT_GPR6				;\
	ldw	$7,$29,PT_GPR7				;\
	ldw	$8,$29,PT_GPR8				;\
	ldw	$9,$29,PT_GPR9				;\
	ldw	$10,$29,PT_GPR10			;\
	ldw	$11,$29,PT_GPR11			;\
	ldw	$12,$29,PT_GPR12			;\
	ldw	$13,$29,PT_GPR13			;\
	ldw	$14,$29,PT_GPR14			;\
	ldw	$15,$29,PT_GPR15			;\
	ldw	$16,$29,PT_GPR16			;\
	ldw	$17,$29,PT_GPR17			;\
	ldw	$18,$29,PT_GPR18			;\
	ldw	$19,$29,PT_GPR19			;\
	ldw	$20,$29,PT_GPR20			;\
	ldw	$21,$29,PT_GPR21			;\
	ldw	$22,$29,PT_GPR22			;\
	ldw	$23,$29,PT_GPR23			;\
	ldw	$24,$29,PT_GPR24			;\
	ldw	$25,$29,PT_GPR25			;\
	ldw	$26,$29,PT_GPR26			;\
	ldw	$27,$29,PT_GPR27			;\
	ldw	$28,$29,PT_GPR28			;\
	ldw	$30,$29,PT_PC				;\
	ldw	$31,$29,PT_GPR31			;\
	ldw	$29,$29,PT_GPR29

#define CHECK_WORK \
	/* loop until _TIF_WORK_MASK is unset */			;\
1:									;\
	SET_PSW(KERNEL_PSW)		/* disable interrupts */	;\
	ldw	$5,$24,TI_FLAGS						;\
	andi	$4,$5,_TIF_WORK_MASK					;\
	beq	$4,$0,2f						;\
	ori	$4,$29,0		/* pt_regs */			;\
	jal	do_work_pending						;\
	j	1b							;\
2:

ENTRY(ret_from_kernel_thread)
	jal	schedule_tail
	ori	$4,$17,0
	jalr	$16	/* fn */
	j	ret_from_syscall

ENTRY(ret_from_fork)
	jal	schedule_tail
	ldw	$2,$29,PT_GPR2		/* load return value */
	j	ret_from_syscall

ENTRY(__switch_to)
	ori	$8,$4,TI_CPU_CONTEXT	/* prev_ti->cpu_context */

	/* save stack-pointer */
	stw	$29,$8,CC_GPR29
	/* save frame-pointer */
	stw	$28,$8,CC_GPR28
	/* save return address */
	stw	$31,$8,CC_GPR31

	/* save callee-saved registers */
	stw	$16,$8,CC_GPR16
	stw	$17,$8,CC_GPR17
	stw	$18,$8,CC_GPR18
	stw	$19,$8,CC_GPR19
	stw	$20,$8,CC_GPR20
	stw	$21,$8,CC_GPR21
	stw	$22,$8,CC_GPR22
	stw	$23,$8,CC_GPR23

	/* return prev current_thread_info()->task */
	ldw	$2,$24,TI_TASK

	ori	$24,$5,0		/* current_thread_info = next_ti */
	ori	$8,$24,TI_CPU_CONTEXT	/* $8 = next_ti->cpu_context */

	/* restore callee-saved registers */
	ldw	$16,$8,CC_GPR16
	ldw	$17,$8,CC_GPR17
	ldw	$18,$8,CC_GPR18
	ldw	$19,$8,CC_GPR19
	ldw	$20,$8,CC_GPR20
	ldw	$21,$8,CC_GPR21
	ldw	$22,$8,CC_GPR22
	ldw	$23,$8,CC_GPR23

	/* restore stack-pointer */
	ldw	$29,$8,CC_GPR29
	/* restore frame-pointer */
	ldw	$28,$8,CC_GPR28
	/* restore return address */
	ldw	$31,$8,CC_GPR31

	ldw	$4,$4,TI_TASK		/* load argument for schedule_tail */
	jr	$31

/*
 * The sigreturn is a special case syscall, which get some special
 * treatment. We arrive here from syscall_handler by a jalr, but we do not
 * return to where we were called from, instead we take a different path where
 * the full pt_regs context is restored.
 */
ENTRY(sys_rt_sigreturn)
	/*
	 * As we are taking a different path from the normal syscalls,
	 * restore the stack pointer here.
	 */
	addi	$29,$29,8+16
	ori	$4,$29,0
	jal	do_sys_rt_sigreturn
	/* Restore context and return. */
	j	ret_from_exception

/*
 * Main entry point for all exceptions except user tlb misses.
 * We arrive here straight with a jump from the exception vector.
 */
ENTRY(exception_handler)
	/* syscalls */
	mvfs	$26,SPR_PSW
	slri	$26,$26,SPR_PSW_EID_BIT
	andi	$26,$26,0x1f
	subi	$26,$26,20		/* EID = 20: trap */
	beq	$26,$0,syscall_handler
	/* kernel tlb misses are handled as user tlb misses */
	subi	$26,$26,1		/* EID = 21: kernel tlb miss */
	beq	$26,$0,tlbmiss
	/* fall-through */

ENTRY(exception_dispatch)
	SWITCH_TO_KERNEL_STACK
	SAVE_REGS
	SET_PSW(KERNEL_PSW)

	ori	$4,$29,0		/* pt_regs */
	jal	do_exception

ret_from_exception:
	CHECK_WORK
	RESTORE_REGS
	rfx				/* return to where exception occured */

/*
 * Syscall ABI:
 * syscall number in $2
 * args in $4-$9
 */
syscall_handler:
	SWITCH_TO_KERNEL_STACK
	/*
	 * FIXME: it's not necessary to save all regs here, this should be
	 * optimized. More precisely, only the regs we clobber in the syscall
	 * handler needs to be saved. However, if there are work pending, and
	 * most importantly work that involves signal handling, a fuller set
	 * of register needs to be saved (for the switch to userspace
	 * signal handler). Saving of those registers should be moved out
	 * of the fast-path syscall handling together with the CHECKWORK macro.
	 */
	SAVE_REGS
	/* Save the syscall number into regs->orig_r2 */
	stw	$2,$29,PT_ORIG_GPR2
/* SJK DEBUG */
	ori	$4,$29,0
	jal	syscall_debug
	ldw	$2,$29,PT_GPR2
	ldw	$4,$29,PT_GPR4
	ldw	$5,$29,PT_GPR5
	ldw	$6,$29,PT_GPR6
	ldw	$7,$29,PT_GPR7
	ldw	$8,$29,PT_GPR8
	ldw	$9,$29,PT_GPR9
/* SJK DEBUG END */
	addi	$30,$30,4	/* skip over the trap instruction */
	stw	$30,$29,PT_PC	/* TODO: try to avoid this store */

	slli	$2,$2,2
	ldw	$2,$2,sys_call_table
	/* enable interrupts */
	SET_PSW(KERNEL_PSW | SPR_PSW_IC)

	/*
	 * The usage of $8 and $9 as argument registers are only part of
	 * the syscall ABI and have to be put on the stack before doing the
	 * syscall.
	 */
	subi	$29,$29,8+16
	stw	$8,$29,16
	stw	$9,$29,20
	jalr	$2
	addi	$29,$29,8+16
	stw	$2,$29,PT_GPR2
	/* fall-through */

ret_from_syscall:
	CHECK_WORK
	ldw	$26,$29,PT_PSW
	ldw	$2,$29,PT_GPR2
	ldw	$30,$29,PT_PC
	ldw	$31,$29,PT_GPR31
	ldw	$29,$29,PT_GPR29
	mvts	$26,SPR_PSW
	rfx
