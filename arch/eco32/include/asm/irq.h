#ifndef __ASM_ECO32_IRQ_H__
#define __ASM_ECO32_IRQ_H__

#define	NR_IRQS		16
#include <asm-generic/irq.h>

#define NO_IRQ		(-1)

extern void do_IRQ(int irq, struct pt_regs *regs);

#endif /* __ASM_ECO32_IRQ_H__ */
