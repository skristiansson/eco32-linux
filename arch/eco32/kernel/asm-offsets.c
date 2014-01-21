/*
 * Copyright (C) 2014 Stefan Kristiansson, stefan.kristiansson@saunalahti.fi
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2.  This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */

#include <linux/ptrace.h>
#include <linux/kbuild.h>

int main(void)
{
	/* offsets into the task_struct */
	DEFINE(TASK_STATE, offsetof(struct task_struct, state));
	DEFINE(TASK_FLAGS, offsetof(struct task_struct, flags));
	DEFINE(TASK_PTRACE, offsetof(struct task_struct, ptrace));
	DEFINE(TASK_THREAD, offsetof(struct task_struct, thread));
	DEFINE(TASK_MM, offsetof(struct task_struct, mm));
	DEFINE(TASK_ACTIVE_MM, offsetof(struct task_struct, active_mm));

	/* offsets into thread_info */
	DEFINE(TI_TASK, offsetof(struct thread_info, task));
	DEFINE(TI_FLAGS, offsetof(struct thread_info, flags));
	DEFINE(TI_PREEMPT, offsetof(struct thread_info, preempt_count));
	DEFINE(TI_CPU_CONTEXT, offsetof(struct thread_info, cpu_context));

	/* offsets into pt_regs */
	DEFINE(PT_SIZE, sizeof(struct pt_regs));
	DEFINE(PT_GPR0, offsetof(struct pt_regs, gpr[0]));
	DEFINE(PT_GPR1, offsetof(struct pt_regs, gpr[1]));
	DEFINE(PT_GPR2, offsetof(struct pt_regs, gpr[2]));
	DEFINE(PT_GPR3, offsetof(struct pt_regs, gpr[3]));
	DEFINE(PT_GPR4, offsetof(struct pt_regs, gpr[4]));
	DEFINE(PT_GPR5, offsetof(struct pt_regs, gpr[5]));
	DEFINE(PT_GPR6, offsetof(struct pt_regs, gpr[6]));
	DEFINE(PT_GPR7, offsetof(struct pt_regs, gpr[7]));
	DEFINE(PT_GPR8, offsetof(struct pt_regs, gpr[8]));
	DEFINE(PT_GPR9, offsetof(struct pt_regs, gpr[9]));
	DEFINE(PT_GPR10, offsetof(struct pt_regs, gpr[10]));
	DEFINE(PT_GPR11, offsetof(struct pt_regs, gpr[11]));
	DEFINE(PT_GPR12, offsetof(struct pt_regs, gpr[12]));
	DEFINE(PT_GPR13, offsetof(struct pt_regs, gpr[13]));
	DEFINE(PT_GPR14, offsetof(struct pt_regs, gpr[14]));
	DEFINE(PT_GPR15, offsetof(struct pt_regs, gpr[15]));
	DEFINE(PT_GPR16, offsetof(struct pt_regs, gpr[16]));
	DEFINE(PT_GPR17, offsetof(struct pt_regs, gpr[17]));
	DEFINE(PT_GPR18, offsetof(struct pt_regs, gpr[18]));
	DEFINE(PT_GPR19, offsetof(struct pt_regs, gpr[19]));
	DEFINE(PT_GPR20, offsetof(struct pt_regs, gpr[20]));
	DEFINE(PT_GPR21, offsetof(struct pt_regs, gpr[21]));
	DEFINE(PT_GPR22, offsetof(struct pt_regs, gpr[22]));
	DEFINE(PT_GPR23, offsetof(struct pt_regs, gpr[23]));
	DEFINE(PT_GPR24, offsetof(struct pt_regs, gpr[24]));
	DEFINE(PT_GPR25, offsetof(struct pt_regs, gpr[25]));
	DEFINE(PT_GPR26, offsetof(struct pt_regs, gpr[26]));
	DEFINE(PT_GPR27, offsetof(struct pt_regs, gpr[27]));
	DEFINE(PT_GPR28, offsetof(struct pt_regs, gpr[28]));
	DEFINE(PT_GPR29, offsetof(struct pt_regs, gpr[29]));
	DEFINE(PT_GPR30, offsetof(struct pt_regs, gpr[30]));
	DEFINE(PT_GPR31, offsetof(struct pt_regs, gpr[31]));
	DEFINE(PT_PC, offsetof(struct pt_regs, pc));
	DEFINE(PT_PSW, offsetof(struct pt_regs, psw));

	/* offsets into cpu_context */
	DEFINE(CC_GPR16, offsetof(struct cpu_context, r16));
	DEFINE(CC_GPR17, offsetof(struct cpu_context, r17));
	DEFINE(CC_GPR18, offsetof(struct cpu_context, r18));
	DEFINE(CC_GPR19, offsetof(struct cpu_context, r19));
	DEFINE(CC_GPR20, offsetof(struct cpu_context, r20));
	DEFINE(CC_GPR21, offsetof(struct cpu_context, r21));
	DEFINE(CC_GPR22, offsetof(struct cpu_context, r22));
	DEFINE(CC_GPR23, offsetof(struct cpu_context, r23));
	DEFINE(CC_GPR28, offsetof(struct cpu_context, r28));
	DEFINE(CC_GPR29, offsetof(struct cpu_context, r29));
	DEFINE(CC_GPR31, offsetof(struct cpu_context, r31));

	DEFINE(NUM_USER_SEGMENTS, TASK_SIZE >> 28);
	return 0;
}
