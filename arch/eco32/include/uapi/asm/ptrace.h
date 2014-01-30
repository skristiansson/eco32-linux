/*
 * Copyright (C) 2014 Stefan Kristiansson, stefan.kristiansson@saunalahti.fi
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2.  This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */
#ifndef _UAPI_ASM_ECO32_PTRACE_H
#define _UAPI_ASM_ECO32_PTRACE_H

#ifndef __ASSEMBLY__

struct pt_regs {
	unsigned long r0;
	unsigned long r1;
	unsigned long r2;
	unsigned long r3;
	unsigned long r4;
	unsigned long r5;
	unsigned long r6;
	unsigned long r7;
	unsigned long r8;
	unsigned long r9;
	unsigned long r10;
	unsigned long r11;
	unsigned long r12;
	unsigned long r13;
	unsigned long r14;
	unsigned long r15;
	unsigned long r16;
	unsigned long r17;
	unsigned long r18;
	unsigned long r19;
	unsigned long r20;
	unsigned long r21;
	unsigned long r22;
	unsigned long r23;
	unsigned long r24;
	unsigned long r25;
	unsigned long r26;
	unsigned long r27;
	unsigned long r28;
	unsigned long r29;
	unsigned long pc; /* r30 */
	unsigned long r31;
	unsigned long psw; /* Processor Status Word */
	unsigned long orig_r2; /* for syscall restart */
};

#endif /* __ASSEMBLY__ */

#endif /* _UAPI_ASM_ECO32_PTRACE_H */
