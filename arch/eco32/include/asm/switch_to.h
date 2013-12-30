/*
 * Copyright (C) 2014 Stefan Kristiansson, stefan.kristiansson@saunalahti.fi
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2.  This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */

#ifndef __ASM_ECO32_SWITCH_TO_H
#define __ASM_ECO32_SWITCH_TO_H

struct task_struct;
struct thread_info;

extern struct task_struct *__switch_to(struct thread_info *prev,
				       struct thread_info *next);

#define switch_to(prev, next, last)					\
	do {								\
		(last) = __switch_to(task_thread_info(prev),		\
				     task_thread_info(next));		\
	} while (0)

#endif /* __ASM_ECO32_SWITCH_TO_H */
