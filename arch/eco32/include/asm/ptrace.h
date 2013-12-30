/*
 * Copyright (C) 2014 Stefan Kristiansson, stefan.kristiansson@saunalahti.fi
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2.  This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */

#ifndef __ASM_ECO32_PTRACE_H
#define __ASM_ECO32_PTRACE_H

#include <uapi/asm/ptrace.h>
#include <asm/spr.h>

#ifndef __ASSEMBLY__
#define user_mode(regs)			((regs)->psw & SPR_PSW_UC)
#define kernel_mode(regs)		(!user_mode(regs))

#define instruction_pointer(regs)	((regs)->pc)
#define profile_pc(regs)		instruction_pointer(regs)
#define user_stack_pointer(regs)	((regs)->gpr[29])

static inline long regs_return_value(struct pt_regs *regs)
{
	return regs->gpr[31];
}

#endif /* __ASSEMBLY__ */

#endif /* __ASM_ECO32_PTRACE_H */
