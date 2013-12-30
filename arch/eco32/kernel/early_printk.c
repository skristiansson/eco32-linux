/*
 * Copyright (C) 2014 Stefan Kristiansson, stefan.kristiansson@saunalahti.fi
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2.  This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */

#include <linux/kernel.h>
#include <linux/console.h>
#include <linux/printk.h>
#include <linux/init.h>

static void early_printk_uart_putc(char c)
{
	volatile unsigned int *base = (unsigned int *)0xf0300000;

	if (c == '\n')
		early_printk_uart_putc('\r');

	while ((*(base + 2) & 1) == 0)
		;

	*(base + 3) = c;
}

static void early_printk_uart_write(struct console *con, const char *s,
				    unsigned n)
{
	while (n-- && *s) {
		if (*s == '\n')
			early_printk_uart_putc('\r');
		early_printk_uart_putc(*s);
		s++;
	}
}

static struct console early_printk_uart = {
	.name	= "early",
	.write	= early_printk_uart_write,
	.flags	= CON_PRINTBUFFER | CON_BOOT,
	.index	= -1
};

void __init setup_early_printk(void)
{
	if (early_console)
		return;
	early_console = &early_printk_uart;

	register_console(&early_printk_uart);
}
