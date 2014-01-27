/*
 * Copyright (C) 2014 Stefan Kristiansson, stefan.kristiansson@saunalahti.fi
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2.  This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */

#include <linux/sched.h>
#include <linux/seq_file.h>
#include <linux/bootmem.h>
#include <linux/memblock.h>
#include <linux/of_platform.h>
#include <linux/of_fdt.h>
#include <asm/sections.h>
#include <asm/setup.h>

char __initdata cmd_line[COMMAND_LINE_SIZE] = CONFIG_CMDLINE;

void __init setup_cpuinfo(void)
{
	/* SJK TODO */
}

static int __init eco32_device_probe(void)
{
	of_platform_populate(NULL, NULL, NULL, NULL);

	return 0;
}
device_initcall(eco32_device_probe);

static void __init setup_memory(void)
{
	unsigned long bootmap_size;
	phys_addr_t memory_start, memory_end;
	struct memblock_region *region;

	memory_end = memory_start = 0;

	for_each_memblock(memory, region) {
		memory_start = region->base;
		memory_end = region->base + region->size;
		pr_info("%s: Main mem: 0x%08x-0x%08x\n", __func__, memory_start,
			memory_end);
	}

	if (!memory_end)
		panic("%s: Missing memory setting\n", __func__);

	max_pfn = PFN_DOWN(memblock_end_of_DRAM());
	min_low_pfn = PFN_UP(__pa(&_end));
	max_low_pfn = max_pfn - PFN_UP(memory_start);

	bootmap_size = init_bootmem(min_low_pfn, max_low_pfn);
	free_bootmem(PFN_PHYS(min_low_pfn),
		     (max_pfn - min_low_pfn) << PAGE_SHIFT);
	reserve_bootmem(PFN_PHYS(min_low_pfn), bootmap_size, BOOTMEM_DEFAULT);

	for_each_memblock(reserved, region) {
		pr_info("%s: Reserved: 0x%08x-0x%08x\n", __func__,
			(u32) region->base, (u32) (region->base+region->size));
		reserve_bootmem(region->base, region->size, BOOTMEM_DEFAULT);
	}
}

void __init setup_arch(char **cmdline_p)
{
#ifdef CONFIG_EARLY_PRINTK
        setup_early_printk();
#endif
	unflatten_device_tree();

	setup_cpuinfo();

	init_mm.start_code = (unsigned long)_stext;
	init_mm.end_code = (unsigned long)_etext;
	init_mm.end_data = (unsigned long)_edata;
	init_mm.brk = (unsigned long)_end;

	setup_memory();

	paging_init();

	*cmdline_p = cmd_line;
	/* SJK TODO */
}

void __init time_init(void)
{
	/* SJK TODO */
}

void calibrate_delay(void)
{
	/* Why do we need this function?
	 * A hevy-weight version exists in
	 * http://lxr.free-electrons.com/source/init/calibrate.c#L34
	 * but there it is indicated that calibrate_delay_direct would be used
	 * if ARCH_HAS_READ_CURRENT_TIMER is defined (which we do define).
	 */
	/* SJK TODO */
}

static int show_cpuinfo(struct seq_file *m, void *v)
{
	return seq_printf(m,
			  "cpu\t\t: eco32\n");
}

static void *c_start(struct seq_file *m, loff_t * pos)
{
	/* We only have one CPU... */
	return *pos < 1 ? (void *)1 : NULL;
}

static void *c_next(struct seq_file *m, void *v, loff_t * pos)
{
	++*pos;
	return NULL;
}

static void c_stop(struct seq_file *m, void *v)
{
}

const struct seq_operations cpuinfo_op = {
	.start = c_start,
	.next = c_next,
	.stop = c_stop,
	.show = show_cpuinfo,
};
