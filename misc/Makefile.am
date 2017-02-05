SUBDIRS = turbovnc

EXTRA_DIST = \
	blockdpy.c \
	connect_switch \
	deskshot \
	desktop.cgi \
	dtVncPopup \
	inet6to4 \
	panner.pl \
	qt_tslib_inject.pl \
	ranfb.pl \
	README \
	rx11vnc \
	rx11vnc.pl \
	shm_clear \
	slide.pl \
	uinput.pl \
	ultravnc_repeater.pl \
	vcinject.pl \
	x11vnc_loop \
	Xdummy.c \
	Xdummy.in

bin_SCRIPTS = Xdummy
CLEANFILES = $(bin_SCRIPTS)

do_dummy_c_subst = $(SED) \
	-e '/Will be substituted by Xdummy\.c/d' \
	-e '/"XDUMMY_C"/r $(srcdir)/Xdummy.c'

Xdummy: $(srcdir)/Xdummy.in $(srcdir)/Xdummy.c
	$(do_dummy_c_subst) < $< > $@.tmp
	mv -f $@.tmp $@
