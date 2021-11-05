/*-
 * Copyright (c) 2002 Matthew Dillon.  This code is distributed under
 * the BSD copyright, /usr/src/COPYRIGHT.
 *
 * This file contains prototypes and high-level inlines related to
 * machine-level critical function support:
 *
 *	cpu_critical_enter()		- inlined
 *	cpu_critical_exit()		- inlined
 *	cpu_critical_fork_exit()	- prototyped
 *	cpu_thread_link()		- prototyped
 *	related support functions residing
 *	in <arch>/<arch>/critical.c	- prototyped
 *
 * $FreeBSD: src/sys/i386/include/critical.h,v 1.2 2002/07/10 20:15:58 dillon Exp $
 */

#ifndef _MACHINE_CRITICAL_H_
#define	_MACHINE_CRITICAL_H_

__BEGIN_DECLS

/*
 * Prototypes - see <arch>/<arch>/critical.c
 */
void cpu_unpend(void);
void cpu_critical_fork_exit(void);
void cpu_thread_link(struct thread *td);

#ifdef	__GNUC__

/*
 *	cpu_critical_enter:
 *
 *	This routine is called from critical_enter() on the 0->1 transition
 *	of td_critnest, prior to it being incremented to 1.
 *
 *	If new-style critical section handling we do not have to do anything.
 *	However, as a side effect any interrupts occuring while td_critnest
 *	is non-zero will be deferred.
 */
#define cpu_critical_enter()

/*
 *	cpu_critical_exit:
 *
 *	This routine is called from critical_exit() on a 1->0 transition
 *	of td_critnest, after it has been decremented to 0.  We are
 *	exiting the last critical section.
 *
 *	Note that the td->critnest (1->0) transition interrupt race against
 *	our int_pending/unpend() check below is handled by the interrupt
 *	code for us, so we do not have to do anything fancy.
 */
static __inline void
cpu_critical_exit(void)
{
	/*
	 * We may have to schedule pending interrupts.  Create
	 * conditions similar to an interrupt context and call
	 * unpend().
	 *
	 * note: we do this even if we are in an interrupt
	 * nesting level.  Deep nesting is protected by
	 * critical_*() and if we conditionalized it then we
	 * would have to check int_pending again whenever
	 * we decrement td_intr_nesting_level to 0.
	 */
	if (PCPU_GET(int_pending))
		cpu_unpend();
}

#else /* !__GNUC__ */

void cpu_critical_enter(void)
void cpu_critical_exit(void)

#endif	/* __GNUC__ */

__END_DECLS

#endif /* !_MACHINE_CRITICAL_H_ */

