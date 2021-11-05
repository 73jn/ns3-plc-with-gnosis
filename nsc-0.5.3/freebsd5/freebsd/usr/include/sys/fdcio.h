/*
 * Copyright (C) 1992-1994,2001 by Joerg Wunsch, Dresden
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR(S) ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR(S) BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 *
 * $FreeBSD: src/sys/sys/fdcio.h,v 1.17 2001/12/15 19:07:58 joerg Exp $
 */

#ifndef	_MACHINE_IOCTL_FD_H_
#define	_MACHINE_IOCTL_FD_H_

#ifndef _KERNEL
#include <sys/types.h>
#endif
#include <sys/ioccom.h>

#define FD_FORMAT_VERSION 110	/* used to validate before formatting */
#define FD_MAX_NSEC 36		/* highest known number of spt - allow for */
				/* 2.88 MB drives */

struct fd_formb {
	int format_version;	/* == FD_FORMAT_VERSION */
	int cyl, head;
	int transfer_rate;	/* FDC_???KBPS */

	union {
		struct fd_form_data {
			/*
			 * DO NOT CHANGE THE LAYOUT OF THIS STRUCTS
			 * it is hardware-dependent since it exactly
			 * matches the byte sequence to write to FDC
			 * during its `format track' operation
			 */
			u_char secshift; /* 0 -> 128, ...; usually 2 -> 512 */
			u_char nsecs;	/* must be <= FD_MAX_NSEC */
			u_char gaplen;	/* GAP 3 length; usually 84 */
			u_char fillbyte; /* usually 0xf6 */
			struct fd_idfield_data {
				/*
				 * data to write into id fields;
				 * for obscure formats, they mustn't match
				 * the real values (but mostly do)
				 */
				u_char cylno;	/* 0 thru 79 (or 39) */
				u_char headno;	/* 0, or 1 */
				u_char secno;	/* starting at 1! */
				u_char secsize;	/* usually 2 */
			} idfields[FD_MAX_NSEC]; /* 0 <= idx < nsecs used */
		} structured;
		u_char raw[1];	/* to have continuous indexed access */
	} format_info;
};

/* make life easier */
# define fd_formb_secshift   format_info.structured.secshift
# define fd_formb_nsecs      format_info.structured.nsecs
# define fd_formb_gaplen     format_info.structured.gaplen
# define fd_formb_fillbyte   format_info.structured.fillbyte
/* these data must be filled in for(i = 0; i < fd_formb_nsecs; i++) */
# define fd_formb_cylno(i)   format_info.structured.idfields[i].cylno
# define fd_formb_headno(i)  format_info.structured.idfields[i].headno
# define fd_formb_secno(i)   format_info.structured.idfields[i].secno
# define fd_formb_secsize(i) format_info.structured.idfields[i].secsize

struct fd_type {
	int	sectrac;		/* sectors per track         */
	int	secsize;		/* size code for sectors     */
	int	datalen;		/* data len when secsize = 0 */
	int	gap;			/* gap len between sectors   */
	int	tracks;			/* total number of cylinders */
	int	size;			/* size of disk in sectors   */
	int	trans;			/* transfer speed code       */
	int	heads;			/* number of heads	     */
	int     f_gap;                  /* format gap len            */
	int     f_inter;                /* format interleave factor  */
	int	offset_side2;		/* offset of sectors on side2 */
	int	flags;			/* misc. features */
#define FL_MFM		0x0001		/* MFM recording */
#define FL_2STEP	0x0002		/* 2 steps between cylinders */
#define FL_PERPND	0x0004		/* perpendicular recording */
};

struct fdc_status {
	u_int	status[7];
};

/*
 * cyl and head are being passed into ioctl(FD_READID)
 * all four fields are being returned
 */
struct fdc_readid {
	u_char	cyl;		/* C - 0...79 */
	u_char	head;		/* H - 0...1 */
	u_char	sec;		/* R - 1...n */
	u_char	secshift;	/* N - log2(secsize / 128) */
};

/*
 * Diskette drive type, basically the same as stored in RTC on ISA
 * machines (see /sys/isa/rtc.h), but right-shifted by four bits.
 */
enum fd_drivetype {
	FDT_NONE, FDT_360K, FDT_12M, FDT_720K, FDT_144M, FDT_288M_1,
	FDT_288M
};


#define FD_FORM   _IOW('F', 61, struct fd_formb) /* format a track */
#define FD_GTYPE  _IOR('F', 62, struct fd_type)  /* get drive type */
#define FD_STYPE  _IOW('F', 63, struct fd_type)  /* set drive type */

#define FD_GOPTS  _IOR('F', 64, int) /* drive options, see below */
#define FD_SOPTS  _IOW('F', 65, int)

#define FD_DEBUG  _IOW('F', 66, int)

#define FD_CLRERR _IO('F', 67)	/* clear error counter */

#define FD_READID _IOWR('F', 68, struct fdc_readid) /* read ID field */

/*
 * Obtain NE765 status registers.  Only successful if there is
 * a valid status stored in fdc->status[].
 */
#define FD_GSTAT  _IOR('F', 69, struct fdc_status)

#define FD_GDTYPE _IOR('F', 70, enum fd_drivetype) /* obtain drive type */

/* Options for FD_GOPTS/FD_SOPTS, cleared on device close */
#define FDOPT_NORETRY 0x0001	/* no retries on failure */
#define FDOPT_NOERRLOG 0x002	/* no "hard error" kernel log messages */
#define FDOPT_NOERROR 0x0004	/* do not indicate errors, caller will use
				   FD_GSTAT in order to obtain status */
#define FDOPT_AUTOSEL 0x8000	/* read/only option: device performs media
				 * autoselection */

/*
 * Transfer rate definitions.  Used in the structures above.  They
 * represent the hardware encoding of bits 0 and 1 of the FDC control
 * register when writing to the register.
 * Transfer rates for FM encoding are half the values listed here
 * (but we currently don't support FM encoding).
 */
#define	FDC_500KBPS	0x00	/* 500KBPS MFM drive transfer rate */
#define	FDC_300KBPS	0x01	/* 300KBPS MFM drive transfer rate */
#define	FDC_250KBPS	0x02	/* 250KBPS MFM drive transfer rate */
#define	FDC_1MBPS	0x03	/* 1MPBS MFM drive transfer rate */

#endif /* !_MACHINE_IOCTL_FD_H_ */
