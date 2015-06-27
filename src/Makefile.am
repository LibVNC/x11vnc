
LDADD = @LIBVNCSERVER_LIBS@ @LIBVNCCLIENT_LIBS@ @WSOCKLIB@

if OSX
FRAMEWORKS = -framework ApplicationServices -framework Carbon -framework IOKit -framework Cocoa
if OSX_OPENGL
FRAMEWORKS += -framework OpenGL
if HAVE_X11
GL = /System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib
FRAMEWORKS += -dylib_file $(GL):$(GL)
endif
endif
x11vnc_LDFLAGS = $(FRAMEWORKS)
x11vnc_CFLAGS = -ObjC
endif

if CYGIPC
LD_CYGIPC=-lcygipc
endif

bin_PROGRAMS=x11vnc
x11vnc_SOURCES = 8to24.c appshare.c avahi.c cleanup.c connections.c cursor.c gui.c help.c inet.c keyboard.c linuxfb.c macosx.c macosxCG.c macosxCGP.c macosxCGS.c macosx_opengl.c options.c pm.c pointer.c rates.c remote.c scan.c screen.c selection.c solid.c sslcmds.c sslhelper.c uinput.c unixpw.c user.c userinput.c util.c v4l.c win_utils.c x11vnc.c x11vnc_defs.c xdamage.c xevents.c xinerama.c xkb_bell.c xrandr.c xrecord.c xwrappers.c xi2_devices.c 8to24.h allowed_input_t.h avahi.h blackout_t.h cleanup.h connections.h cursor.h enc.h enums.h gui.h help.h inet.h keyboard.h linuxfb.h macosx.h macosxCG.h macosxCGP.h macosxCGS.h macosx_opengl.h nox11.h nox11_funcs.h options.h params.h pm.h pointer.h rates.h remote.h scan.h screen.h scrollevent_t.h selection.h solid.h sslcmds.h sslhelper.h ssltools.h tkx11vnc.h uinput.h unixpw.h user.h userinput.h util.h v4l.h win_utils.h winattr_t.h x11vnc.h xdamage.h xevents.h xinerama.h xkb_bell.h xrandr.h xrecord.h xwrappers.h default8x16.h xi2_devices.h

INCLUDES_LIBVNCSERVER = @LIBVNCSERVER_CFLAGS@ @LIBVNCCLIENT_CFLAGS@

AM_CPPFLAGS = $(INCLUDES_LIBVNCSERVER) @X_CFLAGS@ @AVAHI_CFLAGS@ @XI2_CFLAGS@ @CAIRO_CFLAGS@ @WAYLAND_CFLAGS@ @GDBUS_CFLAGS@

x11vnc_LDADD=$(LDADD) @SSL_LIBS@ @CRYPT_LIBS@ @X_LIBS@ @AVAHI_LIBS@ @XI2_LIBS@ @CAIRO_LIBS@ @WAYLAND_LIBS@ @GDBUS_LIBS@ $(LD_CYGIPC) 