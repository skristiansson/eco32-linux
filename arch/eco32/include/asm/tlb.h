/*
 * Copyright (C) 2014 Stefan Kristiansson, stefan.kristiansson@saunalahti.fi
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2.  This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */

#ifndef __ASM_ECO32_TLB_H
#define __ASM_ECO32_TLB_H

#define tlb_start_vma(tlb, vma)				do { } while (0)
#define tlb_end_vma(tlb, vma)				do { } while (0)
#define __tlb_remove_tlb_entry(tlb, ptep, address)	do { } while (0)

#define tlb_flush(tlb)					flush_tlb_mm((tlb)->mm)
#include <linux/pagemap.h>
#include <asm-generic/tlb.h>

#endif /* __ASM_ECO32_TLB_H */
