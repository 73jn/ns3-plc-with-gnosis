/*-
 * Copyright (c) 1999-2002 Poul-Henning Kamp
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
 */

#include <sys/cdefs.h>
__FBSDID("$FreeBSD: src/sys/kern/kern_conf.c,v 1.155 2004/08/15 06:24:40 jmg Exp $");

#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/systm.h>
#include <sys/bio.h>
#include <sys/lock.h>
#include <sys/mutex.h>
#include <sys/sysctl.h>
#include <sys/module.h>
#include <sys/malloc.h>
#include <sys/conf.h>
#include <sys/vnode.h>
#include <sys/queue.h>
#include <sys/poll.h>
#include <sys/ctype.h>
#include <sys/tty.h>
#include <machine/stdarg.h>

static MALLOC_DEFINE(M_DEVT, "cdev", "cdev storage");

/* Built at compile time from sys/conf/majors */
extern unsigned char reserved_majors[256];

/*
 * This is the number of hash-buckets.  Experiments with 'real-life'
 * dev_t's show that a prime halfway between two powers of two works
 * best.
 */
#define DEVT_HASH 83

/* The number of struct cdev *'s we can create before malloc(9) kick in.  */
#define DEVT_STASH 50

static struct cdev devt_stash[DEVT_STASH];

static LIST_HEAD(, cdev) dev_hash[DEVT_HASH];

static LIST_HEAD(, cdev) dev_free;

static int free_devt;
SYSCTL_INT(_debug, OID_AUTO, free_devt, CTLFLAG_RW, &free_devt, 0, "");

static struct mtx devmtx;
static void freedev(struct cdev *dev);
static struct cdev *newdev(int x, int y);


static void
devlock(void)
{
	if (!mtx_initialized(&devmtx))
		mtx_init(&devmtx, "cdev", NULL, MTX_DEF);
	mtx_lock(&devmtx);
}

static void
devunlock(void)
{
	mtx_unlock(&devmtx);
}

void
dev_ref(struct cdev *dev)
{
	devlock();
	dev->si_refcount++;
	devunlock();
}

void
dev_rel(struct cdev *dev)
{
	devlock();
	dev->si_refcount--;
	KASSERT(dev->si_refcount >= 0,
	    ("dev_rel(%s) gave negative count", devtoname(dev)));
	if (dev->si_devsw == NULL && dev->si_refcount == 0) {
		LIST_REMOVE(dev, si_list);
		freedev(dev);
	}
	devunlock();
}

void
cdevsw_ref(struct cdevsw *csw)
{
	devlock();
	csw->d_refcount++;
	devunlock();
}

void
cdevsw_rel(struct cdevsw *csw)
{
	devlock();
	csw->d_refcount--;
	KASSERT(csw->d_refcount >= 0,
	    ("cdevsw_vrel(%s) gave negative count", csw->d_name));
	devunlock();
}

int
nullop(void)
{

	return (0);
}

int
eopnotsupp(void)
{

	return (EOPNOTSUPP);
}

static int
enxio(void)
{
	return (ENXIO);
}

static int
enodev(void)
{
	return (ENODEV);
}

/* Define a dead_cdevsw for use when devices leave unexpectedly. */

#define dead_open	(d_open_t *)enxio
#define dead_close	(d_close_t *)enxio
#define dead_read	(d_read_t *)enxio
#define dead_write	(d_write_t *)enxio
#define dead_ioctl	(d_ioctl_t *)enxio
#define dead_poll	(d_poll_t *)enodev
#define dead_mmap	(d_mmap_t *)enodev

static void
dead_strategy(struct bio *bp)
{

	biofinish(bp, NULL, ENXIO);
}

#define dead_dump	(dumper_t *)enxio
#define dead_kqfilter	(d_kqfilter_t *)enxio

static struct cdevsw dead_cdevsw = {
	.d_version =	D_VERSION,
	.d_flags =	D_NEEDGIANT, /* XXX: does dead_strategy need this ? */
	.d_open =	dead_open,
	.d_close =	dead_close,
	.d_read =	dead_read,
	.d_write =	dead_write,
	.d_ioctl =	dead_ioctl,
	.d_poll =	dead_poll,
	.d_mmap =	dead_mmap,
	.d_strategy =	dead_strategy,
	.d_name =	"dead",
	.d_maj =	255,
	.d_dump =	dead_dump,
	.d_kqfilter =	dead_kqfilter
};

/* Default methods if driver does not specify method */

#define null_open	(d_open_t *)nullop
#define null_close	(d_close_t *)nullop
#define no_read		(d_read_t *)enodev
#define no_write	(d_write_t *)enodev
#define no_ioctl	(d_ioctl_t *)enodev
#define no_mmap		(d_mmap_t *)enodev
#define no_kqfilter	(d_kqfilter_t *)enodev

static void
no_strategy(struct bio *bp)
{

	biofinish(bp, NULL, ENODEV);
}

static int
no_poll(struct cdev *dev __unused, int events, struct thread *td __unused)
{
	/*
	 * Return true for read/write.  If the user asked for something
	 * special, return POLLNVAL, so that clients have a way of
	 * determining reliably whether or not the extended
	 * functionality is present without hard-coding knowledge
	 * of specific filesystem implementations.
	 * Stay in sync with vop_nopoll().
	 */
	if (events & ~POLLSTANDARD)
		return (POLLNVAL);

	return (events & (POLLIN | POLLOUT | POLLRDNORM | POLLWRNORM));
}

#define no_dump		(dumper_t *)enodev

struct cdevsw *
devsw(struct cdev *dev)
{
	if (dev->si_devsw != NULL)
		return (dev->si_devsw);
	return (&dead_cdevsw);
}

/*
 * struct cdev * and u_dev_t primitives
 */

int
major(struct cdev *x)
{
	if (x == NULL)
		return NODEV;
	return((x->si_udev >> 8) & 0xff);
}

int
minor(struct cdev *x)
{
	if (x == NULL)
		return NODEV;
	return(x->si_udev & 0xffff00ff);
}

int
dev2unit(struct cdev *x)
{
	int i;

	if (x == NULL)
		return NODEV;
	i = minor(x);
	return ((i & 0xff) | (i >> 8));
}

int
unit2minor(int unit)
{

	KASSERT(unit <= 0xffffff, ("Invalid unit (%d) in unit2minor", unit));
	return ((unit & 0xff) | ((unit << 8) & ~0xffff));
}

static struct cdev *
allocdev(void)
{
	static int stashed;
	struct cdev *si;

	if (LIST_FIRST(&dev_free)) {
		si = LIST_FIRST(&dev_free);
		LIST_REMOVE(si, si_hash);
	} else if (stashed >= DEVT_STASH) {
		MALLOC(si, struct cdev *, sizeof(*si), M_DEVT,
		    M_USE_RESERVE | M_ZERO | M_WAITOK);
	} else {
		si = devt_stash + stashed++;
		bzero(si, sizeof *si);
		si->si_flags |= SI_STASHED;
	}
	si->__si_namebuf[0] = '\0';
	si->si_name = si->__si_namebuf;
	LIST_INIT(&si->si_children);
	TAILQ_INIT(&si->si_snapshots);
	return (si);
}

static struct cdev *
newdev(int x, int y)
{
	struct cdev *si;
	dev_t	udev;
	int hash;

	if (x == umajor(NODEV) && y == uminor(NODEV))
		panic("newdev of NODEV");
	udev = (x << 8) | y;
	hash = udev % DEVT_HASH;
	LIST_FOREACH(si, &dev_hash[hash], si_hash) {
		if (si->si_udev == udev)
			return (si);
	}
	si = allocdev();
	si->si_udev = udev;
	LIST_INSERT_HEAD(&dev_hash[hash], si, si_hash);
        return (si);
}

static void
freedev(struct cdev *dev)
{

	if (dev->si_flags & SI_STASHED) {
		bzero(dev, sizeof(*dev));
		dev->si_flags |= SI_STASHED;
		LIST_INSERT_HEAD(&dev_free, dev, si_hash);
	} else {
		FREE(dev, M_DEVT);
	}
}

dev_t
dev2udev(struct cdev *x)
{
	if (x == NULL)
		return (NODEV);
	return (x->si_udev);
}

struct cdev *
findcdev(dev_t udev)
{
	struct cdev *si;
	int hash;

	if (udev == NODEV)
		return (NULL);
	hash = udev % DEVT_HASH;
	LIST_FOREACH(si, &dev_hash[hash], si_hash) {
		if (si->si_udev == udev)
			return (si);
	}
	return (NULL);
}

int
uminor(dev_t dev)
{
	return (dev & 0xffff00ff);
}

int
umajor(dev_t dev)
{
	return ((dev & 0xff00) >> 8);
}

static void
find_major(struct cdevsw *devsw)
{
	int i;

	for (i = NUMCDEVSW - 1; i > 0; i--)
		if (reserved_majors[i] != i)
			break;
	KASSERT(i > 0, ("Out of major numbers (%s)", devsw->d_name));
	devsw->d_maj = i;
	reserved_majors[i] = i;
	devsw->d_flags |= D_ALLOCMAJ;
}

static void
fini_cdevsw(struct cdevsw *devsw)
{
	if (devsw->d_flags & D_ALLOCMAJ) {
		reserved_majors[devsw->d_maj] = 0;
		devsw->d_maj = MAJOR_AUTO;
		devsw->d_flags &= ~D_ALLOCMAJ;
	} else if (devsw->d_maj == 0)
		devsw->d_maj = 256;
	devsw->d_flags &= ~D_INIT;
}

static void
prep_cdevsw(struct cdevsw *devsw)
{

	devlock();

	if (devsw->d_version != D_VERSION_00) {
		printf(
		    "WARNING: Device driver \"%s\" has wrong version %s\n",
		    devsw->d_name, "and is disabled.  Recompile KLD module.");
		devsw->d_open = dead_open;
		devsw->d_close = dead_close;
		devsw->d_read = dead_read;
		devsw->d_write = dead_write;
		devsw->d_ioctl = dead_ioctl;
		devsw->d_poll = dead_poll;
		devsw->d_mmap = dead_mmap;
		devsw->d_strategy = dead_strategy;
		devsw->d_dump = dead_dump;
		devsw->d_kqfilter = dead_kqfilter;
	}
	
	if (devsw->d_flags & D_TTY) {
		if (devsw->d_ioctl == NULL)	devsw->d_ioctl = ttyioctl;
		if (devsw->d_read == NULL)	devsw->d_read = ttyread;
		if (devsw->d_write == NULL)	devsw->d_write = ttywrite;
		if (devsw->d_kqfilter == NULL)	devsw->d_kqfilter = ttykqfilter;
		if (devsw->d_poll == NULL)	devsw->d_poll = ttypoll;
	}

	if (devsw->d_open == NULL)	devsw->d_open = null_open;
	if (devsw->d_close == NULL)	devsw->d_close = null_close;
	if (devsw->d_read == NULL)	devsw->d_read = no_read;
	if (devsw->d_write == NULL)	devsw->d_write = no_write;
	if (devsw->d_ioctl == NULL)	devsw->d_ioctl = no_ioctl;
	if (devsw->d_poll == NULL)	devsw->d_poll = no_poll;
	if (devsw->d_mmap == NULL)	devsw->d_mmap = no_mmap;
	if (devsw->d_strategy == NULL)	devsw->d_strategy = no_strategy;
	if (devsw->d_dump == NULL)	devsw->d_dump = no_dump;
	if (devsw->d_kqfilter == NULL)	devsw->d_kqfilter = no_kqfilter;

	LIST_INIT(&devsw->d_devs);

	devsw->d_flags |= D_INIT;

	if (devsw->d_maj == MAJOR_AUTO) {
		find_major(devsw);
	} else {
		if (devsw->d_maj == 256)	/* XXX: tty_cons.c is magic */
			devsw->d_maj = 0;	
		KASSERT(devsw->d_maj >= 0 && devsw->d_maj < 256,
		    ("Invalid major (%d) in make_dev", devsw->d_maj));
		if (reserved_majors[devsw->d_maj] != devsw->d_maj) {
			printf("WARNING: driver \"%s\" used %s %d\n",
			    devsw->d_name, "unreserved major device number",
			    devsw->d_maj);
			reserved_majors[devsw->d_maj] = devsw->d_maj;
		}
	}
	devunlock();
}

struct cdev *
make_dev(struct cdevsw *devsw, int minornr, uid_t uid, gid_t gid, int perms, const char *fmt, ...)
{
	struct cdev *dev;
	va_list ap;
	int i;

	KASSERT((minornr & ~0xffff00ff) == 0,
	    ("Invalid minor (0x%x) in make_dev", minornr));

	if (!(devsw->d_flags & D_INIT))
		prep_cdevsw(devsw);
	dev = newdev(devsw->d_maj, minornr);
	if (dev->si_flags & SI_CHEAPCLONE &&
	    dev->si_flags & SI_NAMED &&
	    dev->si_devsw == devsw) {
		/*
		 * This is allowed as it removes races and generally
		 * simplifies cloning devices.
		 * XXX: still ??
		 */
		return (dev);
	}
	devlock();
	KASSERT(!(dev->si_flags & SI_NAMED),
	    ("make_dev() by driver %s on pre-existing device (maj=%d, min=%d, name=%s)",
	    devsw->d_name, major(dev), minor(dev), devtoname(dev)));

	va_start(ap, fmt);
	i = vsnrprintf(dev->__si_namebuf, sizeof dev->__si_namebuf, 32, fmt, ap);
	if (i > (sizeof dev->__si_namebuf - 1)) {
		printf("WARNING: Device name truncated! (%s)", 
		    dev->__si_namebuf);
	}
	va_end(ap);
	dev->si_devsw = devsw;
	dev->si_uid = uid;
	dev->si_gid = gid;
	dev->si_mode = perms;
	dev->si_flags |= SI_NAMED;

	LIST_INSERT_HEAD(&devsw->d_devs, dev, si_list);
	devfs_create(dev);
	devunlock();
	return (dev);
}

int
dev_named(struct cdev *pdev, const char *name)
{
	struct cdev *cdev;

	if (strcmp(devtoname(pdev), name) == 0)
		return (1);
	LIST_FOREACH(cdev, &pdev->si_children, si_siblings)
		if (strcmp(devtoname(cdev), name) == 0)
			return (1);
	return (0);
}

void
dev_depends(struct cdev *pdev, struct cdev *cdev)
{

	devlock();
	cdev->si_parent = pdev;
	cdev->si_flags |= SI_CHILD;
	LIST_INSERT_HEAD(&pdev->si_children, cdev, si_siblings);
	devunlock();
}

struct cdev *
make_dev_alias(struct cdev *pdev, const char *fmt, ...)
{
	struct cdev *dev;
	va_list ap;
	int i;

	dev = allocdev();
	devlock();
	dev->si_flags |= SI_ALIAS;
	dev->si_flags |= SI_NAMED;
	va_start(ap, fmt);
	i = vsnrprintf(dev->__si_namebuf, sizeof dev->__si_namebuf, 32, fmt, ap);
	if (i > (sizeof dev->__si_namebuf - 1)) {
		printf("WARNING: Device name truncated! (%s)", 
		    dev->__si_namebuf);
	}
	va_end(ap);

	devfs_create(dev);
	devunlock();
	dev_depends(pdev, dev);
	return (dev);
}

static void
idestroy_dev(struct cdev *dev)
{
	if (!(dev->si_flags & SI_NAMED)) {
		printf( "WARNING: Driver mistake: destroy_dev on %d/%d\n",
		    major(dev), minor(dev));
		panic("don't do that");
	}
		
	devfs_destroy(dev);

	/* Remove name marking */
	dev->si_flags &= ~SI_NAMED;

	/* If we are a child, remove us from the parents list */
	if (dev->si_flags & SI_CHILD) {
		LIST_REMOVE(dev, si_siblings);
		dev->si_flags &= ~SI_CHILD;
	}

	/* Kill our children */
	while (!LIST_EMPTY(&dev->si_children))
		idestroy_dev(LIST_FIRST(&dev->si_children));

	/* Remove from clone list */
	if (dev->si_flags & SI_CLONELIST) {
		LIST_REMOVE(dev, si_clone);
		dev->si_flags &= ~SI_CLONELIST;
	}

	if (!(dev->si_flags & SI_ALIAS)) {
		/* Remove from cdevsw list */
		LIST_REMOVE(dev, si_list);

		/* If cdevsw has no struct cdev *'s, clean it */
		if (LIST_EMPTY(&dev->si_devsw->d_devs))
			fini_cdevsw(dev->si_devsw);

		LIST_REMOVE(dev, si_hash);
	}
	dev->si_drv1 = 0;
	dev->si_drv2 = 0;
	dev->si_devsw = NULL;
	bzero(&dev->__si_u, sizeof(dev->__si_u));
	dev->si_flags &= ~SI_ALIAS;
	if (dev->si_refcount > 0) {
		LIST_INSERT_HEAD(&dead_cdevsw.d_devs, dev, si_list);
	} else {
	freedev(dev);
	}
}

void
destroy_dev(struct cdev *dev)
{

	devlock();
	idestroy_dev(dev);
	devunlock();
}

const char *
devtoname(struct cdev *dev)
{
	char *p;
	int mynor;

	if (dev->si_name[0] == '#' || dev->si_name[0] == '\0') {
		p = dev->si_name;
		if (devsw(dev))
			sprintf(p, "#%s/", devsw(dev)->d_name);
		else
			sprintf(p, "#%d/", major(dev));
		p += strlen(p);
		mynor = minor(dev);
		if (mynor < 0 || mynor > 255)
			sprintf(p, "%#x", (u_int)mynor);
		else
			sprintf(p, "%d", mynor);
	}
	return (dev->si_name);
}

int
dev_stdclone(char *name, char **namep, const char *stem, int *unit)
{
	int u, i;

	i = strlen(stem);
	if (bcmp(stem, name, i) != 0)
		return (0);
	if (!isdigit(name[i]))
		return (0);
	u = 0;
	if (name[i] == '0' && isdigit(name[i+1]))
		return (0);
	while (isdigit(name[i])) {
		u *= 10;
		u += name[i++] - '0';
	}
	if (u > 0xffffff)
		return (0);
	*unit = u;
	if (namep)
		*namep = &name[i];
	if (name[i]) 
		return (2);
	return (1);
}

/*
 * Helper functions for cloning device drivers.
 *
 * The objective here is to make it unnecessary for the device drivers to
 * use rman or similar to manage their unit number space.  Due to the way
 * we do "on-demand" devices, using rman or other "private" methods 
 * will be very tricky to lock down properly once we lock down this file.
 *
 * Instead we give the drivers these routines which puts the struct cdev *'s
 * that are to be managed on their own list, and gives the driver the ability
 * to ask for the first free unit number or a given specified unit number.
 *
 * In addition these routines support paired devices (pty, nmdm and similar)
 * by respecting a number of "flag" bits in the minor number.
 *
 */

struct clonedevs {
	LIST_HEAD(,cdev)	head;
};

void
clone_setup(struct clonedevs **cdp)
{

	*cdp = malloc(sizeof **cdp, M_DEVBUF, M_WAITOK | M_ZERO);
	LIST_INIT(&(*cdp)->head);
}

int
clone_create(struct clonedevs **cdp, struct cdevsw *csw, int *up, struct cdev **dp, u_int extra)
{
	struct clonedevs *cd;
	struct cdev *dev, *dl, *de;
	int unit, low, u;

	KASSERT(*cdp != NULL,
	    ("clone_setup() not called in driver \"%s\"", csw->d_name));
	KASSERT(!(extra & CLONE_UNITMASK),
	    ("Illegal extra bits (0x%x) in clone_create", extra));
	KASSERT(*up <= CLONE_UNITMASK,
	    ("Too high unit (0x%x) in clone_create", *up));

	if (csw->d_maj == MAJOR_AUTO)
		find_major(csw);

	/*
	 * Search the list for a lot of things in one go:
	 *   A preexisting match is returned immediately.
	 *   The lowest free unit number if we are passed -1, and the place
	 *	 in the list where we should insert that new element.
	 *   The place to insert a specified unit number, if applicable
	 *       the end of the list.
	 */
	unit = *up;
	low = extra;
	de = dl = NULL;
	cd = *cdp;
	LIST_FOREACH(dev, &cd->head, si_clone) {
		u = dev2unit(dev);
		if (u == (unit | extra)) {
			*dp = dev;
			return (0);
		}
		if (unit == -1 && u == low) {
			low++;
			de = dev;
			continue;
		}
		if (u > (unit | extra)) {
			dl = dev;
			break;
		}
		de = dev;
	}
	if (unit == -1)
		unit = low & CLONE_UNITMASK;
	dev = newdev(csw->d_maj, unit2minor(unit | extra));
	KASSERT(!(dev->si_flags & SI_CLONELIST),
	    ("Dev %p should not be on clonelist", dev));
	if (dl != NULL)
		LIST_INSERT_BEFORE(dl, dev, si_clone);
	else if (de != NULL)
		LIST_INSERT_AFTER(de, dev, si_clone);
	else
		LIST_INSERT_HEAD(&cd->head, dev, si_clone);
	dev->si_flags |= SI_CLONELIST;
	*up = unit;
	return (1);
}

/*
 * Kill everything still on the list.  The driver should already have
 * disposed of any softc hung of the struct cdev *'s at this time.
 */
void
clone_cleanup(struct clonedevs **cdp)
{
	struct cdev *dev, *tdev;
	struct clonedevs *cd;
	
	cd = *cdp;
	if (cd == NULL)
		return;
	LIST_FOREACH_SAFE(dev, &cd->head, si_clone, tdev) {
		KASSERT(dev->si_flags & SI_NAMED,
		    ("Driver has goofed in cloning underways udev %x", dev->si_udev));
		destroy_dev(dev);
	}
	free(cd, M_DEVBUF);
	*cdp = NULL;
}

/*
 * Helper sysctl for devname(3).  We're given a struct cdev * and return
 * the name, if any, registered by the device driver.
 */
static int
sysctl_devname(SYSCTL_HANDLER_ARGS)
{
	int error;
	dev_t ud;
	struct cdev *dev;

	error = SYSCTL_IN(req, &ud, sizeof (ud));
	if (error)
		return (error);
	if (ud == NODEV)
		return(EINVAL);
	dev = findcdev(ud);
	if (dev == NULL)
		error = ENOENT;
	else
		error = SYSCTL_OUT(req, dev->si_name, strlen(dev->si_name) + 1);
	return (error);
}

SYSCTL_PROC(_kern, OID_AUTO, devname, CTLTYPE_OPAQUE|CTLFLAG_RW|CTLFLAG_ANYBODY,
	NULL, 0, sysctl_devname, "", "devname(3) handler");
