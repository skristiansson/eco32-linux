/*
 * Copyright (C) 2014 Stefan Kristiansson, stefan.kristiansson@saunalahti.fi
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2.  This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */

#ifndef __ASM_ECO32_PGALLOC_H
#define __ASM_ECO32_PGALLOC_H

#include <linux/mm.h>

static inline void pgd_free(struct mm_struct *mm, pgd_t *pgd)
{
	free_page((unsigned long)pgd);
}

static inline pgd_t *pgd_alloc(struct mm_struct *mm)
{
	return (pgd_t *)get_zeroed_page(GFP_KERNEL);
}

static inline struct page *pte_alloc_one(struct mm_struct *mm,
					 unsigned long address)
{
	struct page *pte;
	pte = alloc_pages(GFP_KERNEL|__GFP_REPEAT, 0);
	if (pte)
		clear_page(page_address(pte));
	return pte;
}

static inline pte_t *pte_alloc_one_kernel(struct mm_struct *mm,
					  unsigned long address)
{
	pte_t *pte = (pte_t *)__get_free_page(GFP_KERNEL| __GFP_REPEAT |
					      __GFP_ZERO);
        return pte;
}

static inline void pte_free_kernel(struct mm_struct *mm, pte_t *pte)
{
	free_page((unsigned long)pte);
}

static inline void pte_free(struct mm_struct *mm, struct page *ptepage)
{
	__free_page(ptepage);
}

#define __pte_free_tlb(tlb, pte, addr)	tlb_remove_page((tlb), (pte))

#define pmd_populate(mm, pmd, pte) \
	(pmd_val(*(pmd)) = (unsigned long)page_address(pte))
#define pmd_populate_kernel(mm, pmd, pte) \
	(pmd_val(*(pmd)) = (unsigned long) (pte))

#define check_pgt_cache()	do { } while (0)

#endif /* __ASM_ECO32_PGALLOC_H */
