/*
 * Copyright (C) 2014 Stefan Kristiansson, stefan.kristiansson@saunalahti.fi
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2.  This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */

#ifndef __ASM_ECO32_PGTABLE_H
#define __ASM_ECO32_PGTABLE_H

#include <asm-generic/pgtable-nopmd.h>

#define FIRST_USER_ADDRESS	0

/* On ECO32, we use a two-level page table setup */
#define PGDIR_SHIFT	(PAGE_SHIFT + (PAGE_SHIFT-2))
#define PGDIR_SIZE	(1UL << PGDIR_SHIFT)
#define PGDIR_MASK	(~(PGDIR_SIZE-1))

#ifdef __ASSEMBLY__
#define PTRS_PER_PTE	(1 << (PAGE_SHIFT-2))
#else
#define PTRS_PER_PTE	(1UL << (PAGE_SHIFT-2))
#endif
#define PTRS_PER_PGD	(1UL << (32 - PGDIR_SHIFT))

#ifndef __ASSEMBLY__
#include <asm/mmu.h>

extern void paging_init(void);

/*
 * Certain architectures need to do special things when pte's
 * within a page table are directly modified.  Thus, the following
 * hook is made available.
 */
#define set_pte(ptep, pteval) ((*(ptep)) = (pteval))
#define set_pte_at(mm, addr, ptep, pteval) set_pte(ptep, pteval)

#define set_pmd(pmdp, pmdval) (*(pmdp) = pmdval)

/*
 * Vmalloc area of 64 MB, placed right below the kernel space.
 */
#define VMALLOC_START   (PAGE_OFFSET-0x04000000)
#define VMALLOC_END     (PAGE_OFFSET)
#define VMALLOC_VMADDR(x) ((unsigned long)(x))

/*
 * The ECO32 MMU is extremely simple, it's always switched on and
 * features only one set of TLBs that are shared over instruction and
 * data accesses.
 * The TLBs can be mapped to hold virtual addresses in the range
 * 0x00000000 - 0xc0000000, the range 0xc0000000 - 0xffffffff is always
 * direct-mapped to the physical range 0x00000000 - 0x3fffffff.
 *
 * The TLB only features two flags, valid and write-enable so most flags
 * are emulated in software.
 * The layout of the hardware TLB looks like this:
 *
 * | 31:12 | 11:2     | 1  | 0     |
 * | PPN   | reserved | WE | VALID |
 */

/* Hardware bit-defines*/
#define _PAGE_VALID	(1 << 0)	/* Valid */
#define _PAGE_PRESENT	(1 << 0)	/* Present */
#define _PAGE_WRITE	(1 << 1)	/* Write enabled */

/* Software bit-defines */
#define _PAGE_FILE	(1 << 3)	/* 0=swap, 1=pagecache (!present) */
#define _PAGE_READ	(1 << 4)	/* Read enabled */
#define _PAGE_ACCESSED	(1 << 5)	/* Accessed */
#define _PAGE_EXEC	(1 << 6)	/* Executable */
#define _PAGE_DIRTY	(1 << 7)	/* Dirty */

#define _PAGE_CHG_MASK	(PAGE_MASK | _PAGE_ACCESSED | _PAGE_DIRTY)

#define PAGE_NONE	__pgprot(_PAGE_PRESENT | _PAGE_ACCESSED)
#define PAGE_KERNEL \
	__pgprot(_PAGE_PRESENT | _PAGE_ACCESSED | _PAGE_READ | _PAGE_WRITE)
#define PAGE_READONLY	__pgprot(_PAGE_PRESENT | _PAGE_ACCESSED)
#define PAGE_READONLY_X	__pgprot(_PAGE_PRESENT | _PAGE_ACCESSED | _PAGE_EXEC)
#define PAGE_COPY	__pgprot(_PAGE_PRESENT | _PAGE_ACCESSED)
#define PAGE_COPY_X	__pgprot(_PAGE_PRESENT | _PAGE_ACCESSED | _PAGE_EXEC)
#define PAGE_SHARED	__pgprot(_PAGE_PRESENT | _PAGE_ACCESSED | _PAGE_WRITE)
#define PAGE_SHARED_X \
	__pgprot(_PAGE_PRESENT | _PAGE_ACCESSED | _PAGE_WRITE | _PAGE_EXEC)

/*
 * ECO32 hardware have no support for execute protection.
 */
#define __P000	PAGE_NONE
#define __P001	PAGE_READONLY_X
#define __P010	PAGE_COPY
#define __P011	PAGE_COPY_X
#define __P100	PAGE_READONLY
#define __P101	PAGE_READONLY_X
#define __P110	PAGE_COPY
#define __P111	PAGE_COPY_X

#define __S000	PAGE_NONE
#define __S001	PAGE_READONLY_X
#define __S010	PAGE_SHARED
#define __S011	PAGE_SHARED_X
#define __S100	PAGE_READONLY
#define __S101	PAGE_READONLY_X
#define __S110	PAGE_SHARED
#define __S111	PAGE_SHARED_X

extern unsigned long empty_zero_page[];
#define ZERO_PAGE(vaddr)	(virt_to_page(empty_zero_page))

#define pte_ERROR(e) \
	pr_err("%s:%d: bad pte %p(%08lx).\n", \
	       __FILE__, __LINE__, &(e), pte_val(e))
#define pgd_ERROR(e) \
	pr_err("%s:%d: bad pgd %p(%08lx).\n", \
	       __FILE__, __LINE__, &(e), pgd_val(e))

/* Encode and de-code a swap entry */
/* SJK check those */
#define __swp_type(x)		(((x).val >> 3) & 0x1f)
#define __swp_offset(entry)	((entry).val >> 8)
#define __swp_entry(type, offset) \
	((swp_entry_t) { ((type) << 3) | ((offset) << 8) })
#define __pte_to_swp_entry(pte)	((swp_entry_t) { pte_val(pte) })
#define __swp_entry_to_pte(x)	__pte((x).val)

/* SJK check those */
#define PTE_FILE_MAX_BITS	28
#define pte_to_pgoff(x)		(pte_val(x) >> 4)
#define pgoff_to_pte(x)		__pte(((x) << 4) | _PAGE_FILE)

/* Find an entry in the third-level page table.. */
#define __pte_offset(addr)	(((addr) >> PAGE_SHIFT) & (PTRS_PER_PTE - 1))
#define pte_offset_kernel(dir, addr) \
	((pte_t *) pmd_page_kernel(*(dir)) + __pte_offset(addr))
#define pte_offset_map(dir, addr) \
	((pte_t *)page_address(pmd_page(*(dir))) + __pte_offset(addr))

static inline pte_t pte_modify(pte_t pte, pgprot_t newprot)
{
	pte_val(pte) = (pte_val(pte) & _PAGE_CHG_MASK) | pgprot_val(newprot);
	return pte;
}

#define mk_pte(page, pgprot)	pfn_pte(page_to_pfn(page), (pgprot))
#define pte_page(pte)		pfn_to_page(pte_pfn(pte))
#define pte_unmap(pte)		do { } while (0)
#define pte_unmap_nested(pte)	do { } while (0)
#define pte_pfn(x)		((unsigned long)(((x).pte)) >> PAGE_SHIFT)
#define pfn_pte(pfn, prot)	__pte((((pfn) << PAGE_SHIFT)) | \
				      pgprot_val(prot))
#define pte_none(pte)		(!pte_val(pte))
#define pte_present(pte)	(pte_val(pte) & _PAGE_PRESENT)
#define pte_clear(mm, addr, ptep)	set_pte(ptep, __pte(0))

#define pmd_none(pmd)		(!pmd_val(pmd))
#define pmd_bad(pmd)		(pmd_val(pmd) & (~PAGE_MASK))
#define pmd_present(pmd)	(pmd_val(pmd))
#define pmd_clear(pmdp)		do { pmd_val(*(pmdp)) = 0; } while (0)
#define pmd_page(pmd)		(pfn_to_page(__pa(pmd_val(pmd)) >> PAGE_SHIFT))
#define pmd_page_kernel(pmd)    ((unsigned long) (pmd_val(pmd) & PAGE_MASK))
#define pmd_pgtable(pmd)	pmd_page(pmd)

#define pgd_index(addr)		((addr) >> PGDIR_SHIFT)
#define pgd_offset(mm, addr)	((mm)->pgd + pgd_index(addr))
#define pgd_offset_k(addr)	pgd_offset(&init_mm, addr)

/*
 * The following only work if pte_present() is true.
 * Undefined behaviour if not..
 */
static inline int pte_read(pte_t pte)
{
	return pte_val(pte) & _PAGE_READ;
}

static inline int pte_write(pte_t pte)
{
	return pte_val(pte) & _PAGE_WRITE;
}

static inline int pte_exec(pte_t pte)
{
	return pte_val(pte) & _PAGE_EXEC;
}

static inline int pte_dirty(pte_t pte)
{
	return pte_val(pte) & _PAGE_DIRTY;
}

static inline int pte_young(pte_t pte)
{
	return pte_val(pte) & _PAGE_ACCESSED;
}

static inline int pte_file(pte_t pte)
{
	return pte_val(pte) & _PAGE_FILE;
}

static inline int pte_special(pte_t pte)
{
	return 0;
}

static inline pte_t pte_wrprotect(pte_t pte)
{
	pte_val(pte) &= ~(_PAGE_WRITE);
	return pte;
}

static inline pte_t pte_rdprotect(pte_t pte)
{
	pte_val(pte) &= ~(_PAGE_READ);
	return pte;
}

static inline pte_t pte_exprotect(pte_t pte)
{
	pte_val(pte) &= ~(_PAGE_EXEC);
	return pte;
}

static inline pte_t pte_mkclean(pte_t pte)
{
	pte_val(pte) &= ~(_PAGE_DIRTY);
	return pte;
}

static inline pte_t pte_mkold(pte_t pte)
{
	pte_val(pte) &= ~(_PAGE_ACCESSED);
	return pte;
}

static inline pte_t pte_mkwrite(pte_t pte)
{
	pte_val(pte) |= _PAGE_WRITE;
	return pte;
}

static inline pte_t pte_mkread(pte_t pte)
{
	pte_val(pte) |= _PAGE_READ;
	return pte;
}

static inline pte_t pte_mkexec(pte_t pte)
{
	pte_val(pte) |= _PAGE_EXEC;
	return pte;
}

static inline pte_t pte_mkdirty(pte_t pte)
{
	pte_val(pte) |= _PAGE_DIRTY;
	return pte;
}

static inline pte_t pte_mkyoung(pte_t pte)
{
	pte_val(pte) |= _PAGE_ACCESSED;
	return pte;
}

static inline pte_t pte_mkspecial(pte_t pte)
{
	return pte;
}

extern pgd_t swapper_pg_dir[PTRS_PER_PGD];

#include <asm-generic/pgtable.h>

/*
 * No page table caches to initialise
 */
#define pgtable_cache_init()	do { } while (0)

/*
 * ECO32 doesn't have any mmu caches
 */
#define update_mmu_cache(vma, addr, ptep)	do { } while (0)

#endif /* __ASSEMBLY__ */
#endif /* __ASM_ECO32_PGTABLE_H */
