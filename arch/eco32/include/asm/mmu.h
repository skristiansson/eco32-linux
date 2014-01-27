#ifndef __ASM_ECO32_MMU_H
#define __ASM_ECO32_MMU_H
/*
 * eco32 maps the physical address space 0x00000000 - 0x3fffffff
 * into the virtual address space 0xc0000000 - 0xffffffff accessible
 * by the kernel.
 */
#define ECO32_DIRECT_MAP_PADDR_START	0x00000000
#define ECO32_DIRECT_MAP_PADDR_END	0x3fffffff
#define ECO32_DIRECT_MAP_VADDR_START	0xc0000000
#define ECO32_DIRECT_MAP_VADDR_END	0xffffffff

#ifndef __ASSEMBLY__
typedef unsigned long mm_context_t;
#endif

#endif
