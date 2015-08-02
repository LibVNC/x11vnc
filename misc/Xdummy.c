/*
   Copyright (C) 2002-2010 Karl J. Runge <runge@karlrunge.com> 
   All rights reserved.

This file is part of x11vnc.

x11vnc is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or (at
your option) any later version.

x11vnc is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with x11vnc; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA
or see <http://www.gnu.org/licenses/>.

In addition, as a special exception, Karl J. Runge
gives permission to link the code of its release of x11vnc with the
OpenSSL project's "OpenSSL" library (or with modified versions of it
that use the same license as the "OpenSSL" library), and distribute
the linked executables.  You must obey the GNU General Public License
in all respects for all of the code used other than "OpenSSL".  If you
modify this file, you may extend this exception to your version of the
file, but you are not obligated to do so.  If you do not wish to do
so, delete this exception statement from your version.
*/

/* -- Xdummy.c -- */
#include <stdio.h>
#define O_ACCMODE          0003
#define O_RDONLY             00
#define O_WRONLY             01
#define O_RDWR               02
#define O_CREAT            0100 /* not fcntl */
#define O_EXCL             0200 /* not fcntl */
#define O_NOCTTY           0400 /* not fcntl */
#define O_TRUNC           01000 /* not fcntl */
#define O_APPEND          02000
#define O_NONBLOCK        04000
#define O_NDELAY        O_NONBLOCK
#define O_SYNC           010000
#define O_FSYNC          O_SYNC
#define O_ASYNC          020000

#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <linux/vt.h>
#include <linux/kd.h>

#define __USE_GNU
#include <dlfcn.h>

#include <time.h>
#include <sys/stat.h>

static char tmpdir[4096];
static char str1[4096];
static char str2[4096];

static char devs[256][1024];
static int debug = -1;
static int root = -1;
static int changed_uid = 0;
static int saw_fonts = 0;
static int saw_lib_modules = 0;

static time_t start = 0; 

void check_debug(void) {
	if (debug < 0) {
		if (getenv("XDUMMY_DEBUG") != NULL) {
			debug = 1;
		} else {
			debug = 0;
		}
		/* prevent other processes using the preload: */
		putenv("LD_PRELOAD=");
	}
}
void check_root(void) {
	if (root < 0) {
		/* script tells us if we are root */
		if (getenv("XDUMMY_ROOT") != NULL) {
			root = 1;
		} else {
			root = 0;
		}
	}
}

void check_uid(void) {
	if (start == 0) {
		start = time(NULL);
		if (debug) fprintf(stderr, "START: %u\n", (unsigned int) start);
		return;
	} else if (changed_uid == 0) {
		if (saw_fonts || time(NULL) > start + 20) {
			if (getenv("XDUMMY_UID")) {
				int uid = atoi(getenv("XDUMMY_UID"));
				if (debug) fprintf(stderr, "SETREUID: %d saw_fonts=%d\n", uid, saw_fonts);
				if (uid >= 0) {
					/* this will simply fail in -nonroot mode: */
					setreuid(uid, -1);
				}
			}
			changed_uid = 1;
		}
	}
}

#define CHECKIT if (debug < 0) check_debug(); \
		if (root  < 0) check_root(); \
		check_uid();

static void set_tmpdir(void) {
	char *s;
	static int didset = 0;
	if (didset) {
		return;
	}
	s = getenv("XDUMMY_TMPDIR");
	if (! s) {
		s = "/tmp";
	}
	tmpdir[0] = '\0';
	strcat(tmpdir, s);
	strcat(tmpdir, "/");
	didset = 1;
}

static char *tmpdir_path(const char *path) {
	char *str;
	set_tmpdir();
	strcpy(str2, path);
	str = str2;
	while (*str) {
		if (*str == '/') {
			*str = '_';
		}
		str++;
	}
	strcpy(str1, tmpdir);
	strcat(str1, str2);
	return str1;
}

int open(const char *pathname, int flags, unsigned short mode) {
	int fd;
	char *store_dev = NULL;
	static int (*real_open)(const char *, int , unsigned short) = NULL;

	CHECKIT
	if (! real_open) {
		real_open = (int (*)(const char *, int , unsigned short))
			dlsym(RTLD_NEXT, "open");
	}

	if (strstr(pathname, "lib/modules/")) {
		/* not currently used. */
		saw_lib_modules = 1;
	}

	if (!root) {
		if (strstr(pathname, "/dev/") == pathname) {
			store_dev = strdup(pathname);
		}
		if (strstr(pathname, "/dev/tty") == pathname && strcmp(pathname, "/dev/tty")) {
			pathname = tmpdir_path(pathname);
			if (debug) fprintf(stderr, "OPEN: %s -> %s (as FIFO)\n", store_dev, pathname);
			/* we make it a FIFO so ioctl on it does not fail */
			unlink(pathname);
			mkfifo(pathname, 0666);
		} else if (0) {
			/* we used to handle more /dev files ... */
			fd = real_open(pathname, O_WRONLY|O_CREAT, 0777);
			close(fd);
		}
	}

	fd = real_open(pathname, flags, mode);

	if (debug) fprintf(stderr, "OPEN: %s %d %d fd=%d\n", pathname, flags, mode, fd);

	if (! root) {
		if (store_dev) {
			if (fd < 256) {
				strcpy(devs[fd], store_dev);
			}
			free(store_dev);
		}
	}

	return(fd);
}

int open64(const char *pathname, int flags, unsigned short mode) {
	int fd;

	CHECKIT
	if (debug) fprintf(stderr, "OPEN64: %s %d %d\n", pathname, flags, mode);

	fd = open(pathname, flags, mode);
	return(fd);
}

int rename(const char *oldpath, const char *newpath) {
	static int (*real_rename)(const char *, const char *) = NULL;

	CHECKIT
	if (! real_rename) {
		real_rename = (int (*)(const char *, const char *))
			dlsym(RTLD_NEXT, "rename");
	}

	if (debug) fprintf(stderr, "RENAME: %s %s\n", oldpath, newpath);

	if (root) {
		return(real_rename(oldpath, newpath));
	}

	if (strstr(oldpath, "/var/log") == oldpath) {
		if (debug) fprintf(stderr, "RENAME: returning 0\n");
		return 0;
	}
	return(real_rename(oldpath, newpath));
}

FILE *fopen(const char *pathname, const char *mode) {
	static FILE* (*real_fopen)(const char *, const char *) = NULL;
	char *str;

	if (! saw_fonts) {
		if (strstr(pathname, "/fonts/")) {
			if (strstr(pathname, "fonts.dir")) {
				saw_fonts = 1;
			} else if (strstr(pathname, "fonts.alias")) {
				saw_fonts = 1;
			}
		}
	}

	CHECKIT
	if (! real_fopen) {
		real_fopen = (FILE* (*)(const char *, const char *))
			dlsym(RTLD_NEXT, "fopen");
	}

	if (debug) fprintf(stderr, "FOPEN: %s %s\n", pathname, mode);

	if (strstr(pathname, "xdummy_modified_xconfig.conf")) {
		/* make our config appear to be in /etc/X11, etc. */
		char *q = strrchr(pathname, '/');
		if (q != NULL && getenv("XDUMMY_TMPDIR") != NULL) {
			strcpy(str1, getenv("XDUMMY_TMPDIR"));
			strcat(str1, q);
			if (debug) fprintf(stderr, "FOPEN: %s -> %s\n", pathname, str1);
			pathname = str1;
		}
	}

	if (root) {
		return(real_fopen(pathname, mode));
	}

	str = (char *) pathname;
	if (strstr(pathname, "/var/log") == pathname) {
		str = tmpdir_path(pathname);
		if (debug) fprintf(stderr, "FOPEN: %s -> %s\n", pathname, str);
	}
	return(real_fopen(str, mode));
}


#define RETURN0 if (debug) \
	{fprintf(stderr, "IOCTL: covered %d 0x%x\n", fd, req);} return 0;
#define RETURN1 if (debug) \
	{fprintf(stderr, "IOCTL: covered %d 0x%x\n", fd, req);} return -1;

int ioctl(int fd, int req, void *ptr) {
	static int closed_xf86Info_consoleFd = 0;
	static int (*real_ioctl)(int, int , void *) = NULL;

	CHECKIT
	if (! real_ioctl) {
		real_ioctl = (int (*)(int, int , void *))
			dlsym(RTLD_NEXT, "open");
	}
	if (debug) fprintf(stderr, "IOCTL: %d 0x%x %p\n", fd, req, ptr);

	/* based on xorg-x11-6.8.1-dualhead.patch */
	if (req == VT_GETMODE) {
		/* close(xf86Info.consoleFd) */
		if (0 && ! closed_xf86Info_consoleFd) {
			/* I think better not to close it... */
			close(fd);
			closed_xf86Info_consoleFd = 1;
		}
		RETURN0
	} else if (req == VT_SETMODE) {
		RETURN0
	} else if (req == VT_GETSTATE) {
		RETURN0
	} else if (req == KDSETMODE) {
		RETURN0
	} else if (req == KDSETLED) {
		RETURN0
	} else if (req == KDGKBMODE) {
		RETURN0
	} else if (req == KDSKBMODE) {
		RETURN0
	} else if (req == VT_ACTIVATE) {
		RETURN0
	} else if (req == VT_WAITACTIVE) {
		RETURN0
	} else if (req == VT_RELDISP) {
		if (ptr == (void *) 1) {
			RETURN1
		} else if (ptr == (void *) VT_ACKACQ) {
			RETURN0
		}
	}

	return(real_ioctl(fd, req, ptr));
}

typedef void (*sighandler_t)(int);
#define SIGUSR1       10
#define SIG_DFL       ((sighandler_t)0)

sighandler_t signal(int signum, sighandler_t handler) {
	static sighandler_t (*real_signal)(int, sighandler_t) = NULL;

	CHECKIT
	if (! real_signal) {
		real_signal = (sighandler_t (*)(int, sighandler_t))
			dlsym(RTLD_NEXT, "signal");
	}

	if (debug) fprintf(stderr, "SIGNAL: %d %p\n", signum, handler);

	if (signum == SIGUSR1) {
		if (debug) fprintf(stderr, "SIGNAL: skip SIGUSR1\n");
		return SIG_DFL;
	}
	
	return(real_signal(signum, handler));
}

int close(int fd) {
	static int (*real_close)(int) = NULL;

	CHECKIT
	if (! real_close) {
		real_close = (int (*)(int)) dlsym(RTLD_NEXT, "close");
	}

	if (debug) fprintf(stderr, "CLOSE: %d\n", fd);
	if (!root) {
		if (fd < 256) {
			devs[fd][0] = '\0';
		}
	}
	return(real_close(fd));
}

int stat(const char *path, struct stat *buf) {
	static int (*real_stat)(const char *, struct stat *) = NULL;

	CHECKIT
	if (! real_stat) {
		real_stat = (int (*)(const char *, struct stat *))
			dlsym(RTLD_NEXT, "stat");
	}

	if (debug) fprintf(stderr, "STAT: %s\n", path);

	return(real_stat(path, buf));
}

int stat64(const char *path, struct stat *buf) {
	static int (*real_stat64)(const char *, struct stat *) = NULL;

	CHECKIT
	if (! real_stat64) {
		real_stat64 = (int (*)(const char *, struct stat *))
			dlsym(RTLD_NEXT, "stat64");
	}

	if (debug) fprintf(stderr, "STAT64: %s\n", path);

	return(real_stat64(path, buf));
}

int chown(const char *path, uid_t owner, gid_t group) {
	static int (*real_chown)(const char *, uid_t, gid_t) = NULL;

	CHECKIT
	if (! real_chown) {
		real_chown = (int (*)(const char *, uid_t, gid_t))
			dlsym(RTLD_NEXT, "chown");
	}

	if (root) {
		return(real_chown(path, owner, group));
	}

	if (debug) fprintf(stderr, "CHOWN: %s %d %d\n", path, owner, group);

	if (strstr(path, "/dev") == path) {
		if (debug) fprintf(stderr, "CHOWN: return 0\n");
		return 0;
	}

	return(real_chown(path, owner, group));
}

extern int *__errno_location (void);
#ifndef ENODEV
#define ENODEV 19
#endif

int ioperm(unsigned long from, unsigned long num, int turn_on) {
	static int (*real_ioperm)(unsigned long, unsigned long, int) = NULL;

	CHECKIT
	if (! real_ioperm) {
		real_ioperm = (int (*)(unsigned long, unsigned long, int))
			dlsym(RTLD_NEXT, "ioperm");
	}
	if (debug) fprintf(stderr, "IOPERM: %d %d %d\n", (int) from, (int) num, turn_on);
	if (root) {
		return(real_ioperm(from, num, turn_on));
	}
	if (from == 0 && num == 1024 && turn_on == 1) {
		/* we want xf86EnableIO to fail */
		if (debug) fprintf(stderr, "IOPERM: setting ENODEV.\n");
		*__errno_location() = ENODEV;
		return -1;
	}
	return 0;
}

int iopl(int level) {
	static int (*real_iopl)(int) = NULL;

	CHECKIT
	if (! real_iopl) {
		real_iopl = (int (*)(int)) dlsym(RTLD_NEXT, "iopl");
	}
	if (debug) fprintf(stderr, "IOPL: %d\n", level);
	if (root) {
		return(real_iopl(level));
	}
	return 0;
}

#ifdef INTERPOSE_GETUID 

/*
 * we got things to work w/o pretending to be root.
 * so we no longer interpose getuid(), etc.
 */

uid_t getuid(void) {
	static uid_t (*real_getuid)(void) = NULL;
	CHECKIT
	if (! real_getuid) {
		real_getuid = (uid_t (*)(void)) dlsym(RTLD_NEXT, "getuid");
	}
	if (root) {
		return(real_getuid());
	}
	if (debug) fprintf(stderr, "GETUID: 0\n");
	return 0;
}
uid_t geteuid(void) {
	static uid_t (*real_geteuid)(void) = NULL;
	CHECKIT
	if (! real_geteuid) {
		real_geteuid = (uid_t (*)(void)) dlsym(RTLD_NEXT, "geteuid");
	}
	if (root) {
		return(real_geteuid());
	}
	if (debug) fprintf(stderr, "GETEUID: 0\n");
	return 0;
}
uid_t geteuid_kludge1(void) {
	static uid_t (*real_geteuid)(void) = NULL;
	CHECKIT
	if (! real_geteuid) {
		real_geteuid = (uid_t (*)(void)) dlsym(RTLD_NEXT, "geteuid");
	}
	if (debug) fprintf(stderr, "GETEUID: 0 saw_libmodules=%d\n", saw_lib_modules);
	if (root && !saw_lib_modules) {
		return(real_geteuid());
	} else {
		saw_lib_modules = 0;
		return 0;
	}
}

uid_t getuid32(void) {
	static uid_t (*real_getuid32)(void) = NULL;
	CHECKIT
	if (! real_getuid32) {
		real_getuid32 = (uid_t (*)(void)) dlsym(RTLD_NEXT, "getuid32");
	}
	if (root) {
		return(real_getuid32());
	}
	if (debug) fprintf(stderr, "GETUID32: 0\n");
	return 0;
}
uid_t geteuid32(void) {
	static uid_t (*real_geteuid32)(void) = NULL;
	CHECKIT
	if (! real_geteuid32) {
		real_geteuid32 = (uid_t (*)(void)) dlsym(RTLD_NEXT, "geteuid32");
	}
	if (root) {
		return(real_geteuid32());
	}
	if (debug) fprintf(stderr, "GETEUID32: 0\n");
	return 0;
}

gid_t getgid(void) {
	static gid_t (*real_getgid)(void) = NULL;
	CHECKIT
	if (! real_getgid) {
		real_getgid = (gid_t (*)(void)) dlsym(RTLD_NEXT, "getgid");
	}
	if (root) {
		return(real_getgid());
	}
	if (debug) fprintf(stderr, "GETGID: 0\n");
	return 0;
}
gid_t getegid(void) {
	static gid_t (*real_getegid)(void) = NULL;
	CHECKIT
	if (! real_getegid) {
		real_getegid = (gid_t (*)(void)) dlsym(RTLD_NEXT, "getegid");
	}
	if (root) {
		return(real_getegid());
	}
	if (debug) fprintf(stderr, "GETEGID: 0\n");
	return 0;
}
gid_t getgid32(void) {
	static gid_t (*real_getgid32)(void) = NULL;
	CHECKIT
	if (! real_getgid32) {
		real_getgid32 = (gid_t (*)(void)) dlsym(RTLD_NEXT, "getgid32");
	}
	if (root) {
		return(real_getgid32());
	}
	if (debug) fprintf(stderr, "GETGID32: 0\n");
	return 0;
}
gid_t getegid32(void) {
	static gid_t (*real_getegid32)(void) = NULL;
	CHECKIT
	if (! real_getegid32) {
		real_getegid32 = (gid_t (*)(void)) dlsym(RTLD_NEXT, "getegid32");
	}
	if (root) {
		return(real_getegid32());
	}
	if (debug) fprintf(stderr, "GETEGID32: 0\n");
	return 0;
}
#endif

#if 0
/* maybe we need to interpose on strcmp someday... here is the template */
int strcmp(const char *s1, const char *s2) {
	static int (*real_strcmp)(const char *, const char *) = NULL;
	CHECKIT
	if (! real_strcmp) {
		real_strcmp = (int (*)(const char *, const char *)) dlsym(RTLD_NEXT, "strcmp");
	}
	if (debug) fprintf(stderr, "STRCMP: '%s' '%s'\n", s1, s2);
	return(real_strcmp(s1, s2));
}
#endif
