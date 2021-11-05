/*-
 * Copyright (c) 2002 Mitsuru IWASAKI
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $FreeBSD: src/sys/i386/include/acpica_machdep.h,v 1.1 2002/07/30 19:35:31 iwasaki Exp $
 */

/******************************************************************************
 *
 * Name: acpica_machdep.h - arch-specific defines, etc.
 *       $Revision: 1.1 $
 *
 *****************************************************************************/

#ifndef __ACPICA_MACHDEP_H__
#define __ACPICA_MACHDEP_H__

#ifdef _KERNEL
/*
 * Calling conventions:
 *
 * ACPI_SYSTEM_XFACE        - Interfaces to host OS (handlers, threads)
 * ACPI_EXTERNAL_XFACE      - External ACPI interfaces 
 * ACPI_INTERNAL_XFACE      - Internal ACPI interfaces
 * ACPI_INTERNAL_VAR_XFACE  - Internal variable-parameter list interfaces
 */
#define ACPI_SYSTEM_XFACE
#define ACPI_EXTERNAL_XFACE
#define ACPI_INTERNAL_XFACE
#define ACPI_INTERNAL_VAR_XFACE

/* Asm macros */

#define ACPI_ASM_MACROS
#define BREAKPOINT3
#define ACPI_DISABLE_IRQS() disable_intr()
#define ACPI_ENABLE_IRQS()  enable_intr()

#define ACPI_FLUSH_CPU_CACHE()	wbinvd()

#define asm         __asm
/*! [Begin] no source code translation
 *
 * A brief explanation as GNU inline assembly is a bit hairy
 *  %0 is the output parameter in EAX ("=a")
 *  %1 and %2 are the input parameters in ECX ("c")
 *  and an immediate value ("i") respectively
 *  All actual register references are preceded with "%%" as in "%%edx"
 *  Immediate values in the assembly are preceded by "$" as in "$0x1"
 *  The final asm parameter are the operation altered non-output registers.
 */
#define ACPI_ACQUIRE_GLOBAL_LOCK(GLptr, Acq) \
    do { \
        int dummy; \
        asm("1:     movl (%1),%%eax;" \
            "movl   %%eax,%%edx;" \
            "andl   %2,%%edx;" \
            "btsl   $0x1,%%edx;" \
            "adcl   $0x0,%%edx;" \
            "lock;  cmpxchgl %%edx,(%1);" \
            "jnz    1b;" \
            "cmpb   $0x3,%%dl;" \
            "sbbl   %%eax,%%eax" \
            :"=a"(Acq),"=c"(dummy):"c"(GLptr),"i"(~1L):"dx"); \
    } while(0)

#define ACPI_RELEASE_GLOBAL_LOCK(GLptr, Acq) \
    do { \
        int dummy; \
        asm("1:     movl (%1),%%eax;" \
            "movl   %%eax,%%edx;" \
            "andl   %2,%%edx;" \
            "lock;  cmpxchgl %%edx,(%1);" \
            "jnz    1b;" \
            "andl   $0x1,%%eax" \
            :"=a"(Acq),"=c"(dummy):"c"(GLptr),"i"(~3L):"dx"); \
    } while(0)


/*
 * Math helper asm macros
 */
#define ACPI_DIV_64_BY_32(n_hi, n_lo, d32, q32, r32) \
        asm("divl %2;"        \
        :"=a"(q32), "=d"(r32) \
        :"r"(d32),            \
        "0"(n_lo), "1"(n_hi))


#define ACPI_SHIFT_RIGHT_64(n_hi, n_lo) \
    asm("shrl   $1,%2;"             \
        "rcrl   $1,%3;"             \
        :"=r"(n_hi), "=r"(n_lo)     \
        :"0"(n_hi), "1"(n_lo))

/*! [End] no source code translation !*/
#endif /* _KERNEL */

#define ACPI_MACHINE_WIDTH             32
#define COMPILER_DEPENDENT_INT64       long long
#define COMPILER_DEPENDENT_UINT64      unsigned long long
#define ACPI_USE_NATIVE_DIVIDE

#endif /* __ACPICA_MACHDEP_H__ */
