/*
 * Copyright (C) 2014 Stefan Kristiansson, stefan.kristiansson@saunalahti.fi
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2.  This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */

#ifndef __ASM_ECO32_TLBFLUSH_H
#define __ASM_ECO32_TLBFLUSH_H

#include <linux/mm.h>
#include <asm/processor.h>
#include <asm/pgtable.h>
#include <asm/pgalloc.h>
#include <asm/current.h>
#include <linux/sched.h>

/*
 *  - flush_tlb() flushes the current mm struct TLBs
 *  - flush_tlb_all() flushes all processes TLBs
 *  - flush_tlb_mm(mm) flushes the specified mm context TLB's
 *  - flush_tlb_page(vma, vmaddr) flushes one page
 *  - flush_tlb_range(mm, start, end) flushes a range of pages
 */

static inline void flush_tlb_all(void)
{
	BUG(); /* SJK TODO */
}

static inline void flush_tlb_mm(struct mm_struct *mm)
{
	BUG(); /* SJK TODO */
}

static inline void flush_tlb_page(struct vm_area_struct *vma,
				  unsigned long addr)
{
	BUG(); /* SJK TODO */
}

static inline void flush_tlb_range(struct vm_area_struct *vma,
				   unsigned long start,
				   unsigned long end)
{
	BUG(); /* SJK TODO */
}

static inline void flush_tlb(void)
{
	flush_tlb_mm(current->mm);
}

static inline void flush_tlb_kernel_range(unsigned long start,
					  unsigned long end)
{
	flush_tlb_range(NULL, start, end);
}

#endif /* __ASM_ECO32_TLBFLUSH_H */
