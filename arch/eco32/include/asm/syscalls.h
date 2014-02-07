#ifndef __ASM_ECO32_SYSCALLS_H
#define __ASM_ECO32_SYSCALLS_H

asmlinkage long sys_eco32_atomic(unsigned long type, unsigned long arg1,
				 unsigned long arg2, unsigned long arg3);

#include <asm-generic/syscalls.h>

#endif /* __ASM_ECO32_SYSCALLS_H */
