/*
 * Copyright (C) 2014 Stefan Kristiansson, stefan.kristiansson@saunalahti.fi
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2.  This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */

#include <linux/linkage.h>
#include <asm/asm-offsets.h>
#include <asm/spr.h>

#define KERNEL_PSW	(SPR_PSW_V)

#define SWITCH_TO_KERNEL_STACK \
	mvfs	$3,SPR_PSW				;\
	slri	$3,$3,25	/* PSW_UP */		;\
	andi	$3,$3,1					;\
	beq	$3,$0,1f	/* check mode */	;\
	/* user mode */					;\
	ori	$3,$29,0	/* save old $29 to $3 */;\
	ori	$29,$24,TI_CPU_CONTEXT			;\
	ldw	$29,$29,CC_GPR29			;\
	j	2f					;\
1:	/* kernel mode */				;\
	ori	$3,$29,0				;\
2:							;\
	subi	$29,$29,PT_SIZE

/* SJK: all of those are not needed to be saved */
#define SAVE_REGS \
	stw	$3,$29,PT_GPR29				;\
	stw	$0,$29,PT_GPR0				;\
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
	mvfs	$3,SPR_PSW				;\
	stw	$3,$29,PT_PSW

#define RESTORE_REGS \
	ldw	$3,$29,PT_PSW				;\
	mvts	$3,SPR_PSW				;\
	ldw	$0,$29,PT_GPR0				;\
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

ENTRY(exception_handler)
	SWITCH_TO_KERNEL_STACK
	SAVE_REGS
	ori	$3,$0,KERNEL_PSW
	mvts	$3,SPR_PSW
	ori	$4,$29,0		/* pt_regs */
	jal	do_exception
	RESTORE_REGS
	rfx				/* return to where exception occured */

ENTRY(page_fault_handler)
	SWITCH_TO_KERNEL_STACK
	SAVE_REGS
	ori	$3,$0,KERNEL_PSW
	mvts	$3,SPR_PSW
	ori	$4,$29,0
	mvfs	$5,SPR_TLB_BAD_ADDRESS
	jal	do_page_fault
	RESTORE_REGS
	rfx				/* return to where exception occured */
