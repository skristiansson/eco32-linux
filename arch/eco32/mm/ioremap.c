/*
 * Copyright (C) 2014 Stefan Kristiansson, stefan.kristiansson@saunalahti.fi
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2.  This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */

#include <linux/vmalloc.h>
#include <linux/io.h>

/*
 * Remap an arbitrary physical address space into the kernel virtual
 * address space. Needed when the kernel wants to access high addresses
 * directly.
 *
 * NOTE! We need to allow non-page-aligned mappings too: we will obviously
 * have to convert them into an offset in a page-aligned mapping, but the
 * caller shouldn't need to know that small detail.
 */
void __iomem *__init_refok
__ioremap(phys_addr_t addr, unsigned long size, pgprot_t prot)
{
	BUG(); /* SJK TODO */
}

void iounmap(void *addr)
{
	BUG(); /* SJK TODO */
}
