/*
 * Copyright (C) 2014 Stefan Kristiansson, stefan.kristiansson@saunalahti.fi
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2.  This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */

#include <linux/mm.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <asm/pgtable.h>
#include <asm/uaccess.h>

extern void die(char *, struct pt_regs *, long);

volatile pgd_t *current_pgd;

static int vmalloc_fault(unsigned long address)
{
	pgd_t *pgd, *pgd_k;
	pud_t *pud, *pud_k;
	pmd_t *pmd, *pmd_k;

        /* Make sure we are in vmalloc area: */
	if (!(address >= VMALLOC_START && address < VMALLOC_END))
		return -1;

	pgd = (pgd_t *)current_pgd + pgd_index(address);
	pgd_k = pgd_offset_k(address);
	if (!pgd_present(*pgd_k))
		return -1;

	pud = pud_offset(pgd, address);
	pud_k = pud_offset(pgd_k, address);
	if (!pud_present(*pud_k))
		return -1;

	pmd = pmd_offset(pud, address);
	pmd_k = pmd_offset(pud_k, address);
	if (!pmd_present(*pmd_k))
		return -1;

	set_pmd(pmd, *pmd_k);

	return 0;
}

int fixup_exception(struct pt_regs *regs)
{
        const struct exception_table_entry *entry;

	entry = search_exception_tables(regs->pc);
	if (entry) {
		regs->pc = entry->fixup;
		return 1;
        }

	return 0;
}

void do_page_fault(struct pt_regs *regs, unsigned long address)
{
	struct task_struct *tsk;
	struct mm_struct *mm;
	struct vm_area_struct *vma;
	siginfo_t info;
	int fault;
	unsigned int eid = (regs->psw >> SPR_PSW_EID_BIT) & 0x1f;
	unsigned int flags = FAULT_FLAG_ALLOW_RETRY | FAULT_FLAG_KILLABLE;

	tsk = current;

	/* Fault in kernel space that is not a protection fault. */
	if (kernel_mode(regs) && eid == EID_TLB_MISS) {
		if (!vmalloc_fault(address))
			return;
	}

	/* If parent context had interrupts enabled, re-enable them */
	if (interrupts_enabled(regs))
		local_irq_enable();

	if (user_mode(regs))
		flags |= FAULT_FLAG_USER;

	mm = tsk->mm;
	if (in_interrupt() || !mm)
		goto no_context;

retry:
	down_read(&mm->mmap_sem);
	vma = find_vma(mm, address);
	if (!vma)
		goto bad_area;

	if (vma->vm_start <= address)
		goto good_area;

	if (!(vma->vm_flags & VM_GROWSDOWN))
		goto bad_area;

	if (expand_stack(vma, address))
		goto bad_area;

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

	if (user_mode(regs)) {
		info.si_signo = SIGSEGV;
		info.si_errno = 0;
		info.si_code = SEGV_ACCERR;
		info.si_addr = (void __user *)address;
		force_sig_info(SIGBUS, &info, current);
		return;
	}

no_context:
	/*
	 * Are we prepared to handle this kernel fault?
	 *
	 * (The kernel has valid exception-points in the source
	 *  when it acesses user-memory. When it fails in one
	 *  of those points, we find it in a table and do a jump
	 *  to some fixup code that loads an appropriate error
	 *  code)
	 */
	if (fixup_exception(regs))
		return;

	die("Oops", regs, address);

out_of_memory:
	up_read(&mm->mmap_sem);

	if (!user_mode(regs))
		goto no_context;

	pagefault_out_of_memory();

	return;

do_sigbus:
	up_read(&mm->mmap_sem);

	if (!user_mode(regs))
		goto no_context;

	info.si_signo = SIGBUS;
	info.si_errno = 0;
	info.si_code = BUS_ADRERR;
	info.si_addr = (void __user *)address;
	force_sig_info(SIGBUS, &info, current);
}
