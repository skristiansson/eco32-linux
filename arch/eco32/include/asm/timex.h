/*
 * Copyright (C) 2014 Stefan Kristiansson, stefan.kristiansson@saunalahti.fi
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2.  This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */

#ifndef __ASM_ECO32_TIMEX_H
#define __ASM_ECO32_TIMEX_H

#define get_cycles get_cycles

#include <asm-generic/timex.h>
static int sjk_debug;
static inline cycles_t get_cycles(void)
{
	return sjk_debug++;//0; /* SJK FIXME */
}

#define ARCH_HAS_READ_CURRENT_TIMER
#endif
