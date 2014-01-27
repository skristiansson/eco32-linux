/*
 * Copyright (C) 2014 Stefan Kristiansson, stefan.kristiansson@saunalahti.fi
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2.  This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */

#include <linux/interrupt.h>
#include <linux/init.h>
#include <linux/ftrace.h>
#include <linux/irq.h>
#include <linux/irqchip.h>
#include <linux/export.h>
#include <linux/irqdomain.h>
#include <linux/irqflags.h>
#include <linux/of.h>
#include <linux/of_platform.h>

#include "../../drivers/irqchip/irqchip.h"

/* SJK: use generic versions? */
static void eco32_irq_mask(struct irq_data *data)
{
	/* SJK TODO */
}

static void eco32_irq_unmask(struct irq_data *data)
{
	/* SJK TODO */
}

static void eco32_irq_ack(struct irq_data *data)
{
	/* SJK TODO */
}

static void eco32_irq_mask_ack(struct irq_data *data)
{
	/* SJK TODO */
}

static struct irq_chip eco32_dev = {
	.name = "eco32-irq_chip",
	.irq_unmask = eco32_irq_unmask,
	.irq_mask = eco32_irq_mask,
	.irq_ack = eco32_irq_ack,
	.irq_mask_ack = eco32_irq_mask_ack,
};

static struct irq_domain *root_domain;

static inline int get_irq(int first)
{
	/* SJK TODO */
	return -1;
}


static int eco32_map(struct irq_domain *d, unsigned int irq, irq_hw_number_t hw)
{
	irq_set_chip_and_handler_name(irq, &eco32_dev,
				      handle_level_irq, "level");
	irq_set_status_flags(irq, IRQ_LEVEL | IRQ_NOPROBE);

	return 0;
}

static const struct irq_domain_ops eco32_irq_domain_ops = {
	.xlate = irq_domain_xlate_onecell,
	.map = eco32_map,
};

static int __init
init_eco32_IRQ(struct device_node *intc, struct device_node *parent)
{
	/* Disable interrupts */
	mvts(SPR_PSW, mvfs(SPR_PSW) & ~(SPR_PSW_IEN(0xffff)));

	root_domain = irq_domain_add_linear(intc, 16,
					    &eco32_irq_domain_ops, NULL);
	return 0;
}
IRQCHIP_DECLARE(eco32_intc, "eco32,intc", init_eco32_IRQ);

void __init init_IRQ(void)
{
	/* process the entire interrupt tree in one go */
	irqchip_init();
}

void __irq_entry do_IRQ(struct pt_regs *regs)
{
	BUG(); /* SJK TODO */
}
