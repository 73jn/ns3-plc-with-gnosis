/*-
 * Copyright (c) 2000 Peter Wemm <peter@FreeBSD.org>
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
 * $FreeBSD: src/sys/i386/include/legacyvar.h,v 1.5 2004/05/03 14:49:43 jhb Exp $
 */

#ifndef _MACHINE_LEGACYVAR_H_
#define	_MACHINE_LEGACYVAR_H_

enum legacy_device_ivars {
	LEGACY_IVAR_PCIBUS
};

#define LEGACY_ACCESSOR(var, ivar, type)				\
    __BUS_ACCESSOR(legacy, var, LEGACY, ivar, type)

LEGACY_ACCESSOR(pcibus,			PCIBUS,		u_int32_t)

#undef LEGACY_ACCESSOR

int	legacy_pcib_maxslots(device_t dev);
u_int32_t legacy_pcib_read_config(device_t dev, int bus, int slot, int func,
    int reg, int bytes);
int	legacy_pcib_read_ivar(device_t dev, device_t child, int which,
    uintptr_t *result);
void	legacy_pcib_write_config(device_t dev, int bus, int slot, int func,
    int reg, u_int32_t data, int bytes);
int	legacy_pcib_write_ivar(device_t dev, device_t child, int which,
    uintptr_t value);

#endif /* !_MACHINE_LEGACYVAR_H_ */
