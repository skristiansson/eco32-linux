/*
 * Copyright (C) 2013 Stefan Kristiansson, stefan.kristiansson@saunalahti.fi
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2.  This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */

#include <linux/mm.h>
#include <linux/bootmem.h>
#include <linux/sched.h>
#include <asm/mmu_context.h>

void __init paging_init(void)
{

	unsigned long zones_size[MAX_NR_ZONES];
	int i;

	/* Clear out the init_mm.pgd that will contain the kernel's mappings. */
	for(i = 0; i < PTRS_PER_PGD; i++)
		swapper_pg_dir[i] = __pgd(0);

	current_pgd = init_mm.pgd;

	memset(zones_size, 0, sizeof(zones_size));
	zones_size[ZONE_NORMAL] = max_low_pfn;

	free_area_init(zones_size);
}

/* References to section boundaries */
void __init mem_init(void)
{
	free_all_bootmem();
	pr_info("%s: done!\n", __func__);
}

#ifdef CONFIG_BLK_DEV_INITRD
void free_initrd_mem(unsigned long start, unsigned long end)
{
	free_reserved_area((void *)start, (void *)end, -1, "initrd");
}
#endif

void free_initmem(void)
{
	free_initmem_default(-1);
}
