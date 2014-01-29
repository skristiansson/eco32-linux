/*
 * Copyright (C) 2014 Stefan Kristiansson, stefan.kristiansson@saunalahti.fi
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2. This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 *
 * Driver for the uart core included in the eco32 reference design.
 * More or less based on arc_uart and altera_uart.
 */

#include <linux/module.h>
#include <linux/console.h>
#include <linux/serial.h>
#include <linux/serial_core.h>
#include <linux/tty_flip.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_platform.h>

#define DRIVER_NAME		"eco32-uart"
#define ECO32_SERIAL_DEV_NAME	"ttyECO"

/* Register definitions */
#define ECO32_UART_RXCTRL	0x0
#define ECO32_UART_RXDATA	0x4
#define ECO32_UART_TXCTRL	0x8
#define ECO32_UART_TXDATA	0xC

/* Bit defines for TXCTRL and RXCTRL */
#define ECO32_UART_READY	(1 << 0)
#define ECO32_UART_IE		(1 << 1)

static struct uart_port eco32_uart_ports[CONFIG_SERIAL_ECO32_NR_PORTS];
#ifdef CONFIG_SERIAL_ECO32_CONSOLE
static struct console eco32_uart_console;
#endif

static struct uart_driver eco32_uart_driver = {
	.owner		= THIS_MODULE,
	.driver_name	= DRIVER_NAME,
	.dev_name	= ECO32_SERIAL_DEV_NAME,
	.major		= 0,
	.minor		= 0,
	.nr		= CONFIG_SERIAL_ECO32_NR_PORTS,
#ifdef CONFIG_SERIAL_ECO32_CONSOLE
	.cons		= &eco32_uart_console,
#endif
};

static u32 eco32_uart_read_reg(struct uart_port *port, loff_t offset)
{
	return ioread32be(port->membase + offset);
}

static void eco32_uart_write_reg(struct uart_port *port, loff_t offset, u32 val)
{
	iowrite32be(val, port->membase + offset);
}

static unsigned int eco32_uart_tx_empty(struct uart_port *port)
{
	unsigned int txctrl = eco32_uart_read_reg(port, ECO32_UART_TXCTRL);

	if (txctrl & ECO32_UART_READY)
		return TIOCSER_TEMT;

	return 0;
}

static void eco32_uart_putc(struct uart_port *port, const char c)
{
	while (!eco32_uart_tx_empty(port))
		cpu_relax();

	eco32_uart_write_reg(port, ECO32_UART_TXDATA, c);
}

static void eco32_uart_tx_chars(struct uart_port *port)
{
	struct circ_buf *xmit = &port->state->xmit;

	while (xmit->tail != xmit->head) {
		eco32_uart_putc(port, xmit->buf[xmit->tail]);
		xmit->tail = (xmit->tail + 1) & (UART_XMIT_SIZE - 1);
		port->icount.tx++;
	}
}

static void eco32_uart_rx_chars(struct uart_port *port)
{
	u32 c;

	while (eco32_uart_read_reg(port, ECO32_UART_RXCTRL) &
	       ECO32_UART_READY) {
		c = eco32_uart_read_reg(port, ECO32_UART_RXDATA);
		uart_insert_char(port, 0, 0, c, TTY_NORMAL);
		port->icount.rx++;

		spin_unlock(&port->lock);
		tty_flip_buffer_push(&port->state->port);
		spin_lock(&port->lock);
	}

}

static irqreturn_t eco32_uart_rx_isr(int irq, void *data)
{
	struct uart_port *port = data;

	spin_lock(&port->lock);
	eco32_uart_rx_chars(port);
	spin_unlock(&port->lock);

	return IRQ_HANDLED;
}

static unsigned int eco32_uart_get_mctrl(struct uart_port *port)
{
	/*
	 * Pretend we have a Modem status reg and following bits are
	 *  always set, to satify the serial core state machine
	 *  (DSR) Data Set Ready
	 *  (CTS) Clear To Send
	 *  (CAR) Carrier Detect
	 */
	return TIOCM_CTS | TIOCM_DSR | TIOCM_CAR;
}

static void eco32_uart_set_mctrl(struct uart_port *port, unsigned int mctrl)
{
	/* MCR not present */
}

static void eco32_uart_start_tx(struct uart_port *port)
{
	eco32_uart_tx_chars(port);
}

static void eco32_uart_stop_tx(struct uart_port *port)
{
	while (!eco32_uart_tx_empty(port))
		cpu_relax();
}

static void eco32_uart_stop_rx(struct uart_port *port)
{
	u32 rxctrl;

	if (request_irq(port->irq, eco32_uart_rx_isr, 0, DRIVER_NAME, port))
		return -EBUSY;

	/* disable rx interrupt */
	rxctrl = eco32_uart_read_reg(port, ECO32_UART_RXCTRL);
	eco32_uart_write_reg(port, ECO32_UART_RXCTRL, rxctrl & ~ECO32_UART_IE);
}

/* Enable Modem Status Interrupts */

static void eco32_uart_enable_ms(struct uart_port *port)
{
	/* MSR not present */
}

static void eco32_uart_break_ctl(struct uart_port *port, int break_state)
{
	/* ECO32 UART doesn't support sending Break signal */
}

static int eco32_uart_startup(struct uart_port *port)
{
	u32 rxctrl;

	if (request_irq(port->irq, eco32_uart_rx_isr, 0, DRIVER_NAME, port))
		return -EBUSY;

	/* enable rx interrupt */
	rxctrl = eco32_uart_read_reg(port, ECO32_UART_RXCTRL);
	eco32_uart_write_reg(port, ECO32_UART_RXCTRL, rxctrl | ECO32_UART_IE);

	return 0;
}

static void eco32_uart_shutdown(struct uart_port *port)
{
	if (port->irq)
		free_irq(port->irq, port);

	return 0;
}

static void eco32_uart_set_termios(struct uart_port *port, struct ktermios *new,
				   struct ktermios *old)
{
	/* The UART only supports a baudrate of 38400 */
	unsigned int baud = 38400;
	unsigned long flags;

	spin_lock_irqsave(&port->lock, flags);

	/*
	 * UART doesn't support Parity/Hardware Flow Control;
	 * Only supports 8N1 character size
	 */
	new->c_cflag &= ~(CMSPAR|CRTSCTS|CSIZE);
	new->c_cflag |= CS8;

	if (old)
		tty_termios_copy_hw(new, old);

	tty_termios_encode_baud_rate(new, baud, baud);
	uart_update_timeout(port, new->c_cflag, baud);

	spin_unlock_irqrestore(&port->lock, flags);
}

static const char *eco32_uart_type(struct uart_port *port)
{
	return port->type == PORT_ECO32 ? DRIVER_NAME : NULL;
}

static int eco32_uart_request_port(struct uart_port *port)
{
	/* UARTs always present */
	return 0;
}

static void eco32_uart_release_port(struct uart_port *port)
{
	/* Nothing to release... */
}

static int eco32_uart_verify_port(struct uart_port *port,
				  struct serial_struct *ser)
{
	if (ser->type != PORT_UNKNOWN && ser->type != PORT_ECO32)
		return -EINVAL;
	return 0;
}

static void eco32_uart_config_port(struct uart_port *port, int flags)
{
	if (flags & UART_CONFIG_TYPE)
		port->type = PORT_ECO32;
}

static struct uart_ops eco32_uart_ops = {
	.tx_empty	= eco32_uart_tx_empty,
	.get_mctrl	= eco32_uart_get_mctrl,
	.set_mctrl	= eco32_uart_set_mctrl,
	.start_tx	= eco32_uart_start_tx,
	.stop_tx	= eco32_uart_stop_tx,
	.stop_rx	= eco32_uart_stop_rx,
	.enable_ms	= eco32_uart_enable_ms,
	.break_ctl	= eco32_uart_break_ctl,
	.startup	= eco32_uart_startup,
	.shutdown	= eco32_uart_shutdown,
	.set_termios	= eco32_uart_set_termios,
	.type		= eco32_uart_type,
	.request_port	= eco32_uart_request_port,
	.release_port	= eco32_uart_release_port,
	.config_port	= eco32_uart_config_port,
	.verify_port	= eco32_uart_verify_port,
#ifdef CONFIG_CONSOLE_POLL
	.poll_get_char	= eco32_uart_poll_get_char,
	.poll_put_char	= eco32_uart_poll_put_char,
#endif
};

#ifdef CONFIG_SERIAL_ECO32_CONSOLE
static void eco32_uart_console_write(struct console *co, const char *s,
				     unsigned int count)
{
	struct uart_port *port = &eco32_uart_ports[co->index];

	for (; count; count--, s++) {
		eco32_uart_putc(port, *s);
		if (*s == '\n')
			eco32_uart_putc(port, '\r');
	}
}

static int __init eco32_uart_console_setup(struct console *co, char *options)
{
	struct uart_port *port;
	/* those are the only settings that are supported */
	int baud = 38400;
	int bits = 8;
	int parity = 'n';
	int flow = 'n';

	if (co->index < 0 || co->index >= CONFIG_SERIAL_ECO32_NR_PORTS)
		return -EINVAL;
	port = &eco32_uart_ports[co->index];
	if (!port->membase)
		return -ENODEV;

	if (options)
		uart_parse_options(options, &baud, &parity, &bits, &flow);

	return uart_set_options(port, co, baud, parity, bits, flow);
}

static struct console eco32_uart_console = {
	.name	= ECO32_SERIAL_DEV_NAME,
	.write	= eco32_uart_console_write,
	.device	= uart_console_device,
	.setup	= eco32_uart_console_setup,
	.flags	= CON_PRINTBUFFER,
	.index	= -1,
	.data	= &eco32_uart_driver,
};

static int __init eco32_uart_console_init(void)
{
	register_console(&eco32_uart_console);
	return 0;
}
console_initcall(eco32_uart_console_init);

#endif /* CONFIG_SERIAL_ECO32_CONSOLE */

static int eco32_uart_probe(struct platform_device *pdev)
{
	int dev_id;
	struct resource *res_mem;
	struct resource *res_irq;
	struct uart_port *port;
	struct device_node *np = pdev->dev.of_node;

	if (!np)
		return -ENODEV;

	dev_id = of_alias_get_id(np, "serial");
	port = &eco32_uart_ports[dev_id < 0 ? 0 : dev_id];

	platform_set_drvdata(pdev, port);

	res_mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res_mem)
		return -ENODEV;

	res_irq = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	if (!res_irq)
		return -ENODEV;

	port->mapbase = res_mem->start;
	port->membase = ioremap_nocache(res_mem->start, resource_size(res_mem));
	if (!port->membase)
		return -ENXIO;

	port->irq = res_irq->start;
	port->dev = &pdev->dev;
	port->iotype = SERIAL_IO_MEM;
	port->ops = &eco32_uart_ops;
	port->flags = UPF_BOOT_AUTOCONF;
	port->line = pdev->id < 0 ? 0 : pdev->id;

	uart_add_one_port(&eco32_uart_driver, port);

	return 0;
}

static int eco32_uart_remove(struct platform_device *pdev)
{
	/* TODO... */
	return 0;
}

static struct of_device_id eco32_uart_match[] = {
	{ .compatible = "eco32,uart", },
	{},
};

static struct platform_driver eco32_uart_platform_driver = {
	.probe = eco32_uart_probe,
	.remove = eco32_uart_remove,
	.driver = {
		.name	= DRIVER_NAME,
		.owner	= THIS_MODULE,
		.of_match_table = eco32_uart_match
	}
};
MODULE_DEVICE_TABLE(of, eco32_uart_match);

static int __init eco32_uart_init(void)
{
	int rc;

	rc = uart_register_driver(&eco32_uart_driver);
	if (rc)
		return rc;
	rc = platform_driver_register(&eco32_uart_platform_driver);
	if (rc)
		uart_unregister_driver(&eco32_uart_driver);

	return rc;
}

static void __exit eco32_uart_exit(void)
{
	platform_driver_unregister(&eco32_uart_platform_driver);
	uart_unregister_driver(&eco32_uart_driver);
}

module_init(eco32_uart_init);
module_exit(eco32_uart_exit);

MODULE_AUTHOR("Stefan Kristiansson <stefan.kristiansson@saunalahti.fi>");
MODULE_DESCRIPTION("ECO32 reference design uart driver");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:" DRIVER_NAME);
MODULE_ALIAS_CHARDEV_MAJOR(SERIAL_ECO32_MAJOR);
