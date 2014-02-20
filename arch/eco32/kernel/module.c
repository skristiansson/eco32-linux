/*
 * Copyright (C) 2014 Stefan Kristiansson, stefan.kristiansson@saunalahti.fi
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2.  This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */

#include <linux/moduleloader.h>
#include <linux/elf.h>

int apply_relocate_add(Elf32_Shdr *sechdrs,
		       const char *strtab,
		       unsigned int symindex,
		       unsigned int relsec,
		       struct module *me)
{
	unsigned int i;
	Elf32_Rela *rel = (void *)sechdrs[relsec].sh_addr;
	Elf32_Sym *sym;
	u32 *location;
	u32 value;

	pr_debug("Applying relocate section %u to %u\n", relsec,
		 sechdrs[relsec].sh_info);

	for (i = 0; i < sechdrs[relsec].sh_size / sizeof(*rel); i++) {
		/* This is where to make the change */
		location = (void *)sechdrs[sechdrs[relsec].sh_info].sh_addr +
			rel[i].r_offset;
		/* This is the symbol it is referring to. */
		sym = (Elf32_Sym *)sechdrs[symindex].sh_addr +
			ELF32_R_SYM(rel[i].r_info);
		value = sym->st_value + rel[i].r_addend;

		switch (ELF32_R_TYPE(rel[i].r_info)) {
		case R_ECO32_W32:
			*location = value;
			break;

		case R_ECO32_HI16:
			*((u16 *)location + 1) = value >> 16;
			break;

		case R_ECO32_LO16:
			*((u16 *)location + 1) = value;
			break;

		case R_ECO32_R16:
			/* Relocations are PC + 4 relative */
			value -= (u32)location + 4;
			*location &= 0xffff0000;
			*location |= (value >> 2) & 0x0000ffff;
			break;

		case R_ECO32_R26:
			/* Relocations are PC + 4 relative */
			value -= (u32)location + 4;
			*location &= 0xfc000000;
			*location |= (value >> 2) & 0x03ffffff;
			break;

		default:
			pr_err("module %s: Unknown relocation: %u\n",
			       me->name, ELF32_R_TYPE(rel[i].r_info));
			return -ENOENT;
		}
	}

	return 0;
}
