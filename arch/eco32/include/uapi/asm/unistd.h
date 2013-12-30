/*
 * Copyright (C) 2014 Stefan Kristiansson, stefan.kristiansson@saunalahti.fi
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2.  This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */

#define sys_mmap2 sys_mmap_pgoff

#define __ARCH_WANT_SYS_CLONE

#include <asm-generic/unistd.h>
