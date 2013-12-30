/*
 * Copyright (C) 2014 Stefan Kristiansson, stefan.kristiansson@saunalahti.fi
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2.  This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */

#ifndef ___ASM_ECO32_IRQFLAGS_H
#define ___ASM_ECO32_IRQFLAGS_H
#include <asm/spr.h>

#define ARCH_IRQ_DISABLED        0
#define ARCH_IRQ_ENABLED         SPR_PSW_IC

static inline unsigned long arch_local_save_flags(void)
{
	return mvfs(SPR_PSW) & SPR_PSW_IC;
}

static inline void arch_local_irq_restore(unsigned long flags)
{
	mvts(SPR_PSW, (mvfs(SPR_PSW) & ~SPR_PSW_IC) | flags);
}

#include <asm-generic/irqflags.h>

#endif /* ___ASM_ECO32_IRQFLAGS_H */
