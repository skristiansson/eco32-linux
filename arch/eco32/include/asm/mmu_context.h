/*
 * Copyright (C) 2014 Stefan Kristiansson, stefan.kristiansson@saunalahti.fi
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2.  This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */

#ifndef __ASM_ECO32_MMU_CONTEXT_H
#define __ASM_ECO32_MMU_CONTEXT_H

#include <asm-generic/mm_hooks.h>
#include <asm/tlbflush.h>

/*
 * current active pgd - this is similar to other processors pgd
 * registers like cr3 on the i386
 */
extern volatile pgd_t *current_pgd;   /* defined in arch/eco32/mm/fault.c */

static inline int init_new_context(struct task_struct *tsk,
				   struct mm_struct *mm)
{
	mm->context = 0;

	return 0;
}

static inline  void switch_mm(struct mm_struct *prev, struct mm_struct *next,
			      struct task_struct *tsk)
{
	current_pgd = next->pgd;
	if (prev != next)
		flush_tlb_mm(prev);
}

#define deactivate_mm(tsk, mm)	do { } while (0)

#define activate_mm(prev, next) switch_mm((prev), (next), NULL)

static inline void enter_lazy_tlb(struct mm_struct *mm, struct task_struct *tsk)
{
}

static inline void destroy_context(struct mm_struct *mm)
{
	flush_tlb_mm(mm);
}

#endif
