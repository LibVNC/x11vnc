# Process this file with autoconf to produce a configure script.
AC_INIT(x11vnc, 0.9.16, https://github.com/LibVNC/x11vnc)
AM_INIT_AUTOMAKE([subdir-objects])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])
AM_CONFIG_HEADER(config.h)
AC_CONFIG_MACRO_DIR([m4])

AM_MAINTAINER_MODE([enable])

AC_PROG_CC
AC_PROG_SED
PKG_PROG_PKG_CONFIG


# some OS's need both -lssl and -lcrypto on link line:
AH_TEMPLATE(HAVE_LIBCRYPTO, [openssl libcrypto library present])
AC_ARG_WITH(crypto,
[  --without-crypto        disable support for openssl libcrypto],,)

AH_TEMPLATE(HAVE_LIBSSL, [openssl libssl library present])
AC_ARG_WITH(ssl,
[  --without-ssl           disable support for openssl libssl]
[  --with-ssl=DIR          use openssl include/library files in DIR],,)

if test "x$with_crypto" != "xno" -a "x$with_ssl" != "xno"; then
	if test ! -z "$with_ssl" -a "x$with_ssl" != "xyes"; then
		saved_CPPFLAGS="$CPPFLAGS"
		saved_LDFLAGS="$LDFLAGS"
		CPPFLAGS="$CPPFLAGS -I$with_ssl/include"
		LDFLAGS="$LDFLAGS -L$with_ssl/lib"
		if test "x$ld_minus_R" = "xno"; then
			:
		elif test "x$GCC" = "xyes"; then
			LDFLAGS="$LDFLAGS -Xlinker -R$with_ssl/lib"
		else
			LDFLAGS="$LDFLAGS -R$with_ssl/lib"
		fi
	fi
	AC_CHECK_LIB(crypto, RAND_file_name,
		[AC_DEFINE(HAVE_LIBCRYPTO) HAVE_LIBCRYPTO="true"], ,)
	if test ! -z "$with_ssl" -a "x$with_ssl" != "xyes"; then
		if test "x$HAVE_LIBCRYPTO" != "xtrue"; then
			CPPFLAGS="$saved_CPPFLAGS"
			LDFLAGS="$saved_LDFLAGS"
		fi
	fi
fi

AH_TEMPLATE(HAVE_LIBCRYPT, [libcrypt library present])
AC_ARG_WITH(crypt,
[  --without-crypt         disable support for libcrypt],,)
if test "x$with_crypt" != "xno"; then
	AC_CHECK_FUNCS([crypt], HAVE_LIBC_CRYPT="true")
	if test -z "$HAVE_LIBC_CRYPT"; then
		AC_CHECK_LIB(crypt, crypt,
			CRYPT_LIBS="-lcrypt"
			[AC_DEFINE(HAVE_LIBCRYPT)], ,)
	fi
fi
AC_SUBST(CRYPT_LIBS)

AH_TEMPLATE(HAVE_X509_PRINT_EX_FP, [open ssl X509_print_ex_fp available])
if test "x$with_ssl" != "xno"; then
	if test "x$HAVE_LIBCRYPTO" = "xtrue"; then
		PKG_CHECK_MODULES(OPENSSL, [openssl >= 1.0.0],
			SSL_LIBS="-lssl -lcrypto"
			[AC_DEFINE(HAVE_LIBSSL) HAVE_LIBSSL="true"], ,)
	else
		PKG_CHECK_MODULES(OPENSSL, [openssl >= 1.0.0],
			SSL_LIBS="-lssl"
			[AC_DEFINE(HAVE_LIBSSL) HAVE_LIBSSL="true"], ,)
	fi
fi
AC_SUBST(SSL_LIBS)
AM_CONDITIONAL(HAVE_LIBSSL, test ! -z "$SSL_LIBS")

# Checks for X libraries
HAVE_X11="false"
AC_PATH_XTRA
AH_TEMPLATE(HAVE_X11, [X11 build environment present])



# FIXME always true, should be factored out
build_x11vnc="yes"

# x11vnc only:
if test "$build_x11vnc" = "yes"; then

AH_TEMPLATE(HAVE_XSHM, [MIT-SHM extension build environment present])
AH_TEMPLATE(HAVE_XTEST, [XTEST extension build environment present])
AH_TEMPLATE(HAVE_XTESTGRABCONTROL, [XTEST extension has XTestGrabControl])
AH_TEMPLATE(HAVE_XKEYBOARD, [XKEYBOARD extension build environment present])
AH_TEMPLATE(HAVE_LIBXINERAMA, [XINERAMA extension build environment present])
AH_TEMPLATE(HAVE_LIBXRANDR, [XRANDR extension build environment present])
AH_TEMPLATE(HAVE_LIBXFIXES, [XFIXES extension build environment present])
AH_TEMPLATE(HAVE_LIBXDAMAGE, [XDAMAGE extension build environment present])
AH_TEMPLATE(HAVE_LIBXCOMPOSITE, [XCOMPOSITE extension build environment present])
AH_TEMPLATE(HAVE_LIBXCURSOR, [Xcursor library build environment present])
AH_TEMPLATE(HAVE_XI2, [XINPUT 2 extension build environment present])
AH_TEMPLATE(HAVE_LIBXTRAP, [DEC-XTRAP extension build environment present])
AH_TEMPLATE(HAVE_RECORD, [RECORD extension build environment present])
AH_TEMPLATE(HAVE_SOLARIS_XREADSCREEN, [Solaris XReadScreen available])
AH_TEMPLATE(HAVE_IRIX_XREADDISPLAY, [IRIX XReadDisplay available])
AH_TEMPLATE(HAVE_FBPM, [FBPM extension build environment present])
AH_TEMPLATE(HAVE_DPMS, [DPMS extension build environment present])
AH_TEMPLATE(HAVE_LINUX_VIDEODEV_H, [video4linux build environment present])
AH_TEMPLATE(HAVE_LINUX_FB_H, [linux fb device build environment present])
AH_TEMPLATE(HAVE_LINUX_INPUT_H, [linux/input.h present])
AH_TEMPLATE(HAVE_LINUX_UINPUT_H, [linux uinput device build environment present])
AH_TEMPLATE(HAVE_MACOSX_NATIVE_DISPLAY, [build MacOS X native display support])
AH_TEMPLATE(HAVE_MACOSX_OPENGL_H, [MacOS X OpenGL present])

AC_ARG_WITH(xkeyboard,
[  --without-xkeyboard     disable xkeyboard extension support],,)
AC_ARG_WITH(xinerama,
[  --without-xinerama      disable xinerama extension support],,)
AC_ARG_WITH(xrandr,
[  --without-xrandr        disable xrandr extension support],,)
AC_ARG_WITH(xfixes,
[  --without-xfixes        disable xfixes extension support],,)
AC_ARG_WITH(xdamage,
[  --without-xdamage       disable xdamage extension support],,)
AC_ARG_WITH(xcomposite,
[  --without-xcomposite    disable xcomposite extension support],,)
AC_ARG_WITH(xtrap,
[  --without-xtrap         disable xtrap extension support],,)
AC_ARG_WITH(xrecord,
[  --without-xrecord       disable xrecord extension support],,)
AC_ARG_WITH(fbpm,
[  --without-fbpm          disable fbpm extension support],,)
AC_ARG_WITH(dpms,
[  --without-dpms          disable dpms extension support],,)
AC_ARG_WITH(v4l,
[  --without-v4l           disable video4linux support],,)
AC_ARG_WITH(fbdev,
[  --without-fbdev         disable linux fb device support],,)
AC_ARG_WITH(uinput,
[  --without-uinput        disable linux uinput device support],,)
AC_ARG_WITH(macosx-native,
[  --without-macosx-native disable MacOS X native display support],,)
AC_ARG_WITH(colormultipointer,
[  --without-colormultipointer  disable color support for multiple pointers]
[  --with-cairo=DIR        use cairo include/library files in DIR (needed for color cursor support)],,)

fi
# end x11vnc only.

if test "x$with_x" = "xno"; then
	HAVE_X11="false"
elif test "$X_CFLAGS" != "-DX_DISPLAY_MISSING"; then
	AC_CHECK_LIB(X11, XGetImage, [AC_DEFINE(HAVE_X11) HAVE_X11="true"],
		HAVE_X11="false",
		$X_LIBS $X_PRELIBS -lX11 $X_EXTRA_LIBS)

	# x11vnc only:
	if test $HAVE_X11 = "true" -a "$build_x11vnc" = "yes"; then
		X_PRELIBS="$X_PRELIBS -lXext"

		AC_CHECK_LIB(Xext, XShmGetImage, 
			[AC_DEFINE(HAVE_XSHM)], ,
			$X_LIBS $X_PRELIBS -lX11 $X_EXTRA_LIBS)

		AC_CHECK_LIB(Xext, XReadScreen, 
			[AC_DEFINE(HAVE_SOLARIS_XREADSCREEN)], ,
			$X_LIBS $X_PRELIBS -lX11 $X_EXTRA_LIBS)

		AC_CHECK_HEADER(X11/extensions/readdisplay.h,
			[AC_DEFINE(HAVE_IRIX_XREADDISPLAY)], ,
			[#include <X11/Xlib.h>])

		if test "x$with_fbpm" != "xno"; then
			AC_CHECK_LIB(Xext, FBPMForceLevel, 
				[AC_DEFINE(HAVE_FBPM)], ,
				$X_LIBS $X_PRELIBS -lX11 $X_EXTRA_LIBS)
		fi

		if test "x$with_dpms" != "xno"; then
			AC_CHECK_LIB(Xext, DPMSForceLevel, 
				[AC_DEFINE(HAVE_DPMS)], ,
				$X_LIBS $X_PRELIBS -lX11 $X_EXTRA_LIBS)
		fi

		AC_CHECK_LIB(Xtst, XTestGrabControl, 
			X_PRELIBS="-lXtst $X_PRELIBS"
			[AC_DEFINE(HAVE_XTESTGRABCONTROL) HAVE_XTESTGRABCONTROL="true"], ,
			$X_LIBS $X_PRELIBS -lX11 $X_EXTRA_LIBS)

		AC_CHECK_LIB(Xtst, XTestFakeKeyEvent, 
			X_PRELIBS="-lXtst $X_PRELIBS"
			[AC_DEFINE(HAVE_XTEST) HAVE_XTEST="true"], ,
			$X_LIBS $X_PRELIBS -lX11 $X_EXTRA_LIBS)

		if test "x$with_xrecord" != "xno"; then
			AC_CHECK_LIB(Xtst, XRecordEnableContextAsync, 
				X_PRELIBS="-lXtst $X_PRELIBS"
				[AC_DEFINE(HAVE_RECORD)], ,
				$X_LIBS $X_PRELIBS -lX11 $X_EXTRA_LIBS)
		fi

		# we use XTRAP on X11R5, or user can set X11VNC_USE_XTRAP
		if test "x$with_xtrap" != "xno"; then
		    if test ! -z "$X11VNC_USE_XTRAP" -o -z "$HAVE_XTESTGRABCONTROL"; then
			AC_CHECK_LIB(XTrap, XETrapSetGrabServer, 
				X_PRELIBS="$X_PRELIBS -lXTrap"
				[AC_DEFINE(HAVE_LIBXTRAP)], ,
				$X_LIBS $X_PRELIBS -lX11 $X_EXTRA_LIBS)
			# tru64 uses libXETrap.so
			AC_CHECK_LIB(XETrap, XETrapSetGrabServer, 
				X_PRELIBS="$X_PRELIBS -lXETrap"
				[AC_DEFINE(HAVE_LIBXTRAP)], ,
				$X_LIBS $X_PRELIBS -lX11 $X_EXTRA_LIBS)
		    fi
		fi

		if test "x$with_xkeyboard" != "xno"; then
			saved_CPPFLAGS="$CPPFLAGS"
			CPPFLAGS="$CPPFLAGS $X_CFLAGS"
			AC_CHECK_HEADER(X11/XKBlib.h, HAVE_XKBLIB_H="true",
				HAVE_XKBLIB_H="false", [#include <X11/Xlib.h>])
			CPPFLAGS="$saved_CPPFLAGS"
			if test $HAVE_XKBLIB_H = "true"; then
				AC_CHECK_LIB(X11, XkbSelectEvents,
					[AC_DEFINE(HAVE_XKEYBOARD)], ,
					$X_LIBS $X_PRELIBS -lX11 $X_EXTRA_LIBS)
			fi
		fi

		if test "x$with_xinerama" != "xno"; then
			AC_CHECK_LIB(Xinerama, XineramaQueryScreens,
				X_PRELIBS="$X_PRELIBS -lXinerama"
				[AC_DEFINE(HAVE_LIBXINERAMA)], ,
				$X_LIBS $X_PRELIBS -lX11 $X_EXTRA_LIBS)
		fi

		if test "x$with_xrandr" != "xno"; then
			AC_CHECK_LIB(Xrandr, XRRSelectInput,
				X_PRELIBS="$X_PRELIBS -lXrandr"
				[AC_DEFINE(HAVE_LIBXRANDR) HAVE_LIBXRANDR="true"], ,
				$X_LIBS $X_PRELIBS -lX11 $X_EXTRA_LIBS)
		fi

		if test "x$with_xfixes" != "xno"; then
			AC_CHECK_LIB(Xfixes, XFixesGetCursorImage,
				X_PRELIBS="$X_PRELIBS -lXfixes"
				[AC_DEFINE(HAVE_LIBXFIXES) HAVE_LIBXFIXES="true"], ,
				$X_LIBS $X_PRELIBS -lX11 $X_EXTRA_LIBS)
		fi

		if test "x$with_xdamage" != "xno"; then
			AC_CHECK_LIB(Xdamage, XDamageQueryExtension,
				X_PRELIBS="$X_PRELIBS -lXdamage"
				[AC_DEFINE(HAVE_LIBXDAMAGE) HAVE_LIBXDAMAGE="true"], ,
				$X_LIBS $X_PRELIBS -lX11 $X_EXTRA_LIBS)
		fi

		if test "x$with_xcomposite" != "xno"; then
		   	AC_CHECK_LIB(Xcomposite, XCompositeQueryExtension,
				X_PRELIBS="$X_PRELIBS -lXcomposite"
			        [AC_DEFINE(HAVE_LIBXCOMPOSITE) HAVE_LIBXCOMPOSITE="true"], ,
			        $X_LIBS $X_PRELIBS -lX11 $X_EXTRA_LIBS)
		fi

		# check for XI2 support
		PKG_CHECK_MODULES(XI2, [xi >= 1.2.99] [inputproto >= 1.9.99.9],
			 HAVE_XI2="yes"; AC_DEFINE(HAVE_XI2, 1, [XI2 available]),
			 HAVE_XI2="no");

		if test "$HAVE_XI2" = "yes" -a "x$with_colormultipointer" != "xno"; then
			AC_CHECK_LIB(Xcursor, XcursorImageLoadCursor,
				X_PRELIBS="$X_PRELIBS -lXcursor"
				[AC_DEFINE(HAVE_LIBXCURSOR) HAVE_LIBXCURSOR="true"], ,
				$X_LIBS $X_PRELIBS -lX11 $X_EXTRA_LIBS)
		fi


		if test ! -z "$HAVE_LIBXFIXES" -o ! -z "$HAVE_LIBXDAMAGE"; then
			# need /usr/sfw/lib in RPATH for Solaris 10 and later
			case `(uname -sr) 2>/dev/null` in
			"SunOS 5"*) X_EXTRA_LIBS="$X_EXTRA_LIBS -R/usr/sfw/lib" ;;
			esac
		fi
		if test ! -z "$HAVE_LIBXRANDR"; then
			# also need /usr/X11/include for Solaris 10 10/08 and later
			case `(uname -sr) 2>/dev/null` in
			"SunOS 5"*) CPPFLAGS="$CPPFLAGS -I/usr/X11/include" ;;
			esac
		fi

		X_LIBS="$X_LIBS $X_PRELIBS -lX11 $X_EXTRA_LIBS"
	fi
	# end x11vnc only.
fi

AC_SUBST(X_LIBS)
AM_CONDITIONAL(HAVE_X11, test $HAVE_X11 != "false")

# x11vnc only:
if test "$build_x11vnc" = "yes"; then

if test "x$HAVE_X11" = "xfalse" -a "x$with_x" != "xno"; then
	AC_MSG_ERROR([
==========================================================================
*** A working X window system build environment is required to build ***
x11vnc.  Make sure any required X development packages are installed.
If they are installed in non-standard locations, one can use the
--x-includes=DIR and --x-libraries=DIR configure options or set the
CPPFLAGS and LDFLAGS environment variables to indicate where the X
window system header files and libraries may be found.  On 64+32 bit
machines you may need to point to lib64 or lib32 directories to pick up
the correct word size.

If you want to build x11vnc without X support (e.g. for -rawfb use only
or for native Mac OS X), specify the --without-x configure option.
==========================================================================
])
fi

if test "x$HAVE_X11" = "xtrue" -a "x$HAVE_XTEST" != "xtrue"; then
	AC_MSG_WARN([
==========================================================================
*** A working build environment for the XTEST extension was not found ***
(libXtst).  An x11vnc built this way will be *ONLY BARELY USABLE*.
You will be able to move the mouse but not click or type.  There can
also be deadlocks if an application grabs the X server.

It is recommended that you install the necessary development packages
for XTEST (perhaps it is named something like libxtst-dev) and run
configure again.
==========================================================================
])
	sleep 5
fi

if test "x$with_v4l" != "xno"; then
	AC_CHECK_HEADER(linux/videodev.h,
		[AC_DEFINE(HAVE_LINUX_VIDEODEV_H)],,)
fi
if test "x$with_fbdev" != "xno"; then
	AC_CHECK_HEADER(linux/fb.h,
		[AC_DEFINE(HAVE_LINUX_FB_H)],,)
fi
if test "x$with_uinput" != "xno"; then
	AC_CHECK_HEADER(linux/input.h,
		[AC_DEFINE(HAVE_LINUX_INPUT_H) HAVE_LINUX_INPUT_H="true"],,)
	if test "x$HAVE_LINUX_INPUT_H" = "xtrue"; then
		AC_CHECK_HEADER(linux/uinput.h,
			[AC_DEFINE(HAVE_LINUX_UINPUT_H)],, [#include <linux/input.h>])
	fi
fi

if test "x$with_macosx_native" != "xno"; then
	AC_DEFINE(HAVE_MACOSX_NATIVE_DISPLAY)
fi

# Check for OS X opengl header
AC_CHECK_HEADER(OpenGL/OpenGL.h,
	[AC_DEFINE(HAVE_MACOSX_OPENGL_H) HAVE_MACOSX_OPENGL_H="true"],,)

AH_TEMPLATE(HAVE_AVAHI, [Avahi/mDNS client build environment present])
AC_ARG_WITH(avahi,
[  --without-avahi         disable support for Avahi/mDNS]
[  --with-avahi=DIR        use avahi include/library files in DIR],,)
if test "x$with_avahi" != "xno"; then
	printf "checking for avahi... "
	if test ! -z "$with_avahi" -a "x$with_avahi" != "xyes"; then
		AVAHI_CFLAGS="-I$with_avahi/include"
		AVAHI_LIBS="-L$with_avahi/lib -lavahi-common -lavahi-client"
		echo "using $with_avahi"
		with_avahi=yes
	elif pkg-config --atleast-version=0.6.4 avahi-client >/dev/null 2>&1; then
		AVAHI_CFLAGS=`pkg-config --cflags avahi-client`
		AVAHI_LIBS=`pkg-config --libs avahi-client`
		with_avahi=yes
		echo yes
	else
		with_avahi=no
		echo no
	fi
fi
if test "x$with_avahi" = "xyes"; then
	AC_DEFINE(HAVE_AVAHI)
	AC_SUBST(AVAHI_CFLAGS)
	AC_SUBST(AVAHI_LIBS)
fi

if test "$HAVE_XI2" = "yes"; then
AH_TEMPLATE(HAVE_CAIRO, [cairo graphics library present])
if test "x$with_cairo" != "xno"; then
	printf "checking for cairo... "
	if test ! -z "$with_cairo" -a "x$with_cairo" != "xyes"; then
		CAIRO_CFLAGS="-I$with_cairo/include"
		CAIRO_LIBS="-L$with_cairo/lib -lcairo"
		echo "using $with_cairo"
		with_cairo=yes
	elif pkg-config cairo >/dev/null 2>&1; then
		CAIRO_CFLAGS=`pkg-config --cflags cairo`
		CAIRO_LIBS=`pkg-config --libs cairo`
		with_cairo=yes
		echo yes
	else
		with_cairo=no
		echo no
	fi
fi
if test "x$with_cairo" = "xyes"; then
	AC_DEFINE(HAVE_CAIRO)
	AC_SUBST(CAIRO_CFLAGS)
	AC_SUBST(CAIRO_LIBS)
fi
fi


fi
# end x11vnc only.

# XI2 present?
AM_CONDITIONAL(HAVE_XI2, [ test "$HAVE_XI2" = "yes" ])
# cairo?
AM_CONDITIONAL(HAVE_CAIRO, [ test "$with_cairo" = "yes" ])


# only used in x11vnc/Makefile.am but needs to always be defined:
AM_CONDITIONAL(OSX_OPENGL, test "$HAVE_MACOSX_OPENGL_H" = "true")

# Checks for libraries.
PKG_CHECK_MODULES(LIBVNCSERVER, libvncserver >= 0.9.8)
PKG_CHECK_MODULES(LIBVNCCLIENT, libvncclient >= 0.9.8)



AC_CANONICAL_HOST
MINGW=`echo $host_os | grep mingw32  2>/dev/null`
AM_CONDITIONAL(MINGW, test ! -z "$MINGW" )
if test ! -z "$MINGW"; then
	WSOCKLIB="-lws2_32"
	LDFLAGS="$LDFLAGS -no-undefined"
fi
AC_SUBST(WSOCKLIB)



# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([arpa/inet.h endian.h fcntl.h netdb.h netinet/in.h stdlib.h string.h sys/endian.h sys/socket.h sys/time.h sys/timeb.h syslog.h unistd.h ws2tcpip.h])
AC_CHECK_HEADERS([pwd.h sys/wait.h utmpx.h termios.h sys/ioctl.h sys/stropts.h])


# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_C_BIGENDIAN
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_HEADER_SYS_WAIT
AX_TYPE_SOCKLEN_T
AC_CACHE_CHECK([for in_addr_t],
        vnc_cv_inaddrt, [
                AC_TRY_COMPILE([#include <sys/types.h>
#include <netinet/in.h>],
                        [in_addr_t foo; return 0;],
                                        [inaddrt=yes],
                                        [inaddrt=no]),
        ])
AH_TEMPLATE(NEED_INADDR_T, [Need a typedef for in_addr_t])
if test $inaddrt = no ; then
  AC_DEFINE(NEED_INADDR_T)
fi
# Checks for library functions.
AC_FUNC_MEMCMP
AC_FUNC_STAT
AC_FUNC_STRFTIME
AC_FUNC_VPRINTF
AC_FUNC_FORK
AC_CHECK_LIB(nsl,gethostbyname)
AC_CHECK_LIB(socket,socket)

uname_s=`(uname -s) 2>/dev/null`
if test "x$uname_s" = "xHP-UX"; then
	# need -lsec for getspnam()
	LDFLAGS="$LDFLAGS -lsec"
fi

AC_CHECK_FUNCS([ftime gethostbyname gethostname gettimeofday inet_ntoa memmove memset mmap mkfifo select socket strchr strcspn strdup strerror strstr])
AC_CHECK_FUNCS([setsid setpgrp getpwuid getpwnam getspnam getuid geteuid setuid setgid seteuid setegid initgroups waitpid setutxent grantpt shmat])


# check, if shmget is in cygipc.a
AC_CHECK_LIB(cygipc,shmget)
AM_CONDITIONAL(CYGIPC, test "$HAVE_CYGIPC" = "true")

# Check if /usr/include/linux exists, if so, define LINUX
AM_CONDITIONAL(LINUX, test -d /usr/include/linux)

# Check for OS X specific header
AC_CHECK_HEADER(ApplicationServices/ApplicationServices.h, HAVE_OSX="true")
AM_CONDITIONAL(OSX, test "$HAVE_OSX" = "true")

# Check for Android specific header
AC_CHECK_HEADER(android/api-level.h, HAVE_ANDROID="true")
AM_CONDITIONAL(ANDROID, test "$HAVE_ANDROID" = "true")
if test "$HAVE_ANDROID" = "true"; then
	AC_DEFINE(HAVE_ANDROID, 1, [Android host system detected])
fi

# On Solaris 2.7, write() returns ENOENT when it really means EAGAIN
AH_TEMPLATE(ENOENT_WORKAROUND, [work around when write() returns ENOENT but does not mean it])
case `(uname -sr) 2>/dev/null` in
    "SunOS 5.7")
	AC_DEFINE(ENOENT_WORKAROUND)
    ;;
esac

# Check for rpm SOURCES path
printf "checking for rpm sources path... "
RPMSOURCEDIR="NOT-FOUND"
for directory in packages OpenLinux redhat RedHat rpm RPM "" ; do
	if test -d /usr/src/${directory}/SOURCES; then
		RPMSOURCEDIR="/usr/src/${directory}/SOURCES/"
	fi
done
echo "$RPMSOURCEDIR"
AM_CONDITIONAL(HAVE_RPM, test "$RPMSOURCEDIR" != "NOT-FOUND")
AM_CONDITIONAL(WITH_X11VNC,  test "$build_x11vnc" = "yes")
AC_SUBST(RPMSOURCEDIR)

AC_CONFIG_FILES([Makefile src/Makefile misc/Makefile misc/turbovnc/Makefile])

AC_OUTPUT


