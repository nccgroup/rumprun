/******************************************************************************
 * common.c
 * 
 * Common stuff special to x86 goes here.
 * 
 * Copyright (c) 2002-2003, K A Fraser & R Neugebauer
 * Copyright (c) 2005, Grzegorz Milos, Intel Research Cambridge
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
 * DEALINGS IN THE SOFTWARE.
 *
 */

#include <mini-os/os.h>

#include <bmk-core/string.h>

/*
 * Shared page for communicating with the hypervisor.
 * Events flags go here, for example.
 */
shared_info_t *HYPERVISOR_shared_info;

/*
 * This structure contains start-of-day info, such as pagetable base pointer,
 * address of the shared_info structure, and things like that.
 */
union start_info_union _minios_start_info_union;

/*
 * Just allocate the kernel stack here. SS:ESP is set up to point here
 * in head.S.
 */
char __guard1[PAGE_SIZE] __attribute__ ((section (".guard1")));
char _minios_stack[2*STACK_SIZE];
//const char *_minios_stack_end = _minios_stack + 2*STACK_SIZE;

extern char _minios_shared_info[PAGE_SIZE];

/* Assembler interface fns in entry.S. */
extern void _minios_entry_hypervisor_callback(void);
extern void _minios_entry_failsafe_callback(void);

#if defined(__x86_64__)
#define __pte(x) ((pte_t) { (x) } )
#else
#define __pte(x) ({ unsigned long long _x = (x);        \
    ((pte_t) {(unsigned long)(_x), (unsigned long)(_x>>32)}); })
#endif

static
shared_info_t *map_shared_info(unsigned long pa)
{
    int rc;

	if ( (rc = HYPERVISOR_update_va_mapping(
              (unsigned long)_minios_shared_info, __pte(pa | 7), UVMF_INVLPG)) )
	{
		minios_printk("Failed to map shared_info!! rc=%d\n", rc);
		minios_do_exit();
	}
	return (shared_info_t *)_minios_shared_info;
}

void
arch_init(start_info_t *si)
{
	/* Copy the start_info struct to a globally-accessible area. */
	/* WARN: don't do printk before here, it uses information from
	   shared_info. Use xprintk instead. */
	bmk_memcpy(&start_info, si, sizeof(*si));

	/* set up minimal memory infos */
	_minios_phys_to_machine_mapping = (unsigned long *)start_info.mfn_list;

	/* Grab the shared_info pointer and put it in a safe place. */
	HYPERVISOR_shared_info = map_shared_info(start_info.shared_info);

	    /* Set up event and failsafe callback addresses. */
#ifdef __i386__
	gdtinit32();
	HYPERVISOR_set_callbacks(
		__KERNEL_CS, (unsigned long)_minios_entry_hypervisor_callback,
		__KERNEL_CS, (unsigned long)_minios_entry_failsafe_callback);
#else
	HYPERVISOR_set_callbacks(
		(unsigned long)_minios_entry_hypervisor_callback,
		(unsigned long)_minios_entry_failsafe_callback, 0);
#endif

}

void
arch_fini(void)
{
#ifdef __i386__
	HYPERVISOR_set_callbacks(0, 0, 0, 0);
#else
	HYPERVISOR_set_callbacks(0, 0, 0);
#endif
}

void
arch_print_info(void)
{
	minios_printk("  stack:      %p-%p\n", _minios_stack,
                _minios_stack + sizeof(_minios_stack));
}


