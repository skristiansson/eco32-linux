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
#include <asm/spr.h>
#include <asm/tlb.h>
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
	int i;

	mvts(SPR_TLB_ENTRY_HIGH, 0xc0000000);
	mvts(SPR_TLB_ENTRY_LOW, 0);
	for (i = 0; i < NUM_TLB_ENTRIES; i++) {
		mvts(SPR_TLB_INDEX, i);
		tbwi();
	}
}

static inline void flush_tlb_mm(struct mm_struct *mm)
{
	/* eco32 only have 32 tlbs, so let's just flush them all */
	flush_tlb_all();
}

static inline void flush_tlb_page(struct vm_area_struct *vma,
				  unsigned long addr)
{
	unsigned long tlb_index;

	/*
	 * The tbs instruction search the tlbs for the virtual address
	 * present in SPR_TLB_ENTRY_HIGH. If found, the tlb index is returned
	 * in SPR_TLB_INDEX otherwise SPR_TLB_INDEX contains 0x80000000.
	 */
	mvts(SPR_TLB_ENTRY_HIGH, addr);
	tbs();
	tlb_index = mvfs(SPR_TLB_INDEX);

	if (tlb_index != 0x80000000) {
		mvts(SPR_TLB_ENTRY_HIGH, 0xc0000000);
		mvts(SPR_TLB_ENTRY_LOW, 0);
		tbwi();
	}
}

static inline void flush_tlb_range(struct vm_area_struct *vma,
				   unsigned long start,
				   unsigned long end)
{
	int i;
	unsigned long addr;

	for (i = 0; i < NUM_TLB_ENTRIES; i++) {
		mvts(SPR_TLB_INDEX, i);
		addr = mvfs(SPR_TLB_ENTRY_HIGH);
		if (addr >= start && addr < end) {
			mvts(SPR_TLB_ENTRY_HIGH, 0xc0000000);
			mvts(SPR_TLB_ENTRY_LOW, 0);
			tbwi();
		}
	}
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
