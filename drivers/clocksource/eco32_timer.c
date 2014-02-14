/*
 * Driver for the timer present in the eco32 reference system.
 *
 * Copyright (C) 2014 Stefan Kristiansson, stefan.kristiansson@saunalahti.fi
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2.  This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */

#include <linux/interrupt.h>
#include <linux/clk.h>
#include <linux/clocksource.h>
#include <linux/clockchips.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>

#define ECO32_TIMER_CTRL	0x00000000
#define ECO32_TIMER_DIV		0x00000004
#define ECO32_TIMER_CNT		0x00000008

#define ECO32_TIMER_WRAP	(1 << 0)
#define ECO32_TIMER_IE		(1 << 1)

static struct clock_event_device eco32_clockevent_device;
static void __iomem *eco32_timer_base;
static u32 eco32_timer_remainder;

static u32 eco32_timer_read_reg(loff_t offset)
{
	return ioread32be(eco32_timer_base + offset);
}

static void eco32_timer_write_reg(loff_t offset, u32 val)
{
	iowrite32be(val, eco32_timer_base + offset);
}

/*
 * Prepare the timer for an event.
 * We want to use the timer as a continous clock source as well,
 * so the difference between the current counter value and the delta value
 * is accumulated into a 'remainder' variable, that will be used to
 * provide the 'correct' continous timer value in the clock source
 * read function.
 * Correct is a bit of a bold statement though, since the timer will drift in
 * between the read and the write.
 */
static int eco32_timer_set_next_event(unsigned long delta,
				      struct clock_event_device *dev)
{
	eco32_timer_remainder += eco32_timer_read_reg(ECO32_TIMER_CNT) - delta;
	eco32_timer_write_reg(ECO32_TIMER_DIV, delta);
	eco32_timer_write_reg(ECO32_TIMER_CTRL, ECO32_TIMER_IE);

	return 0;
}

static void eco32_timer_set_mode(enum clock_event_mode mode,
				 struct clock_event_device *evt)
{
	switch (mode) {
	case CLOCK_EVT_MODE_PERIODIC:
		pr_info( "%s: periodic\n", __func__);
		break;
	case CLOCK_EVT_MODE_ONESHOT:
		pr_info( "%s: oneshot\n", __func__);
		break;
	case CLOCK_EVT_MODE_UNUSED:
		pr_info( "%s: unused\n", __func__);
		break;
	case CLOCK_EVT_MODE_SHUTDOWN:
		pr_info( "%s: shutdown\n", __func__);
		break;
	case CLOCK_EVT_MODE_RESUME:
		pr_info( "%s: resume\n", __func__);
		break;
	}
}

static struct clock_event_device eco32_clockevent_device = {
        .name           = "eco32_clockevent",
	.features       = CLOCK_EVT_FEAT_ONESHOT,
	.set_mode       = eco32_timer_set_mode,
	.set_next_event = eco32_timer_set_next_event,
	.rating         = 300,
};

static __init void eco32_clockevent_init(struct clk *clk)
{
	eco32_clockevent_device.cpumask = cpumask_of(0);
	clockevents_config_and_register(&eco32_clockevent_device,
					clk_get_rate(clk),
					0x1, 0xffffffff);
}

static void eco32_timer_ack(void)
{
	eco32_timer_write_reg(ECO32_TIMER_DIV, ~0UL);
	/* clear wrap and ie flags */
	eco32_timer_write_reg(ECO32_TIMER_CTRL, 0);
}

static irqreturn_t eco32_timer_interrupt(int irq, void *dev_id)
{
	struct clock_event_device *evt = dev_id;

	eco32_timer_ack();
	evt->event_handler(evt);

	return IRQ_HANDLED;
}

static struct irqaction eco32_timer_irq = {
        .name           = "Eco32 Timer",
	.dev_id         = &eco32_clockevent_device,
	.flags          = IRQF_TIMER,
	.handler        = eco32_timer_interrupt,
};

static cycle_t eco32_timer_read(struct clocksource *cs)
{
	return (cycle_t) ~(eco32_timer_read_reg(ECO32_TIMER_CNT) +
			   eco32_timer_remainder);
}

static struct clocksource clocksource_eco32 = {
	.name	= "eco32_timer",
	.rating	= 100,
	.read	= eco32_timer_read,
	.mask	= CLOCKSOURCE_MASK(32),
	.flags	= CLOCK_SOURCE_IS_CONTINUOUS,
};

static __init void eco32_clocksource_init(struct clk *clk)
{
	clocksource_register_hz(&clocksource_eco32, clk_get_rate(clk));
	eco32_timer_write_reg(ECO32_TIMER_DIV, ~0UL);
	eco32_timer_write_reg(ECO32_TIMER_CTRL, 0);
}

static __init void eco32_timer_init(struct device_node *node)
{
	int irq;
	struct clk *clk;

	eco32_timer_base = of_iomap(node, 0);
	if (!eco32_timer_base)
		panic("%s: unable to map resource\n", node->name);

	irq = irq_of_parse_and_map(node, 0);
	if (irq <= 0)
		panic("%s: unable to parse irq\n", node->name);

	if (setup_irq(irq, &eco32_timer_irq))
		panic("%s: unable to setup irq\n", node->name);

	clk = of_clk_get(node, 0);
	if (IS_ERR(clk))
		panic("%s: unable to get clk\n", node->name);
	clk_prepare_enable(clk);

	eco32_clocksource_init(clk);
	eco32_clockevent_init(clk);
}
CLOCKSOURCE_OF_DECLARE(eco32_timer, "eco32,timer", eco32_timer_init);
