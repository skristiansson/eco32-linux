/*
 * Copyright (C) 2014 Stefan Kristiansson, stefan.kristiansson@saunalahti.fi
 *
 * Based on the OpenRISC implementation:
 * Copyright (C) 2003 Matjaz Breskvar <phoenix@bsemi.com>
 * Copyright (C) 2010-2011 Jonas Bonn <jonas@southpole.se>
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2.  This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */

/*
 * NOTE: this code is completely untested on eco32, at the time of writing this,
 * there are no devices using DMA and there is no cache architecture defined for
 * eco32 yet.
 */

#include <linux/dma-mapping.h>
#include <linux/dma-debug.h>
#include <linux/export.h>
#include <linux/dma-attrs.h>

#include <asm/tlbflush.h>

static int
page_set_nocache(pte_t *pte, unsigned long addr,
		 unsigned long next, struct mm_walk *walk)
{
	pte_val(*pte) |= _PAGE_NONCACHED;

	/*
	 * Flush the page out of the TLB so that the new page flags get
	 * picked up next time there's an access
	 */
	flush_tlb_page(NULL, addr);

	return 0;
}

static int
page_clear_nocache(pte_t *pte, unsigned long addr,
		   unsigned long next, struct mm_walk *walk)
{
	pte_val(*pte) &= ~_PAGE_NONCACHED;

	/*
	 * Flush the page out of the TLB so that the new page flags get
	 * picked up next time there's an access
	 */
	flush_tlb_page(NULL, addr);

	return 0;
}

static void *
eco32_dma_alloc(struct device *dev, size_t size,
		dma_addr_t *dma_handle, gfp_t gfp,
		struct dma_attrs *attrs)
{
	unsigned long va;
	void *page;
	struct mm_walk walk = {
		.pte_entry = page_set_nocache,
		.mm = &init_mm
	};

	page = alloc_pages_exact(size, gfp);
	if (!page)
		return NULL;

	/* This gives us the real physical address of the first page. */
	*dma_handle = __pa(page);

	va = (unsigned long)page;

	if (!dma_get_attr(DMA_ATTR_NON_CONSISTENT, attrs)) {
		/*
		 * We need to iterate through the pages, clearing the dcache for
		 * them and setting the cache-inhibit bit.
		 */
		if (walk_page_range(va, va + size, &walk)) {
			free_pages_exact(page, size);
			return NULL;
		}
	}

	return (void *)va;
}

static void
eco32_dma_free(struct device *dev, size_t size, void *vaddr,
	       dma_addr_t dma_handle, struct dma_attrs *attrs)
{
	unsigned long va = (unsigned long)vaddr;
	struct mm_walk walk = {
		.pte_entry = page_clear_nocache,
		.mm = &init_mm
	};

	if (!dma_get_attr(DMA_ATTR_NON_CONSISTENT, attrs)) {
		/* walk_page_range shouldn't be able to fail here */
		WARN_ON(walk_page_range(va, va + size, &walk));
	}

	free_pages_exact(vaddr, size);
}

static void flush_dcache(unsigned start, size_t size)
{
	/* TODO */
}

static void invalidate_dcache(unsigned start, size_t size)
{
	/* TODO */
}

static dma_addr_t
eco32_map_page(struct device *dev, struct page *page,
	       unsigned long offset, size_t size,
	       enum dma_data_direction dir,
	       struct dma_attrs *attrs)
{
	unsigned long cl;
	dma_addr_t addr = page_to_phys(page) + offset;

	switch (dir) {
	case DMA_TO_DEVICE:
		flush_dcache(addr, size);
		break;
	case DMA_FROM_DEVICE:
		invalidate_dcache(addr, size);
		break;
	default:
		/*
		 * NOTE: If dir == DMA_BIDIRECTIONAL then there's no need to
		 * flush nor invalidate the cache here as the area will need
		 * to be manually synced anyway.
		 */
		break;
	}

	return addr;
}

static void
eco32_unmap_page(struct device *dev, dma_addr_t dma_handle,
		 size_t size, enum dma_data_direction dir,
		 struct dma_attrs *attrs)
{
}

static int
eco32_map_sg(struct device *dev, struct scatterlist *sg,
	     int nents, enum dma_data_direction dir,
	     struct dma_attrs *attrs)
{
	struct scatterlist *s;
	int i;

	for_each_sg(sg, s, nents, i) {
		s->dma_address = eco32_map_page(dev, sg_page(s), s->offset,
						s->length, dir, NULL);
	}

	return nents;
}

static void
eco32_unmap_sg(struct device *dev, struct scatterlist *sg,
	      int nents, enum dma_data_direction dir,
	      struct dma_attrs *attrs)
{
	struct scatterlist *s;
	int i;

	for_each_sg(sg, s, nents, i) {
		eco32_unmap_page(dev, sg_dma_address(s), sg_dma_len(s), dir, NULL);
	}
}

static void
eco32_sync_single_for_cpu(struct device *dev,
			  dma_addr_t dma_handle, size_t size,
			  enum dma_data_direction dir)
{
	invalidate_dcache(dma_handle, size);
}

static void
eco32_sync_single_for_device(struct device *dev,
			     dma_addr_t dma_handle, size_t size,
			     enum dma_data_direction dir)
{
	flush_dcache(dma_handle, size);
}

struct dma_map_ops eco32_dma_map_ops = {
	.alloc = eco32_dma_alloc,
	.free = eco32_dma_free,
	.map_page = eco32_map_page,
	.unmap_page = eco32_unmap_page,
	.map_sg = eco32_map_sg,
	.unmap_sg = eco32_unmap_sg,
	.sync_single_for_cpu = eco32_sync_single_for_cpu,
	.sync_single_for_device = eco32_sync_single_for_device,
};
EXPORT_SYMBOL(eco32_dma_map_ops);

/* Number of entries preallocated for DMA-API debugging */
#define PREALLOC_DMA_DEBUG_ENTRIES (1 << 16)

static int __init dma_init(void)
{
	dma_debug_init(PREALLOC_DMA_DEBUG_ENTRIES);

	return 0;
}
fs_initcall(dma_init);
