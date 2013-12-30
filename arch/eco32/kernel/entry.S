/*
 * Copyright (C) 2014 Stefan Kristiansson, stefan.kristiansson@saunalahti.fi
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2.  This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */

#include <linux/linkage.h>
#include <asm/asm-offsets.h>

ENTRY(ret_from_kernel_thread)
	jal	schedule_tail
	ori	$4,$17,0
	jalr	$16	/* fn */
	j	. 	/* SJK TODO */


ENTRY(ret_from_fork)
	jal	schedule_tail
	j	.	/* SJK TODO */

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

ENTRY(sys_rt_sigreturn)
	j	.	/* SJK TODO */
