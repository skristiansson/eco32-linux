/*
 * Copyright (C) 2014 Stefan Kristiansson, stefan.kristiansson@saunalahti.fi
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2.  This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */

#include <linux/init.h>
#include <linux/types.h>
#include <linux/memblock.h>
#include <linux/of_fdt.h>
#include <asm/page.h>

/* Note: https://lkml.org/lkml/2013/11/12/518 */
extern struct boot_param_header __dtb_start;

void __init early_init_dt_add_memory_arch(u64 base, u64 size)
{
	size &= PAGE_MASK;
	memblock_add(base, size);
}

void __init early_init_devtree(void *params)
{

	/* Note: https://lkml.org/lkml/2013/11/19/157 */
	if (!params)
		params = &__dtb_start;
	pr_info("FDT at %p\n", params);

	early_init_dt_scan(params);
	memblock_allow_resize();
}

#ifdef CONFIG_BLK_DEV_INITRD
void __init early_init_dt_setup_initrd_arch(u64 start, u64 end)
{
	BUG();
	/* SJK TODO */
}
#endif
