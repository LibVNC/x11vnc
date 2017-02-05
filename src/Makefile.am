LDADD = \
	@LIBVNCSERVER_LIBS@ \
	@LIBVNCCLIENT_LIBS@ \
	@WSOCKLIB@

if OSX
FRAMEWORKS = \
	-framework ApplicationServices \
	-framework Carbon \
	-framework IOKit \
	-framework Cocoa

if OSX_OPENGL
FRAMEWORKS += -framework OpenGL
if HAVE_X11
GL = /System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib
FRAMEWORKS += -dylib_file $(GL):$(GL)
endif
endif

x11vnc_LDFLAGS = $(FRAMEWORKS)
x11vnc_CFLAGS = -ObjC
endif # OSX

if CYGIPC
LD_CYGIPC = -lcygipc
endif

bin_PROGRAMS = x11vnc
x11vnc_SOURCES = \
	8to24.c \
	8to24.h \
	allowed_input_t.h \
	appshare.c \
	avahi.c \
	avahi.h \
	blackout_t.h \
	cleanup.c \
	cleanup.h \
	connections.c \
	connections.h \
	cursor.c \
	cursor.h \
	default8x16.h \
	enc.h \
	gui.c \
	gui.h \
	help.c \
	help.h \
	inet.c \
	inet.h \
	keyboard.c \
	keyboard.h \
	linuxfb.c \
	linuxfb.h \
	macosx.c \
	macosxCG.c \
	macosxCG.h \
	macosxCGP.c \
	macosxCGP.h \
	macosxCGS.c \
	macosxCGS.h \
	macosx.h \
	macosx_opengl.c \
	macosx_opengl.h \
	nox11_funcs.h \
	nox11.h \
	options.c \
	options.h \
	params.h \
	pm.c \
	pm.h \
	pointer.c \
	pointer.h \
	rates.c \
	rates.h \
	remote.c \
	remote.h \
	scan.c \
	scan.h \
	screen.c \
	screen.h \
	scrollevent_t.h \
	selection.c \
	selection.h \
	solid.c \
	solid.h \
	sslcmds.c \
	sslcmds.h \
	sslhelper.c \
	sslhelper.h \
	ssltools.h \
	tkx11vnc.h \
	uinput.c \
	uinput.h \
	unixpw.c \
	unixpw.h \
	user.c \
	user.h \
	userinput.c \
	userinput.h \
	util.c \
	util.h \
	v4l.c \
	v4l.h \
	winattr_t.h \
	win_utils.c \
	win_utils.h \
	x11vnc.c \
	x11vnc_defs.c \
	x11vnc.h \
	xdamage.c \
	xdamage.h \
	xevents.c \
	xevents.h \
	xi2_devices.c \
	xi2_devices.h \
	xinerama.c \
	xinerama.h \
	xkb_bell.c \
	xkb_bell.h \
	xrandr.c \
	xrandr.h \
	xrecord.c \
	xrecord.h \
	xwrappers.c \
	xwrappers.h

INCLUDES_LIBVNCSERVER = \
	@LIBVNCSERVER_CFLAGS@ \
	@LIBVNCCLIENT_CFLAGS@

AM_CPPFLAGS = \
	$(INCLUDES_LIBVNCSERVER) \
	@X_CFLAGS@ \
	@AVAHI_CFLAGS@ \
	@XI2_CFLAGS@ \
	@CAIRO_CFLAGS@

x11vnc_LDADD = \
	$(LDADD) \
	@SSL_LIBS@ \
	@CRYPT_LIBS@ \
	@X_LIBS@ \
	@AVAHI_LIBS@ \
	@XI2_LIBS@ \
	@CAIRO_LIBS@ \
	$(LD_CYGIPC)
