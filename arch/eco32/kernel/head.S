/*
 * Copyright (C) 2014 Stefan Kristiansson, stefan.kristiansson@saunalahti.fi
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2.  This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */

#include <linux/init.h>
#include <linux/linkage.h>
#include <asm/page.h>
#include <asm/pgtable.h>
#include <asm/asm-offsets.h>
#include <asm/thread_info.h>
#include <asm/spr.h>

	.org 0x0
_reset:
	j	_start

exception:
	j	exception_handler

ENTRY(tlbmiss)
	/*
	 * Before we can use $1, it has to be saved away, since it might be
	 * holding the address at the offending instruction.
	 * $1 can't be stored directly, since gas might insert a ldhi;ori
	 * sequence on exception_saved_gpr1.
	 * And we have to save $1, since gas will use it when we load
	 * current_pgd later.
	 * The register $26 is reserved for OS use, so it can be used as a
	 * scratch register without saving.
	 */
	ori	$26,$1,0
	stw	$26,$0,exception_saved_gpr1

	/* load current_pgd, we use the asm temp reg ($1) as storage */
	ori	$1,$0,current_pgd
	ldw	$1,$1,0

	mvfs	$26,SPR_TLB_ENTRY_HIGH	/* virtual address */
	slri	$26,$26,PGDIR_SHIFT
	slli	$26,$26,2		/* word address */
	/* add offset to pgd */
	add	$1,$1,$26
	ldw	$1,$1,0
	beq	$1,$0,_fault

	addi	$26,$0,PAGE_MASK
	and	$1,$1,$26
	mvfs	$26,SPR_TLB_ENTRY_HIGH
	slri	$26,$26,PAGE_SHIFT
	andi	$26,$26,PTRS_PER_PTE-1	/* 1024-1 = 0x3ff */
	slli	$26,$26,2		/* word address */
	add	$1,$1,$26
	ldw	$1,$1,0			/* pte */
	andi	$26,$1,1		/* check present bit */
	beq	$26,$0,_fault
	mvts	$1,SPR_TLB_ENTRY_LOW
	tbwr				/* write out to random tlb */

	/* restore $1 */
	ldw	$26,$0,exception_saved_gpr1
	ori	$1,$26,0

	rfx

_fault:
	/* restore $1 */
	ldw	$26,$0,exception_saved_gpr1
	ori	$1,$26,0
	/* let the exception dispatcher call do_page_fault() */
	j	exception_dispatch

exception_saved_gpr1:
	.space 4

	__HEAD
	.global _start
_start:
	/* Set vectors to be located at top of RAM */
	ori	$8,$0,SPR_PSW_V
	mvts	$8,SPR_PSW

	/* Setup kernel stack */
	ori	$29,$0,init_thread_union + THREAD_SIZE
	/* Setup current thread pointer */
	ori	$24,$0,init_thread_union

	/* Flush tlbs */
	ori	$8,$0,0xc0000000
	mvts	$8,SPR_TLB_ENTRY_HIGH
	mvts	$0,SPR_TLB_ENTRY_LOW
	ori	$8,$0,32
1:
	subi	$8,$8,1
	mvts	$8,SPR_TLB_INDEX
	tbwi	/* Write out the tlb entry */
	bne	$8,$0,1b

	/* Clear bss */
	addi	$8,$0,__bss_start
	addi	$9,$0,_end
1:
	stw	$0,$8,0
	addi	$8,$8,4
	bltu	$8,$9,1b

	/* early setup - init device tree init */
	ori	$4,$0,0 /* TODO: check for fdt pointer passed in $4 */
	jal	early_init_devtree

	j	start_kernel

	.section .data,"aw"
	.align	PAGE_SHIFT
	.global  empty_zero_page
empty_zero_page:
	.space  PAGE_SIZE

	.global  swapper_pg_dir
swapper_pg_dir:
	.space  PAGE_SIZE
