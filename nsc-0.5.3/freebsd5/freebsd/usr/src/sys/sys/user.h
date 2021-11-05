/*-
 * Copyright (c) 1982, 1986, 1989, 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)user.h	8.2 (Berkeley) 9/23/93
 * $FreeBSD: src/sys/sys/user.h,v 1.61.2.2 2005/02/05 01:02:09 das Exp $
 */

#ifndef _SYS_USER_H_
#define _SYS_USER_H_

#include <machine/pcb.h>
#ifndef _KERNEL
/* stuff that *used* to be included by user.h, or is now needed */
#include <sys/errno.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/ucred.h>
#include <sys/uio.h>
#include <sys/queue.h>
#include <sys/_lock.h>
#include <sys/_mutex.h>
#include <sys/proc.h>
#include <vm/vm.h>		/* XXX */
#include <vm/vm_param.h>	/* XXX */
#include <vm/pmap.h>		/* XXX */
#include <vm/vm_map.h>		/* XXX */
#endif /* !_KERNEL */
#ifndef _SYS_RESOURCEVAR_H_
#include <sys/resourcevar.h>
#endif
#ifndef _SYS_SIGNALVAR_H_
#include <sys/signalvar.h>
#endif

/*
 * KERN_PROC subtype ops return arrays of selected proc structure entries:
 *
 * When adding new fields to this structure, ALWAYS add them at the end
 * and decrease the size of the spare field by the amount of space that
 * you are adding.  Byte aligned data should be added to the ki_sparestring
 * space; other entries should be added to the ki_spare space. Always
 * verify that sizeof(struct kinfo_proc) == KINFO_PROC_SIZE when you are
 * done. If you change the size of this structure, many programs will stop
 * working! Once you have added the new field, you will need to add code
 * to initialize it in two places: kern/kern_proc.c in the function
 * fill_kinfo_proc and in lib/libkvm/kvm_proc.c in the function kvm_proclist.
 *
 * KI_NSPARE is the number of spare-longs to define in the array at the
 * end of kinfo_proc.  It may need to be overridden on a platform-specific
 * basis as new fields are added.
 */
#define	KI_NSPARE	16

#ifdef __alpha__
#define	KINFO_PROC_SIZE	912
#endif
#ifdef __amd64__
#define	KINFO_PROC_SIZE	912
#endif
#ifdef __arm__
#undef KI_NSPARE			/* Fewer spare longs on this arch */
#define	KI_NSPARE	15
#define	KINFO_PROC_SIZE	648
#endif
#ifdef __ia64__
#define	KINFO_PROC_SIZE 912
#endif
#ifdef __i386__
#undef KI_NSPARE			/* Fewer spare longs on this arch */
#define	KI_NSPARE	15
#define	KINFO_PROC_SIZE	648
#endif
#ifdef __powerpc__
#define	KINFO_PROC_SIZE	656
#endif
#ifdef __sparc64__
#define	KINFO_PROC_SIZE 912
#endif
#ifndef KINFO_PROC_SIZE
#error "Unknown architecture"
#endif

#define	WMESGLEN	8		/* size of returned wchan message */
#define	LOCKNAMELEN	8		/* size of returned lock name */
#define	OCOMMLEN	16		/* size of returned ki_ocomm name */
#define	COMMLEN		19		/* size of returned ki_comm name */
#define	KI_EMULNAMELEN	16		/* size of returned ki_emul */
#define	KI_NGROUPS	16		/* number of groups in ki_groups */
#define	LOGNAMELEN	17		/* size of returned ki_login */

struct kinfo_proc {
	int	ki_structsize;		/* size of this structure */
	int	ki_layout;		/* reserved: layout identifier */
	struct	pargs *ki_args;		/* address of command arguments */
	struct	proc *ki_paddr;		/* address of proc */
	struct	user *ki_addr;		/* kernel virtual addr of u-area */
	struct	vnode *ki_tracep;	/* pointer to trace file */
	struct	vnode *ki_textvp;	/* pointer to executable file */
	struct	filedesc *ki_fd;	/* pointer to open file info */
	struct	vmspace *ki_vmspace;	/* pointer to kernel vmspace struct */
	void	*ki_wchan;		/* sleep address */
	pid_t	ki_pid;			/* Process identifier */
	pid_t	ki_ppid;		/* parent process id */
	pid_t	ki_pgid;		/* process group id */
	pid_t	ki_tpgid;		/* tty process group id */
	pid_t	ki_sid;			/* Process session ID */
	pid_t	ki_tsid;		/* Terminal session ID */
	short	ki_jobc;		/* job control counter */
	short	ki_spare_short1;	/* unused (just here for alignment) */
	dev_t	ki_tdev;		/* controlling tty dev */
	sigset_t ki_siglist;		/* Signals arrived but not delivered */
	sigset_t ki_sigmask;		/* Current signal mask */
	sigset_t ki_sigignore;		/* Signals being ignored */
	sigset_t ki_sigcatch;		/* Signals being caught by user */
	uid_t	ki_uid;			/* effective user id */
	uid_t	ki_ruid;		/* Real user id */
	uid_t	ki_svuid;		/* Saved effective user id */
	gid_t	ki_rgid;		/* Real group id */
	gid_t	ki_svgid;		/* Saved effective group id */
	short	ki_ngroups;		/* number of groups */
	short	ki_spare_short2;	/* unused (just here for alignment) */
	gid_t	ki_groups[KI_NGROUPS];	/* groups */
	vm_size_t ki_size;		/* virtual size */
	segsz_t ki_rssize;		/* current resident set size in pages */
	segsz_t ki_swrss;		/* resident set size before last swap */
	segsz_t ki_tsize;		/* text size (pages) XXX */
	segsz_t ki_dsize;		/* data size (pages) XXX */
	segsz_t ki_ssize;		/* stack size (pages) */
	u_short	ki_xstat;		/* Exit status for wait & stop signal */
	u_short	ki_acflag;		/* Accounting flags */
	fixpt_t	ki_pctcpu;	 	/* %cpu for process during ki_swtime */
	u_int	ki_estcpu;	 	/* Time averaged value of ki_cpticks */
	u_int	ki_slptime;	 	/* Time since last blocked */
	u_int	ki_swtime;	 	/* Time swapped in or out */
	u_int64_t ki_runtime;		/* Real time in microsec */
	struct	timeval ki_start;	/* starting time */
	struct	timeval ki_childtime;	/* time used by process children */
	long	ki_flag;		/* P_* flags */
	long	ki_kiflag;		/* KI_* flags (below) */
	int	ki_traceflag;		/* Kernel trace points */
	char	ki_stat;		/* S* process status */
	signed char ki_nice;		/* Process "nice" value */
	char	ki_lock;		/* Process lock (prevent swap) count */
	char	ki_rqindex;		/* Run queue index */
	u_char	ki_oncpu;		/* Which cpu we are on */
	u_char	ki_lastcpu;		/* Last cpu we were on */
	char	ki_ocomm[OCOMMLEN+1];	/* command name */
	char	ki_wmesg[WMESGLEN+1];	/* wchan message */
	char	ki_login[LOGNAMELEN+1];	/* setlogin name */
	char	ki_lockname[LOCKNAMELEN+1]; /* lock name */
	char	ki_comm[COMMLEN+1];	/* command name */
	char	ki_emul[KI_EMULNAMELEN+1];  /* emulation name */
	char	ki_sparestrings[68];	/* spare string space */
	struct	rusage ki_rusage;	/* process rusage statistics */
	long	ki_sflag;		/* PS_* flags */
	struct	priority ki_pri;	/* process priority */
	long	ki_tdflags;		/* XXXKSE kthread flag */
	struct	pcb *ki_pcb;		/* kernel virtual addr of pcb */
	void	*ki_kstack;		/* kernel virtual addr of stack */
	struct	timeval ki_childstime;	/* system time used by children */
	struct	timeval ki_childutime;	/* user time used by children */
	lwpid_t	ki_tid;			/* XXXKSE thread id */
	int	ki_numthreads;		/* XXXKSE number of threads in total */
	void	*ki_udata;		/* User convenience pointer */
	long	ki_spare[KI_NSPARE];	/* spare room for later growth */
};
void fill_kinfo_proc(struct proc *, struct kinfo_proc *);

/* ki_sessflag values */
#define	KI_CTTY		0x00000001	/* controlling tty vnode active */
#define	KI_SLEADER	0x00000002	/* session leader */
#define	KI_LOCKBLOCK	0x00000004	/* proc blocked on lock ki_lockname */

/*
 * This used to be the per-process structure containing data that
 * isn't needed in core when the process is swapped out, but now it
 * remains only for the benefit of a.out core dumps.
 */
struct user {
	struct	pstats u_stats;		/* *p_stats */
	struct	kinfo_proc u_kproc;	/* eproc */
};

#endif
