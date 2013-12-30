#ifndef _UAPI__ASM_ECO32_ELF_H
#define _UAPI__ASM_ECO32_ELF_H

/*
 * This files is partially exported to userspace.  This allows us to keep
 * the ELF bits in one place which should assist in keeping the kernel and
 * userspace in sync.
 */

/*
 * ELF register definitions..
 */

/* for struct user_regs_struct definition */
#include <asm/ptrace.h>

/* The ECO32 relocation types */
#define R_ECO32_NONE	0
#define R_ECO32_W32	1
#define R_ECO32_HI16	2
#define R_ECO32_LO16	3
#define R_ECO32_R16	4
#define R_ECO32_R26	5

typedef unsigned long elf_greg_t;

/*
 * Note that NGREG is defined to ELF_NGREG in include/linux/elfcore.h, and is
 * thus exposed to user-space.
 */
#define ELF_NGREG (sizeof(struct pt_regs) / sizeof(elf_greg_t))
typedef elf_greg_t elf_gregset_t[ELF_NGREG];

/* A placeholder; eco32 does not have fp support yet, so no fp regs for now.  */
typedef unsigned long elf_fpregset_t;

/* This should be moved to include/linux/elf.h */
#define EM_ECO32	0xEC05

/*
 * These are used to set parameters in the core dumps.
 */
#define ELF_ARCH	EM_ECO32
#define ELF_CLASS	ELFCLASS32
#define ELF_DATA	ELFDATA2MSB

#endif /* _UAPI__ASM_ECO32_ELF_H */
