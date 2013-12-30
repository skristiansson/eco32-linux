/* SJK FIXME */

#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <asm/delay.h>
#include <asm/timex.h>
#include <asm/processor.h>

extern unsigned long loops_per_jiffy;

int read_current_timer(unsigned long *timer_value)
{
	return -ENXIO;

	*timer_value = 0; /* SJK FIXME */
	return 0;
}

void __delay(unsigned long cycles)
{
	cycles_t start = get_cycles();

	/* SJK FIXME */
	while (cycles--)
		cpu_relax();
	return;
	/* SJK FIXME */

	while ((get_cycles() - start) < cycles)
		cpu_relax();
}
EXPORT_SYMBOL(__delay);

inline void __const_udelay(unsigned long xloops)
{
	unsigned long long loops;

	/* SJK FIXME */
	loops = (unsigned long long)xloops * 50 * HZ;
	__delay(loops >> 32);
	return;
	/* SJK FIXME */

	loops = (unsigned long long)xloops * loops_per_jiffy * HZ;

	__delay(loops >> 32);
}
EXPORT_SYMBOL(__const_udelay);

void __udelay(unsigned long usecs)
{
	__const_udelay(usecs * 0x10C7UL); /* 2**32 / 1000000 (rounded up) */
}
EXPORT_SYMBOL(__udelay);

void __ndelay(unsigned long nsecs)
{
	__const_udelay(nsecs * 0x5UL); /* 2**32 / 1000000000 (rounded up) */
}
EXPORT_SYMBOL(__ndelay);
