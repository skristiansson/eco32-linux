/*
 * Copyright (C) 2014 Stefan Kristiansson, stefan.kristiansson@saunalahti.fi
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2.  This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */

#ifndef __ASM_ECO32_SETUP_H
#define __ASM_ECO32_SETUP_H

#include <uapi/asm/setup.h>

#ifndef __ASSEMBLY__
extern void setup_early_printk(void);
extern char cmd_line[COMMAND_LINE_SIZE];
#endif

#endif /* __ASM_ECO32_SETUP_H */
