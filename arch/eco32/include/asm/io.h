/*
 * Copyright (C) 2014 Stefan Kristiansson, stefan.kristiansson@saunalahti.fi
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2.  This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */

#ifndef __ASM_ECO32_IO_H
#define __ASM_ECO32_IO_H

#include <asm-generic/io.h>
#include <asm/pgtable.h>

extern void __iomem *__ioremap(phys_addr_t offset, unsigned long size,
			       pgprot_t prot);

static inline void __iomem *ioremap(phys_addr_t offset, unsigned long size)
{
	return __ioremap(offset, size, PAGE_KERNEL);
}

static inline void __iomem *ioremap_nocache(phys_addr_t offset,
					    unsigned long size)
{
	return __ioremap(offset, size, __pgprot(pgprot_val(PAGE_KERNEL)));
}

extern void iounmap(void *addr);
#endif
