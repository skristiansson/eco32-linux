/*
 * Copyright (C) 2014 Stefan Kristiansson, stefan.kristiansson@saunalahti.fi
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2.  This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
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
	BUG(); /* SJK TODO */

	return 0;
}

static int
page_clear_nocache(pte_t *pte, unsigned long addr,
		   unsigned long next, struct mm_walk *walk)
{
	BUG(); /* SJK TODO */

	return 0;
}

static void *
eco32_dma_alloc(struct device *dev, size_t size,
		dma_addr_t *dma_handle, gfp_t gfp,
		struct dma_attrs *attrs)
{
	BUG(); /* SJK TODO */

	return 0;
}

static void
eco32_dma_free(struct device *dev, size_t size, void *vaddr,
	       dma_addr_t dma_handle, struct dma_attrs *attrs)
{
	BUG(); /* SJK TODO */

	return 0;
}

static dma_addr_t
eco32_map_page(struct device *dev, struct page *page,
	       unsigned long offset, size_t size,
	       enum dma_data_direction dir,
	       struct dma_attrs *attrs)
{
	BUG(); /* SJK TODO */

	return 0;
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
	BUG(); /* SJK DEBUG */

	return 0;
}

static void
eco32_unmap_sg(struct device *dev, struct scatterlist *sg,
	      int nents, enum dma_data_direction dir,
	      struct dma_attrs *attrs)
{
	BUG(); /* SJK DEBUG */
}

static void
eco32_sync_single_for_cpu(struct device *dev,
			  dma_addr_t dma_handle, size_t size,
			  enum dma_data_direction dir)
{
	BUG(); /* SJK TODO */
}

static void
eco32_sync_single_for_device(struct device *dev,
			     dma_addr_t dma_handle, size_t size,
			     enum dma_data_direction dir)
{
	BUG(); /* SJK TODO */
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
