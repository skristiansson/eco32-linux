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
#include <linux/export.h>
#include <linux/irqdomain.h>
#include <linux/irqflags.h>

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

void __init init_IRQ(void)
{
	/* SJK TODO */
}

void __irq_entry do_IRQ(struct pt_regs *regs)
{
	/* SJK TODO */
}
