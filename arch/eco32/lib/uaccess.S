/*
 * Copyright (C) 2014 Stefan Kristiansson, stefan.kristiansson@saunalahti.fi
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2.  This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */

#include <linux/linkage.h>
#include <asm/errno.h>

/*
 * int __copy_tofrom_user(void *to, const void *from, unsigned long size);
 *
 * Return: remaining bytes
 */
	.global	__copy_tofrom_user
__copy_tofrom_user:
	subi	$29,$29,8
	stw	$4,$29,0
	stw	$5,$29,4

	addi	$2,$6,0
2:  	beq  	$2,$0,1f
	subi	$2,$2,1
8:    	ldbu	$8,$5,0
9:    	stb	$8,$4,0
	addi	$4,$4,1
	addi	$5,$5,1
	j	2b
1:
	ldw	$4,$29,0
	ldw	$5,$29,4
	addi	$29,$29,8
	jr	$31

	.section .fixup, "ax"
99:
	j	1b
	.previous

	.section __ex_table, "a"
	.long 8b, 99b		/* read fault */
	.long 9b, 99b		/* write fault */
	.previous

/*
 * unsigned long clear_user(void *addr, unsigned long size) ;
 *
 * Return: remaining bytes
 */
	.global	__clear_user
__clear_user:
	subi	$29,$29,4
	stw	$4,$29,0

	addi	$2,$5,0

2:	beq	$2,$0,1f
	subi	$2,$2,1
9:	stb	$0,$4,0
	addi	$4,$4,1
	j	2b

1:	ldw	$4,$29,0
	addi	$29,$29,4
	jr	$31

	.section .fixup, "ax"
99:
	j	1b
	.previous

	.section __ex_table, "a"
		.long 9b, 99b		/* write fault */
	.previous
