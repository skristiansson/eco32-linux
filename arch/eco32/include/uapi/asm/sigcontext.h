/*
 * Copyright (C) 2014 Stefan Kristiansson, stefan.kristiansson@saunalahti.fi
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2.  This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */

#ifndef __ASM_ECO32_SIGCONTEXT_H
#define __ASM_ECO32_SIGCONTEXT_H

#include <asm/ptrace.h>

struct sigcontext {
	struct pt_regs regs;
	unsigned long oldmask;
};
#endif /* __ASM_ECO32_SIGCONTEXT_H */
