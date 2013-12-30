/*
 * Copyright (C) 2014 Stefan Kristiansson, stefan.kristiansson@saunalahti.fi
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2.  This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */
#ifndef _UAPI_ASM_ECO32_PTRACE_H
#define _UAPI_ASM_ECO32_PTRACE_H

#ifndef __ASSEMBLY__

struct pt_regs {
	unsigned long gpr[32];
	unsigned long pc;
	unsigned long psw; /* Processor Status Word */
};

#endif /* __ASSEMBLY__ */

#endif /* _UAPI_ASM_ECO32_PTRACE_H */
