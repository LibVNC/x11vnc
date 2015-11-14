SUBDIRS = turbovnc
DIST_SUBDIRS = turbovnc
EXTRA_DIST = README blockdpy.c dtVncPopup rx11vnc rx11vnc.pl shm_clear ranfb.pl slide.pl vcinject.pl x11vnc_loop Xdummy.in Xdummy.c ultravnc_repeater.pl connect_switch panner.pl desktop.cgi inet6to4 uinput.pl qt_tslib_inject.pl deskshot
bin_SCRIPTS = Xdummy
CLEANFILES = $(bin_SCRIPTS)

do_dummy_c_subst = $(SED) \
	-e '/Will be substituted by Xdummy\.c/d' \
	-e '/"XDUMMY_C"/r $(srcdir)/Xdummy.c'
Xdummy: $(srcdir)/Xdummy.in $(srcdir)/Xdummy.c
	$(do_dummy_c_subst) < $< > $@.tmp
	mv -f $@.tmp $@
