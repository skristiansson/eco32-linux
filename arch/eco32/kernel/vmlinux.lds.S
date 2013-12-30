/*
 * Copyright (C) 2014 Stefan Kristiansson, stefan.kristiansson@saunalahti.fi
 *
 * Based on the OpenRISC implementation:
 * Copyright (C) 2003 Matjaz Breskvar <phoenix@bsemi.com>
 * Copyright (C) 2010-2011 Jonas Bonn <jonas@southpole.se>
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2, or (at your option) any later version.
 * This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */

#define LOAD_OFFSET  PAGE_OFFSET
#define LOAD_BASE    PAGE_OFFSET

#include <asm/page.h>
#include <asm/cache.h>
#include <asm-generic/vmlinux.lds.h>
#include <asm/thread_info.h>

OUTPUT_FORMAT("elf32-eco32", "elf32-eco32", "elf32-eco32")
jiffies = jiffies_64 + 4;

SECTIONS {
	 /* Read-only sections, merged into text segment: */
	 . = LOAD_BASE ;

	/* _s_kernel_ro must be page aligned */
	. = ALIGN(PAGE_SIZE);
	_s_kernel_ro = .;

	.text : AT(ADDR(.text) - LOAD_OFFSET) {
		_stext = .;
		TEXT_TEXT
		SCHED_TEXT
		LOCK_TEXT
		KPROBES_TEXT
		IRQENTRY_TEXT
		*(.fixup)
		*(.text.__*)
		_etext = .;
	 }
	/* TODO: Check if fixup and text.__* are really necessary
	 * fixup is definitely necessary
	 */

	_sdata = .;

	/* Page alignment required for RO_DATA_SECTION */
	RO_DATA_SECTION(PAGE_SIZE)
	_e_kernel_ro = .;

	/* Whatever comes after _e_kernel_ro had better be page-aligend, too */

	RW_DATA_SECTION(32, PAGE_SIZE, THREAD_SIZE)

	_edata  =  .;

	EXCEPTION_TABLE(4)
	NOTES

	/* Init code and data */
	. = ALIGN(PAGE_SIZE);
	__init_begin = .;

	HEAD_TEXT_SECTION

	/* Page aligned */
	INIT_TEXT_SECTION(PAGE_SIZE)

	/* Align __setup_start on 16 byte boundary */
	INIT_DATA_SECTION(16)

	PERCPU_SECTION(L1_CACHE_BYTES)

	__init_end = .;

	. = ALIGN(PAGE_SIZE);
	.initrd : AT(ADDR(.initrd) - LOAD_OFFSET) {
		__initrd_start = .;
		*(.initrd)
		__initrd_end = .;
		FILL (0);
		. = ALIGN (PAGE_SIZE);
	}

	__vmlinux_end = .;            /* last address of the physical file */

	BSS_SECTION(0, 0, 0x20)

        _end = .;

	/* Throw in the debugging sections */
	STABS_DEBUG
	DWARF_DEBUG

        /* Sections to be discarded -- must be last */
	DISCARDS
}
