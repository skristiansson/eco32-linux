/*
 * Copyright (C) 2014 Stefan Kristiansson, stefan.kristiansson@saunalahti.fi
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2.  This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */

#include <linux/init.h>
#include <asm/page.h>
#include <asm/asm-offsets.h>
#include <asm/thread_info.h>
#include <asm/spr.h>

	.org 0x0
_reset:
	j	_start

exception:
	j	exception

tlbmiss:
	subi	$29,$29,12
	stw	$4,$29,0
	stw	$5,$29,4
	stw	$6,$29,8

	ori	$4,$0,current_pgd
	ldw	$4,$4,0

	stw	$4,$29,0
	stw	$5,$29,4
	stw	$6,$29,8
	addi	$29,$29,12

	j	. /* SJK TODO */

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
	mvts	$0,SPR_TLB_ENTRY_HIGH
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
