/*
 * Copyright (C) 2014 Stefan Kristiansson, stefan.kristiansson@saunalahti.fi
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2.  This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */

#ifndef __ASM_ECO32_SPR_H
#define __ASM_ECO32_SPR_H

/* Special Purpose Registers */
#define	SPR_PSW			0
#define SPR_TLB_INDEX		1
#define SPR_TLB_ENTRY_HIGH	2
#define SPR_TLB_ENTRY_LOW	3
#define SPR_TLB_BAD_ADDRESS	4

/* Processor Status Word */
#define SPR_PSW_V		(1<<27) /* Exception Service Routine Vector */
#define SPR_PSW_UC		(1<<26) /* Current privilege mode */
#define SPR_PSW_UP		(1<<25) /* Previous privilege mode */
#define SPR_PSW_UO		(1<<24) /* Old privilege mode */
#define SPR_PSW_IC		(1<<23) /* Current global interrupt enable */
#define SPR_PSW_IP		(1<<22) /* Previous global interrupt enable */
#define SPR_PSW_IO		(1<<21) /* Old global interrupt enable */
#define SPR_PSW_EID_BIT		16
#define SPR_PSW_EID(x)		(((x) & 0x1f) << 16) /* Exception identifier */
#define SPR_PSW_IEN(x)		((x) & 0xffff) /* Interrupt Enable */

/* EID defines */
/* 0..15 Device Interrupts */
#define EID_BUS_TIMEOUT		16
#define EID_ILLEGAL_INSN	17
#define EID_PRIVILEGED_INSN	18
#define EID_DIV_BY_ZERO		19
#define EID_TRAP		20
#define EID_TLB_MISS		21
#define EID_TLB_WRITE		22
#define EID_TLB_INVALID		23
#define EID_ILLEGAL_ADDR	24
#define EID_PRIVILEGED_ADDR	25
/* 26..31 Reserved */

#ifndef __ASSEMBLY__
static inline void mvts(const unsigned long addr, unsigned long val)
{
	__asm__ __volatile__("mvts %0, %1" : : "r"(val), "i"(addr));
}

static inline unsigned long mvfs(const unsigned long addr)
{
	unsigned long val;

	__asm__ __volatile__("mvfs %0, %1" : "=r"(val) : "i"(addr));

	return val;
}
#endif

#endif /* __ASM_ECO32_SPR_H */
