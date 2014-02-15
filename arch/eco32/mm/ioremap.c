/*
 * Copyright (C) 2014 Stefan Kristiansson, stefan.kristiansson@saunalahti.fi
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2.  This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */

#include <linux/vmalloc.h>
#include <linux/io.h>
#include <asm/mmu.h>

void __iomem *__init_refok
__ioremap(phys_addr_t paddr, unsigned long size, pgprot_t prot)
{
	unsigned long end;

	/* Don't allow wraparound or zero size */
	end = paddr + size - 1;
	if (!size || (end < paddr))
		return NULL;

	/*
	 * If the requested address is in the direct-mapped space,
	 * we can translate it here and don't need to map it through the MMU.
	 */
	if (paddr <= ECO32_DIRECT_MAP_PADDR_END)
		return (void __iomem *)((char *)paddr +
					ECO32_DIRECT_MAP_VADDR_START);

	/* ... and there are no I/O devices in other ranges */
	return NULL;
}

/*
 * Since nothing is ever actually mapped in __ioremap(),
 * nothing needs to be done here.
 */
void iounmap(void *addr)
{
	return;
}
