/*
 * Copyright (C) 2014 Stefan Kristiansson, stefan.kristiansson@saunalahti.fi
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2.  This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */

#include <linux/mm.h>
#include <linux/sched.h>

volatile pgd_t *current_pgd;

void do_page_fault(struct pt_regs *regs, unsigned long address)
{
	struct task_struct *tsk;
	struct mm_struct *mm;
	struct vm_area_struct *vma;
	int fault;
	unsigned int eid = (regs->psw >> SPR_PSW_EID_BIT) & 0x1f;
	unsigned int flags = FAULT_FLAG_ALLOW_RETRY | FAULT_FLAG_KILLABLE;

	printk("SJK DEBUG: %s\n", __func__);
	tsk = current;

	/* If parent context had interrupts enabled, re-enable them */
	if (interrupts_enabled(regs))
		local_irq_enable();

	if (user_mode(regs))
		flags |= FAULT_FLAG_USER;

	printk("SJK DEBUG: %s: tsk = %x\n", __func__, tsk);
	mm = tsk->mm;
	printk("SJK DEBUG: %s: mm = %x\n", __func__, mm);
	if (!mm)
		goto no_context;

retry:
	down_read(&mm->mmap_sem);
	vma = find_vma(mm, address);
	printk("SJK DEBUG: %s: vma = %x, vma->vm_start = %x, address = %x\n",
	       __func__, vma, vma->vm_start, address);

	if (vma->vm_start <= address)
		goto good_area;

good_area:
	if (unlikely(!(vma->vm_flags & (VM_READ | VM_EXEC))))
		goto bad_area;

	if (eid == EID_TLB_WRITE)
		flags |= FAULT_FLAG_WRITE;

	fault = handle_mm_fault(mm, vma, address, flags);

	if ((fault & VM_FAULT_RETRY) && fatal_signal_pending(current))
		return;

	if (unlikely(fault & VM_FAULT_ERROR)) {
		if (fault & VM_FAULT_OOM)
			goto out_of_memory;
		else if (fault & VM_FAULT_SIGBUS)
			goto do_sigbus;
		BUG();
	}

	if (flags & FAULT_FLAG_ALLOW_RETRY) {
		if (unlikely(fault & VM_FAULT_MAJOR))
			current->maj_flt++;
		else
			current->min_flt++;
		if (fault & VM_FAULT_RETRY) {
			flags &= ~FAULT_FLAG_ALLOW_RETRY;
			flags |= FAULT_FLAG_TRIED;

			/*
			 * No need to up_read(&mm->mmap_sem) as we would
			 * have already released it in __lock_page_or_retry
			 * in mm/filemap.c.
                         */
			goto retry;
		}
	}

	up_read(&mm->mmap_sem);

	return;

bad_area:
	up_read(&mm->mmap_sem);

bad_area_nosemaphore:

no_context:
out_of_memory:
do_sigbus:
	BUG(); /* SJK TODO */
}
