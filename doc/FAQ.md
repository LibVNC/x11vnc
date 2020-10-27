# x11vnc FAQ

## Building and Starting

### Q-1: I can't get x11vnc to start up. It says "XOpenDisplay failed (null)" or "Xlib: connection to ":0.0" refused by server Xlib: No protocol specified" and then exits. What do I need to do?

For the former error, you need to specify the X display to connect to
(it also needs to be on the same machine the x11vnc process is to run
on.) Set your DISPLAY environment variable (or use the -display
option) to specify it. Nearly always the correct value will be ":0"
(in fact, x11vnc will now assume :0 if given no other information.)

For the latter error, you need to set up the X11 permissions
correctly.

To make sure X11 permissions are the problem do this simple test:
while sitting at the physical X display open a terminal window
(gnome-terminal, xterm, etc.) You should be able to run x11vnc
successfully without any need for special steps or command line
options in that terminal (i.e. just type "x11vnc".) If that works OK
then you know X11 permissions are the only thing preventing it from
working when you try to start x11vnc via, say, a remote shell.

How to Solve:  See the xauth(1), Xsecurity(7), and xhost(1) man pages
or this Howto for much info on X11 permissions. For example, you may
need to set your XAUTHORITY environment variable (or use the -auth
option) to point to the correct MIT-MAGIC-COOKIE file (e.g.
/home/joe/.Xauthority or /var/gdm/:0.Xauth or /var/lib/kdm/A:0-crWk72K
or /tmp/.gdmzndVlR, etc, etc.), or simply be sure you run x11vnc as
the correct user (i.e. the user who is logged into the X session you
wish to view.)

Note: The MIT cookie file contains the secret key that allows x11vnc
to connect to the desired X display.

If, say, sshd has set XAUTHORITY to point to a random file it has
created for X forwarding that will cause problems. (Under some
circumstances even su(1) and telnet(1) can set XAUTHORITY. See also
the gdm parameter NeverPlaceCookiesOnNFS that sets XAUTHORITY to a
random filename in /tmp for the whole X session.)

Running x11vnc as root is often not enough: you need to know where the
MIT-MAGIC-COOKIE file for the desired X display is.

Example solution:

```
x11vnc -display :0 -auth /var/gdm/:0.Xauth
```

(this is for the display manager gdm and requires root permission to
read the gdm cookie file, see this faq for other display manager
cookie file names.)

Note as of Feb/2007 you can also try the -find option instead of
"-display ..." and see if that finds your display and Xauthority.

Less safe, but to avoid figuring out where the correct XAUTHORITY file
is, if the person sitting at the physical X session types "xhost
+localhost" then one should be able to attach x11vnc to the session
(from the same machine.) The person could then type "xhost -localhost"
after x11vnc has connected to go back to the default permissions.
Also, for some situations the "-users lurk=" option may soon be of use
(please read the documentation on the -users option.)

To test out your X11 permissions from a remote shell, set DISPLAY and
possibly XAUTHORITY (see your shell's man page, bash(1), tcsh(1), on
how to set environment variables) and type xdpyinfo in the same place
you will be typing (or otherwise running) x11vnc. If information is
printed out about the X display (screen sizes, supported extensions,
color visuals info) that means the X11 permissions are set up
properly: xdpyinfo successfully connected to DISPLAY! You could also
type xclock and make sure no errors are reported (a clock should
appear on the X display, press Ctrl-C to stop it.) If these work, then
typing "x11vnc" in the same environment should also work.

Important: if you cannot get your X11 permissions so that the xdpyinfo
or xclock tests work, x11vnc also will not work (all of these X
clients must be allowed to connect to the X server to function
properly.)

Firewalls: Speaking of permissions, it should go without saying that
the host-level firewall will need to be configured to allow
connections in on a port. E.g. 5900 (default VNC port) or 22 (default
SSH port for tunnelling VNC.) Most systems these days have firewalls
turned on by default, so you will actively have to do something to
poke a hole in the firewall at the desired port number. See your
system administration tool for Firewall settings (Yast, Firestarter,
etc.)

### Q-2: I can't get x11vnc and/or libvncserver to compile.

Make sure you have gcc (or other C compiler) and all of the required
libraries and the corresponding -dev/-devel packages installed. These
include Xorg/XFree86, libX11, libjpeg, libz, libssl, ... and don't
forget the devs: libjpeg-dev, libssl-dev ...

The most common build problem that people encounter is that the
necessary X11 libraries are installed on their system however it does
not have the corresponding -dev/-devel packages installed. These dev
packages include C header files and build-time .so symlink. It is a
shame the current trend in distros is to not install the dev package
by default when the the library runtime package is installed... (it
diminishes the power of open source)

As of Nov/2006 here is a list of libraries that x11vnc usually likes
to use:

```
libc.so        libX11.so       libXtst.so       libXext.so
libXfixes.so   libXdamage.so   libXinerama.so   libXrandr.so
libz.so        libjpeg.so      libpthread.so
libssl.so      libcrypto.so    libcrypt.so
```

although x11vnc will be pretty usable with the subset: libc.so,
libX11.so, libXtst.so, libXext.so, libz.so, and libjpeg.so.

After running the libvncserver configure, carefully examine the output
and the messages in the config.log file looking for missing
components. For example, if the configure output looks like:

```
checking how to run the C preprocessor... gcc -E
checking for X... no
checking for XkbSelectEvents in -lX11... no
checking for XineramaQueryScreens in -lXinerama... no
checking for XTestFakeKeyEvent in -lXtst... no
```

or even worse:

```
checking for C compiler default output file name... configure: error:
C compiler cannot create executables
See `config.log' for more details.
```

there is quite a bit wrong with the build environment. Hopefully
simply adding -dev packages and/or gcc or make will fix it.

For Debian the list seems to be:

```
gcc
make
libc6-dev
libjpeg8-dev           (formerly libjpeg62-dev)
libx11-dev
x11proto-core-dev      (formerly x-dev)
libxext-dev
libxtst-dev
libxdamage-dev
libxfixes-dev
libxrandr-dev
libxinerama-dev
libxss-dev             (formerly xlibs-static-dev)
zlib1g-dev
libssl-dev
libavahi-client-dev
linux-libc-dev         (only needed for linux console rawfb support)
```

Note that depending on your OS version the above names may have been
changed and/or additional packages may be needed.

For Redhat the list seems to be:

```
gcc
make
glibc-devel
libjpeg-devel
libX11-devel
xorg-x11-proto-devel
libXdamage-devel
libXfixes-devel
libXrandr-devel
zlib-devel
openssl-devel
avahi-devel
kernel-headers         (only needed for linux console rawfb support)
```

For other distros or OS's the package names may not be the same but
will look similar. Also, distros tend to rename packages as well so
the above list may be out of date. So only use the above lists as
hints for the package names that are needed.

Have a look at Misc. Build Problems for additional fixes.

Note: there is growing trend in Linux and other distros to slice up
core X11 software into more and smaller packages. So be prepared for
more headaches compiling software...

### Q-3: I just built x11vnc successfully, but when I use it my keystrokes and mouse button clicks are ignored  (I am able to move the mouse though.)

This is most likely due to you not having a working build environment
for the XTEST client library libXtst.so. The library is probably
present on your system, but the package installing the build header
file is missing.

If you were watching carefully while configure was running you would
have seen:
checking for XTestFakeKeyEvent in -lXtst... no

The solution is to add the necessary build environment package (and
the library package if that is missing too.) On Debian the build
package is libxtst-dev. Other distros/OS's may have it in another
package.

x11vnc will build without support for this library (e.g. perhaps one
wants a view-only x11vnc on a stripped down or embedded system...) And
at runtime it will also continue to run even if the X server it
connects to does not support XTEST. In both cases it cannot inject
keystrokes or button clicks since XTEST is needed for that (it can
still move the mouse pointer using the X API XWarpPointer().)

You will see a warning message something like this at run time:

```
20/03/2005 22:33:09 WARNING: XTEST extension not available (either missing from
20/03/2005 22:33:09   display or client library libXtst missing at build time.)
20/03/2005 22:33:09   MOST user input (pointer and keyboard) will be DISCARDED.
20/03/2005 22:33:09   If display does have XTEST, be sure to build x11vnc with
20/03/2005 22:33:09   a working libXtst build environment (e.g. libxtst-dev,
20/03/2005 22:33:09   or other packages.)
20/03/2005 22:33:09 No XTEST extension, switching to -xwarppointer mode for
20/03/2005 22:33:09   pointer motion input.
```

Also, as of Nov/2006 there will be a configure build time warning as
well:

```
...
checking for XFixesGetCursorImage in -lXfixes... yes
checking for XDamageQueryExtension in -lXdamage... yes
configure: WARNING:
==========================================================================
A working build environment for the XTEST extension was not found (libXtst).
An x11vnc built this way will be only barely usable.  You will be able to
move the mouse but not click or type.  There can also be deadlocks if an
application grabs the X server.

It is recommended that you install the necessary development packages
for XTEST (perhaps it is named something like libxtst-dev) and run
configure again.
==========================================================================
```

### Q-4: Help, I need to run x11vnc on Solaris 2.5.1 (or other old Unix/Linux) and it doesn't compile!

We apologize that x11vnc does not build cleanly on older versions of
Solaris, Linux, etc.: very few users are on these old releases.

We have heard that since Dec/2004 a Solaris 2.6 built x11vnc will run
on Solaris Solaris 2.5 and 2.5.1 (since a workaround for XConvertCase
is provided.)

In any event, here is a workaround for Solaris 2.5.1 (and perhaps
earlier and perhaps non-Solaris):

First use the environment settings (CPPFLAGS, LDFLAGS, etc.) in the
above Solaris build script to run the configure command. That should
succeed without failure. Then you have to hand edit the autogenerated
rfb/rfbconfig.h file in the source tree, and just before the last
#endif at the bottom of that file insert these workaround lines:

```
struct timeval _tmp_usleep_tv;
#define usleep(x) \
    _tmp_usleep_tv.tv_sec  = (x) / 1000000; \
    _tmp_usleep_tv.tv_usec = (x) % 1000000; \
    select(0, NULL, NULL, NULL, &_tmp_usleep_tv);
int gethostname(char *name, int namelen);
long random();
int srandom(unsigned int seed);
#undef LIBVNCSERVER_HAVE_LIBPTHREAD
#define SHUT_RDWR 2
typedef unsigned int in_addr_t;
#define snprintf(a, n, args...) sprintf((a), ## args)
```

Then run make with the Solaris build script environment, everything
should compile without problems, and the resulting x11vnc binary
should work OK. If some non-x11vnc related programs fail (e.g. test
programs) and the x11vnc binary is not created try "make -k" to have
it keep going. Similar sorts of kludges in rfb/rfbconfig.h can be done
on other older OS (Solaris, Linux, ...) releases.

Here are some notes for similar steps that need to be done to build on
SunOS 4.x

Please let us know if you had to use the above workaround (and whether
it worked or not.) If there is enough demand we will try to push clean
compilations back to earlier Solaris, Linux, etc, releases.

### Q-5: Where can I get a precompiled x11vnc binary for my Operating System?

Hopefully the build steps above and FAQ provide enough info for a
painless compile for most environments. Please report problems with
the x11vnc configure, make, etc. on your system (if your system is
known to compile other GNU packages successfully.)

There are precompiled x11vnc binaries built by other groups that are
available at the following locations:

* Slackware: (.tgz)  http://www.linuxpackages.net/
* SuSE: (.rpm) http:/software.opensuse.org/
* Gentoo: (info)
  http://gentoo-wiki.com/ and http://gentoo-portage.com/
* FreeBSD: (.tbz)
  http://www.freebsd.org/ http://www.freshports.org/net/x11vnc
* NetBSD: (src) http://pkgsrc.se/x11/x11vnc
* OpenBSD: (.tgz) http://openports.se/
* Arch Linux: (.tgz) http://www.archlinux.org/
* Nokia 770 (.deb) http://mike.saunby.googlepages.com/x11vncfornokia7702
* Sharp Zaurus: http://www.focv.com/
* Debian: (.deb) http://packages.debian.org/x11vnc
* Redhat/Fedora: (.rpm) http://packages.sw.be/x11vnc RPMforge
  http://dag.wieers.com/rpm/packages/x11vnc/ (N.B.: unmaintained after
  0.9.3)
* Solaris: (pkg) http://www.sunfreeware.com/

If the above binaries don't work and building x11vnc on your OS fails
(and all else fails!) you can try one of My Collection of x11vnc
Binaries for various OS's and x11vnc releases.

As a general note, the x11vnc program is simple enough you don't
really need to install a package: the binary will in most cases work
as is and from any location (as long as your system libraries are not
too old, etc.) So, for Linux distributions that are not one of the
above, the x11vnc binary from the above packages has a good chance of
working. You can "install" it by just copying the x11vnc binary to the
desired directory in your PATH. Tip on extracting files from a Debian
package: extract the archive via a command like: "ar x
x11vnc_0.6-2_i386.deb" and then you can find the binary in the
resulting data.tar.gz tar file. Also, rpm2cpio(1) is useful in
extracting files from rpm packages.

If you use a standalone binary like this and also want x11vnc to serve
up the Java VNC Viewer jar file (either SSL enabled or regular one),
then you will need to extract the classes subdirectory from the source
tarball and point x11vnc to it via the -httpdir option. E.g.:
x11vnc -httpdir /path/to/x11vnc-0.9.9/classes/ssl ...

Alternatively, you can also go for a more up-to-date approach and use
the very decent noVNC viewer (https://kanaka.github.io/noVNC/) that is
purely HTML5 and does not need any plugins at all.


### Q-6: Where can I get a VNC Viewer binary (or source code) for the Operating System I will be viewing from?

To obtain VNC viewers for the viewing side (Windows, Mac OS, or Unix)
try here:

* http://www.tightvnc.com/download.html
* http://www.realvnc.com/download-free.html
* http://sourceforge.net/projects/cotvnc/
* http://www.ultravnc.com/
* Our Enhanced TightVNC Viewer (SSVNC)[ssvnc.gif]

### Q-7: How can I see all of x11vnc's command line options and documentation on how to use them?

Run:  x11vnc -opts   to list just the option names or run:  x11vnc
-help   for long descriptions about each option. The output is listed
here as well. Yes, x11vnc does have a lot of options, doesn't it...

### Q-8: I don't like typing arcane command line options every time I start x11vnc. What can I do? Is there a config file? Or a GUI?

You could create a shell script that calls x11vnc with your options:

```
#!/bin/sh
#
# filename: X11vnc  (i.e. not "x11vnc")
# It resides in a directory in $PATH. "chmod 755 X11vnc" has been run on it.
#
x11vnc -wait 50 -localhost -rfbauth $HOME/.vnc/passwd -display :0 $*
```

a similar thing can be done via aliases in your shell (bash, tcsh,
csh, etc..)

Or as of Jun/2004 you can use the simple $HOME/.x11vncrc config file
support. If that file exists, each line is taken as a command line
option. E.g. the above would be:

```
# this is a comment in my ~/.x11vncrc file
wait 50        # this is a comment to the end of the line.
-localhost     # note: the leading "-" is optional.
rfbauth  /home/fred/.vnc/passwd
display :0
```

As of Dec/2004 there is now a simple Tcl/Tk GUI based on the
remote-control functionality ("-R") that was added. The /usr/bin/wish
program is needed for operation. The gui is not particularly
user-friendly, it just provides a point and click mode to set all the
many x11vnc parameters and obtain help on them. It is also very useful
for testing. See the -gui option for more info. Examples: "x11vnc ...
-gui" and "x11vnc ... -gui other:0" in the latter case the gui is
displayed on other:0, not the X display x11vnc is polling. There is
also a "-gui tray" system tray mode.

[tkx11vnc.gif]

NOTE: You may need to install the "wish" or "tk" or "tk8.4" package
for the gui mode to work (the package name depends on your OS/distro.)
The tcl/tk "wish" interpreter is used. In debian (and so ubuntu too)
one would run "apt-get install tk" or perhaps "apt-get install tk8.4"

### Q-9: How can I get the GUI to run in the System Tray, or at least be a smaller, simpler icon?

As of Jul/2005 the gui can run in a more friendly small icon mode
"-gui icon" or in the system tray: "-gui tray". It has balloon status,
a simple menu, and a Properities dialog. The full, complicated, gui is
only available under "Advanced". Other improvements were added as
well. Try "Misc -> simple_gui" for a gui with fewer esoteric menu
items.

If the gui fails to embed itself in the system tray, do a retry via
"Window View -> icon" followed by "Window View -> tray" with the popup
menu.

For inexperienced users starting up x11vnc and the GUI while sitting
at the physical X display (not remotely), using something like "x11vnc
-display :0 -gui tray=setpass" might be something for them that they
are accustomed to in a Desktop environment (it prompts for an initial
password, etc.) This is a basic "Share My Desktop" usage mode.

As of Nov/2008 in x11vnc 0.9.6 there is a desktop menu item
(x11vnc.desktop) that runs this command:

```
x11vnc -gui tray=setpass -rfbport PROMPT -logfile %HOME/.x11vnc.log.%VNCDISPLAY
```

which also prompts for which VNC port to use and a couple other
parameters.


### Q-10: How can I get x11vnc to listen on a different port besides the default VNC port (5900)?

Use something like, e.g., "x11vnc -rfbport 5901" to force it to use
port 5901 (this is VNC display :1.) If something else is using that
port x11vnc will exit immediately. If you do not supply the -rfbport
option, it will autoprobe starting at 5900 and work its way up to 5999
looking for a free port to listen on. In that case, watch for the
PORT=59xx line to see which port it found, then subtract 5900 from it
for the VNC display number to enter into the VNC Viewer(s).

The "-N" option will try to match the VNC display number to the X
display (e.g. X11 DISPLAY of :5 (port 6005) will have VNC display :5
(port 5905).)

Also see the "-autoport n" option to indicated at which value the auto
probing should start at.


### Q-11: My Firewall/Router doesn't allow VNC Viewers to connect to x11vnc.

See the Firewalls/Routers discussion.


### Q-12: Is it possible for a VNC Viewer and a VNC Server to connect to each other even though both are behind Firewalls that block all incoming connections?

This is very difficult or impossible to do unless a third machine,
reachable by both, is used as a relay. So we assume a third machine is
somehow being used as a relay.

(Update: It may be possible to do "NAT-2-NAT" without a relay machine
by using a UDP tunnel such as http://samy.pl/pwnat/. All that is
required is that both NAT firewalls allow in UDP packets from an IP
address to which a UDP packet has recently been sent to. If you try it
out let us know how it went.)

In the following discussion, we will suppose port 5950 is being used
on the relay machine as the VNC port for the rendezvous.

A way to rendezvous is to have the VNC Server start a reverse
connection to the relay machine:
x11vnc -connect third-machine.net:5950 ...

and the VNC viewer forward connects as usual:
vncviewer third-machine.net:50

Or maybe two ports would be involved, e.g. the viewer goes to display
:51 (5951.) It depends on the relay software being used.

What software to run on third-machine? A TCP relay of some sort could
be used... Try a google search on "tcp relay" or "ip relay". However,
note that this isn't a simple redirection because it hooks up two
incoming connections. You can look at our UltraVNC repeater
implementation ultravnc_repeater.pl for ideas and possibly to
customize.

Also, if you are not the admin of third-machine you'd have to convince
the owner to allow you to install this software (and he would likely
need to open his server's firewall to allow the port through.)

It is recommended that SSL is used for encryption (e.g. "-ssl SAVE")
when going over the internet.

We have a prototype for performing a rendezvous via a Web Server
acting as the relay machine. Download the vncxfer CGI script and see
the instructions at the top.

Once that CGI script is set up on the website, both users go to, say,
http://somesite.com/vncxfer (or maybe a "/cgi-bin" directory or ".cgi"
suffix must be used.) Previously, both have agreed on the same session
name (say by phone or email) , e.g. "5cows", and put that into the
entry form on the vncxfer starting page (hopefully separated by a few
seconds, so the relay helper can fully start up at the first request.)

The page returned tells them the hostname and port number and possible
command to use for forward (VNC Viewer) and reverse (VNC Server, i.e.
x11vnc) connections as described above.

Also since Oct/2007, x11vnc can connect directly (no web browser),
like this:

```
x11vnc ... -connect localhost:0 -proxy 'http://somesite.com/vncxfer?session=5cows&'
```

Unfortunately the prototype requires that the Web server's firewall
allow in the port (e.g. 5950) used for the rendezvous. Most web
servers are not configured to do this, so you would need to ask the
admin to do this for you. Nearly all free webspace sites, e.g.
www.zendurl.com, will not allow your CGI script to be an open relay
like this. (If you find one that does allow this, let me know!)

Maybe someday a clever trick will be thought up to relax the listening
port requirement (e.g. use HTTP/CGI itself for the transfer... it is
difficult to emulate a full-duplex TCP connection with them.)

See also the Firewalls/Routers discussion and Reverse Connection Proxy
discussion.

SSH method: If both users (i.e. one on Viewer-side and the other on
x11vnc server side) have SSH access to a common machine on the
internet (or otherwise mutually reachable), then SSH plumbing can be
used to solve this problem. The users create SSH tunnels going through
the SSH login machine.

Instead of assuming port 5900 is free on the SSH machine, we will
assume both users agreed to use 5933. This will illustrate how to use
a different port for the redir. It could be any port, what matters is
that both parties refer to the same one.

Set up the Tunnel from the VNC Server side:

```
ssh -t -R 5933:localhost:5900 user@third-machine.net
```

Set up the Tunnel from the VNC Viewer side:

```
ssh -t -L 5900:localhost:5933 user@third-machine.net
```

   Run Server on the VNC Server side:

```
x11vnc -rfbport 5900 -localhost ...
```

   Run Viewer on the VNC Viewer side:

```
vncviewer -encodings "copyrect tight zrle hextile" localhost:0
```

(we assume the old-style -encodings option needs to be used. See here
for details.)

If the SSH machine has been configured (see sshd_config(5)) with the
option GatewayPorts=yes, then the tunnel set up by the VNC Server will
be reachable directly by the VNC viewer (as long as the SSH machine's
firewall does not block the port, 5933 in this example.) So in that
case the Viewer side does not need to run any ssh command, but rather
only runs:
vncviewer third-machine.net:33

In this case we recommend SSL be used for encryption.

The creation of both tunnels can be automated. As of Oct/2007 the -ssh
x11vnc option is available and so only this command needs to be run on
the VNC Server side:
x11vnc -ssh user@third-machine.net:33 ...

(the SSH passphrase may need to be supplied.)

To automate on the VNC Viewer side, the user can use the Enhanced
TightVNC Viewer (SSVNC) by:

* Clicking on 'Use SSH'
* Entering user@third-machine.net:33 into 'VNC Host:Display' entry
  box
* Clicking on 'Connect'

As above, if the SSH GatewayPorts=yes setting is configured the Viewer
side doesn't need to create a SSH tunnel. In SSVNC the Viewer user
could instead select 'Use SSL' and then, e.g., on the Server side
supply "-ssl SAVE" to x11vnc. Then end-to-end SSL encryption would be
used (in addition to the SSH encryption on the Server-side leg.)


### Q-13: Can I make x11vnc more quiet and also go into the background after starting up?

Use the -q and -bg options, respectively.  (also: -quiet is an alias
for -q)

Note that under -bg the stderr messages will be lost unless you use
the "-o logfile" option.


### Q-14: Sometimes when a VNC viewer dies abruptly, x11vnc also dies with the error message like: "Broken pipe". I'm using the -forever mode and I want x11vnc to keep running.

As of Jan/2004 the SIGPIPE signal is ignored. So if a viewer client
terminates abruptly, libvncserver will notice on the next I/O
operation and will close the connection and continue on.

Up until of Apr/2004 the above fix only works for BSD signal systems
(Linux, FreeBSD, ...) For SYSV systems there is a workaround in place
since about Jun/2004.


### Q-15: The Windows TightVNC 1.3.9 Viewer cannot connect to x11vnc.

This appears to be fixed in x11vnc version 0.9 and later. If you need
to use an earlier version of x11vnc, try using the "-rfbversion 3.7"
option. In general sometimes one can get a misbehaving viewer to work
by supplying rfb versions 3.7 or 3.3.


### Q-16: KDE's krdc VNC viewer cannot connect to x11vnc.

This has been fixed in x11vnc version 0.8.4. More info here, here, and
here.


### Q-17: When I start x11vnc on an Alpha Tru64 workstation the X server crashes!

This is a bug in the X server obviously; an X client should never be
able to crash it.

The problem seems to be with the RECORD X extension and so a
workaround is to use the "-noxrecord" x11vnc command line option.


### Q-18: When running x11vnc on an IBM AIX workstation after a few minutes the VNC connection freezes.

One user reports when running x11vnc on AIX 5.3 in his CDE session
after a few minutes or seconds x11vnc will "freeze" (no more updates
being sent, etc.) The freezing appeared to be worse for versions later
than 0.9.2.

The problem seems to be with the RECORD X extension on AIX and so a
workaround is to use the "-noxrecord" x11vnc command line option. The
user found no freezes occurred when using that option.


### Q-19: Are there any build-time customizations possible, e.g. change defaults, create a smaller binary, etc?

There are some options. They are enabled by adding something like
-Dxxxx=1 to the CPPFLAGS environment variable before running configure
(see the build notes for general background.)

```
/*
 * Mar/2006
 * Build-time customization via CPPFLAGS.
 *
 * Summary of options to include in CPPFLAGS for custom builds:
 *
 * -DVNCSHARED  to have the vnc display shared by default.
 * -DFOREVER  to have -forever on by default.
 * -DNOREPEAT=0  to have -repeat on by default.
 * -DADDKEYSYMS=0  to have -noadd_keysyms the default.
 *
 * -DREMOTE_DEFAULT=0  to disable remote-control on by default (-yesremote.)
 * -DREMOTE_CONTROL=0  to disable remote-control mechanism completely.
 * -DEXTERNAL_COMMANDS=0  to disable the running of all external commands.
 * -DFILEXFER=0  disable filexfer.
 *
 * -DHARDWIRE_PASSWD=...      hardwired passwords, quoting necessary.
 * -DHARDWIRE_VIEWPASSWD=...
 * -DNOPW=1                   make -nopw the default (skip warning)
 * -DUSEPW=1                  make -usepw the default
 * -DPASSWD_REQUIRED=1        exit unless a password is supplied.
 * -DPASSWD_UNLESS_NOPW=1     exit unless a password is supplied and no -nopw.
 *
 * -DWIREFRAME=0  to have -nowireframe as the default.
 * -DWIREFRAME_COPYRECT=0  to have -nowirecopyrect as the default.
 * -DWIREFRAME_PARMS=...   set default -wirecopyrect parameters.
 * -DSCROLL_COPYRECT=0     to have -noscrollcopyrect as the default.
 * -DSCROLL_COPYRECT_PARMS=...  set default -scrollcopyrect parameters.
 * -DSCALING_COPYRECT=0
 * -DXDAMAGE=0    to have -noxdamage as the default.
 * -DSKIPDUPS=0   to have -noskip_dups as the default or vice versa.
 *
 * -DPOINTER_MODE_DEFAULT={0,1,2,3,4}  set default -pointer_mode.
 * -DBOLDLY_CLOSE_DISPLAY=0  to not close X DISPLAY under -rawfb.
 * -DSMALL_FOOTPRINT=1  for smaller binary size (no help, no gui, etc)
 *                      use 2 or 3 for even smaller footprint.
 * -DNOGUI  do not include the gui tkx11vnc.
 * -DPOLL_8TO24_DELAY=N
 * -DDEBUG_XEVENTS=1  enable printout for X events.
 *
 * Set these in CPPFLAGS before running configure. E.g.:
 *
 *   % env CPPFLAGS="-DFOREVER -DREMOTE_CONTROL=0" ./configure
 *   % make
 */
```

If other things (e.g. "-I ...") are needed in CPPFLAGS add them as
well.

On some systems is seems you need to set LC_ALL=C for configure to
work properly...

Be careful the following two variables: HARDWIRE_PASSWD and
HARDWIRE_VIEWPASSWD. If set (remember to include the double quotes
around the string), they will be used as default values for the
-passwd and -viewpasswd options. Of course the strings will exist
unobscured in the x11vnc binary: it better not be readable by
unintendeds. Perhaps this is of use in remote access for an embedded
application, etc...

Let us know if more build-time customizations would be useful.


## Win2VNC Related

### Q-20: I have two separate machine displays in front of me, one Windows the other X11: can I use x11vnc in combination with Win2VNC in dual-screen mode to pass the keystrokes and mouse motions to the X11 display?

Yes, for best response start up x11vnc with the "-nofb" option
(disables framebuffer polling, and does other optimizations) on the
secondary display (X11) machine. Then start up Win2VNC on the primary
display (Windows) referring it to the secondary display.

This will also work X11 to X11 using x2vnc, however you would probably
just want to avoid VNC and use x2x for that.

For reference, here are some links to Win2VNC-like programs for
multiple monitor setups:

* Original Win2VNC
* Enhanced Win2VNC (broken?) and sourceforge link
* x2vnc
* x2x
* zvnc (MorphOS)

All of them will work with x11vnc (except x2x where it is not needed.)


### Q-21: I am running Win2VNC on my Windows machine and "x11vnc -nofb" on Unix to pass keyboard and mouse to the Unix monitor. Whenever I start Win2VNC it quickly disconnects and x11vnc says: rfbProcessClientNormalMessage: read: Connection reset by peer

Is the default visual of the X display you run x11vnc on low color
(e.g. 8 bit per pixel PseudoColor)? (you can run xdpyinfo to check,
look in the "screen" section.) There seems to be a bug in Win2VNC in
that it cannot deal correctly with colormaps (PseudoColor is the most
common example of a visual with a colormap.)

If so, there are a couple options. 1) Can you set the default visual
on your display to be depth 24 TrueColor? Sun machines often have 8+24
overlay/multi-depth visuals, and you can make the default visual depth
24 TrueColor (see fbconfig(1) and Xsun(1).) 2) As of Feb/2004 x11vnc
has the -visual option to allow you to force the framebuffer visual to
whatever you want (this usually messes up the colors unless you are
very clever.) In this case, the option provides a convenient
workaround for the Win2VNC bug:

```
x11vnc -nofb -visual TrueColor -display :0 ...
```

So the visual will be set to 8bpp TrueColor and Win2VNC can handle
this. Since Win2VNC does not use the framebuffer data there should be
no problems in doing this.

### Q-22: Can I run "x11vnc -nofb" on a Mac OS X machine to redirect mouse and keyboard input to it from Windows and X11 machines via Win2VNC and x2vnc, respectively?

Yes, as of Nov/2006 you can. There may be a trick or two you'll need
to do to get the Clipboard exchange between the machines to work.



## Color Issues

### Q-23: The X display I run x11vnc on is only 8 bits per pixel (bpp) PseudoColor (i.e. only 256 distinct colors.) The x11vnc colors may start out OK, but after a while they are incorrect in certain windows.

Use the -flashcmap option to have x11vnc watch for changes in the
colormap, and propagate those changes back to connected clients. This
can be slow (since the whole screen must be updated over the network
whenever the colormap changes.) This flashing colormap behavior often
happens if an application installs its own private colormap when the
mouse is in its window. "netscape -install" is a well-known historical
example of this. Consider reconfiguring the system to 16 bpp or depth
24 TrueColor if at all possible.

Also note the option -8to24 (Jan/2006) can often remove the need for
flashing the colormap. Everything is dynamically transformed to depth
24 at 32 bpp using the colormaps. There may be painting errors however
(see the following FAQ for tips on reducing and correcting them.)

In some rare cases (SCO unixware) the -notruecolor option has
corrected colors on 8bpp displays. The red, green, and blue masks were
non-zero in 8bpp PseudoColor on an obscure setup, and this option
corrected the problems.


### Q-24: Color problems: Why are the colors for some windows incorrect in x11vnc? BTW, my X display has nice overlay/multi-depth visuals of different color depths: e.g. there are both depth 8 and 24 visuals available at the same time.

You may want to review the previous question regarding 8 bpp
PseudoColor.

On some hardware (Sun/SPARC and SGI), the -overlay option discussed a
couple paragraphs down may solve this for you (you may want to skip to
it directly.) On other hardware the less robust -8to24 option may help
(also discussed below.)

Run xdpyinfo(1) to see what the default visual is and what the depths
of the other visuals are. Does the default visual have a depth of 8
but there are other visuals of depth 24? If it does, can you possibly
re-configure your X server to make a depth 24 visual the default? If
you can do it, this will save you a lot of grief WRT colors and x11vnc
(and for general usage too!) Here is how I do this on an old
Sparcstation 20 running Solaris 9 with SX graphics

```
xinit -- -dev /dev/fb defclass TrueColor defdepth 24
```

and it works nicely (note: to log into console from the dtlogin
window, select "Options -> Command Line Login", then login and enter
the above command.) See the -dev section of the Xsun(1) manpage for a
description of the above arguments. If you have root permission, a
more permanent and convenient thing to do is to record the arguments
in a line like:

```
:0  Local local_uid@console root /usr/openwin/bin/Xsun -dev /dev/fb defclass TrueColor defdepth 24
```

in /etc/dt/config/Xservers (copy /usr/dt/config/Xservers.) Also look
at the fbconfig(1) and related manpages (e.g. ffbconfig, m64config,
pgxconfig, SUNWjfb_config, etc ...) for hardware framebuffer settings
that may achieve the same effect.

In general for non-Sun machines, look at the "-cc class" and related
options in your X server manpage (perhaps Xserver(1)), it may allow
modifying the default visual (e.g. "-cc 4", see <X11/X.h> for the
visual class numbers.) On XFree86 some video card drivers (e.g. Matrox
mga) have settings like Option "Overlay" "24,8" to support multi-depth
overlays. For these, use the "-cc 4" X server command line option to
get a depth 24 default visual.


The -overlay mode: Another option is if the system with overlay
visuals is a Sun system running Solaris or SGI running IRIX you can
use the -overlay x11vnc option (Aug/2004) to have x11vnc use the
Solaris XReadScreen(3X11) function to poll the "true view" of the
whole screen at depth 24 TrueColor. XReadDisplay(3X11) is used on
IRIX. This is useful for Legacy applications (older versions of
Cadence CAD apps are mentioned by x11vnc users) that require the
default depth be 8bpp, or the app will use a 8bpp visual even if depth
24 visuals are available, and so the default depth workaround
described in the previous paragraph is not sufficient for these apps.

It seems that Xorg is working toward supporting XReadDisplay(3X11) as
part of the RENDER extension work. When it does support it and
provides a library API x11vnc will be modified to take advantage of
the feature to support -overlay on Linux, *BSD, etc. Until then see
the -8to24 mode below.

Misc. notes on -overlay mode: An amusing by-product of -overlay mode
is that the mouse cursor shape is correct! (i.e. XFIXES is not
needed.) The -overlay mode may be somewhat slower than normal mode due
to the extra framebuffer manipulations that must be performed. Also,
on Solaris there is a bug in that for some popup menus, the windows
they overlap will have painting errors (flashing colors) while the
popup is up (a workaround is to disable SaveUnders by passing -su to
Xsun, e.g. in your /etc/dt/config/Xservers file.)


The -8to24 mode: The -8to24 x11vnc option (Jan/2006) is a kludge to
try to dynamically rewrite the pixel values so that the 8bpp part of
the screen is mapped onto depth 24 TrueColor. This is less robust than
the -overlay mode because it is done by x11vnc outside of the X
server. So only use it on OS's that do not support -overlay. The
-8to24 mode will work if the default visual is depth 24 or depth 8. It
scans for any windows within 3 levels of the root window that are 8bpp
(i.e. legacy application), or in general ones that are not using the
default visual. For the windows it finds it uses XGetSubImage() to
retrieve the pixels values and uses the correct indexed colormap to
create a depth 24 TrueColor view of the whole screen. This depth 24,
32bpp view is exported via VNC.

Even on pure 8bpp displays it can be used as an alternative to
-flashcmap to avoid color flashing completely.

This scheme is approximate and can often lead to painting errors. You
can manually correct most painting errors by pressing 3 Alt_L's in a
row, or by using something like: -fixscreen V=3.0 to automatically
refresh the screen every 3 seconds. Also -fixscreen 8=3.0 has been
added to just refresh the non-default visual parts of the screen.

In general the scheme uses many resources and may give rise to
sluggish behavior. If multiple windows are using different 8bpp
indexed colormaps all but one window may need to be iconified for the
colors to be correct. There are a number of tunable parameters to try
to adjust performance and painting accuracy. The option -8to24
nogetimage can give a nice speedup if the default depth 24 X server
supports hiding the 8bpp bits in bits 25-32 of the framebuffer data.
On very slow machines -8to24 poll=0.2,cachewin=5.0 gives an useful
speedup. See the -8to24 help description for information on tunable
parameters, etc.


Colors still not working correctly? Run xwininfo on the application
with the incorrect colors to verify that the depth of its visual is
different from the default visual depth (gotten from xdpyinfo.) One
possible workaround in this case is to use the -id option to point
x11vnc at the application window itself. If the application is
complicated (lots of toplevel windows and popup menus) this may not be
acceptable, and may even crash x11vnc (but not the application.) See
also -appshare.

It is theoretically possible to solve this problem in general (see
xwd(1) for example), but it does not seem trivial or sufficiently fast
for x11vnc to be able to do so in real time. The -8to24 method does
this approximately and is somewhat usable. Fortunately the -overlay
option works for Solaris machines with overlay visuals where most of
this problem occurs.


### Q-25: I am on a high color system (depth >= 24) but I seem to have colormap problems. They either flash or everything is very dark.

This can happen if the default Visual (use xdpyinfo to list them) is
DirectColor instead of TrueColor. These are both usually used in high
color modes, but whereas TrueColor uses static ramps for the Red,
Green, and Blue components, DirectColor has arbitrary colormaps for
the Red, Green, and Blue Components. Currently x11vnc cannot decode
these colormaps and treats them just like TrueColor.

The only workaround so far is to restart the X server with the "-cc 4"
option to force TrueColor as the default visual (DirectColor is "-cc
5"; see /usr/include/X11/X.h.) The only place we have seen this is
with the virtual framebuffer server Xvfb on Xorg 7.2. So in that case
you probably should restart it with something like this: "Xvfb :1 -cc
4 -screen 0 1280x1024x24". It should be possible for x11vnc to handle
DirectColor, but this hasn't been implemented due to its rare usage.

You may also see this problem on an X display with a TrueColor default
visual where an application chooses a DirectColor visual for its
window(s). It seems the application also needs to install its own
colormap for the visual for the colors to be messed up in x11vnc. One
can make xwud do this for example.


### Q-26: How do I figure out the window id to supply to the -id windowid option?

Run the xwininfo program in a terminal. It will ask you to click on
the desired application window. After clicking, it will print out much
information, including the window id (e.g. 0x6000010.) Also, the
visual and depth of the window printed out is often useful in
debugging x11vnc color problems.

Also, as of Dec/2004 you can use "-id pick" to have x11vnc run
xwininfo(1) for you and after you click the window it extracts the
windowid. Besides "pick" there is also "id:root" to allow you to go
back to root window when doing remote-control.


### Q-27: Why don't menus or other transient windows come up when I am using the -id windowid option to view a single application window?

This is related to the behavior of the XGetImage(3X11) and
XShmGetImage() interfaces regarding backingstore, saveunders, etc. The
way the image is retrieved depends on some aspects of how the X server
maintains the display image data and whether other windows are
clipping or obscuring it. See the XGetImage(3X11) man page for more
details. If you disable BackingStore and SaveUnders in the X server
you should be able to see these transient windows.

If things are not working and you still want to do the single window
polling, try the -sid windowid option ("shifted" windowid.)

Update: as of Nov/2009 in the 0.9.9 x11vnc development tarball, there
is an experimental Application Sharing mode that improves upon the
-id/-sid single window sharing: -appshare (run "x11vnc -appshare
-help" for more info.) It is still very primitive and approximate, but
at least it displays multiple top-level windows.


### Q-28: My X display is depth 24 at 24bpp (instead of the normal depth 24 at 32bpp.) I'm having lots of color and visual problems with x11vnc and/or vncviewer. What's up?

First off, depth 24 at 24bpp (bpp=bits-per-pixel) is fairly uncommon
and can cause problems in general. It also can be slower than depth 24
at 32bpp. You might want to switch to 32bpp (for XFree86 see the
"-fbbpp 32", DefaultFbBpp, FbBpp and related options.) Perhaps you
have 24bpp because the video memory of the machine is low and the
screen wouldn't fit in video RAM at 32bpp. For this case depth 16 at
16bpp might be an acceptable option.

In any event x11vnc should handle depth 24 at 24bpp (although
performance may be slower, and you may need to use the ZRLE encoding
instead of Tight.) There are some caveats involving the viewer
however:

The RealVNC Unix viewer cannot handle 24bpp from the server, it will
say: "main: setPF: not 8, 16 or 32 bpp?" and exit. I have not checked
the RealVNC Windows viewer.

So you need to use the TightVNC Unix viewer. However there are some
problems with that too. It seems libvncserver does not do 24bpp
correctly with the Tight encoding. The colors and screen ultimately
get messed up. So you have to use a different encoding with the
TightVNC vncviewer, try "zlib", "hextile", or one of the other
encodings (e.g. vncviewer -encodings "zlib hextile" ....) I have not
checked the TightVNC or UltraVNC Windows viewers.

It appears the older RealVNC Unix viewers (e.g. 3.3.3 and 3.3.7) can
handle 24bpp from the server, so you may want to use those. They
evidently request 32 bpp and libvncserver obliges.

Update: as of Apr/2006 you can use the -24to32 option to have x11vnc
dynamically transform the 24bpp pixel data to 32bpp. This extra
transformation could slow things down further however.

Now coming the opposite direction if you are running the vncviewer on
the 24bpp display, TightVNC will fail with "Can't cope with 24
bits-per-pixel. Sorry." and RealVNC will fail with "main: Error:
couldn't find suitable pixmap format" so evidently you cannot use
24bpp for the vncviewers to work on that X display.

Note, however, that the Unix viewer in the Enhanced TightVNC Viewer
(SSVNC) project can handle 24bpp X displays. It does this by
requesting a 16bpp pixel format (or 8bpp if the -bgr233 option has
been supplied) from the VNC server, and translates that to 24bpp
locally.

## Xterminals

### Q-29: Can I use x11vnc to view and interact with an Xterminal (e.g. NCD) that is not running UNIX and so x11vnc cannot be run on it directly?

You can, but it will likely be very wasteful of network bandwidth
since you will be polling the X display over the network as opposed to
over the local hardware. To do this, run x11vnc on a UNIX machine as
close as possible network-wise (e.g. same switch) to the Xterminal
machine. Use the -display option to point the display to that of the
Xterminal (you'll of course need basic X11 permission to do that) and
finally supply the -noshm option (this enables the polling over the
network.)

If the Xterminal's X display is open to the network for connections,
you might use something like "-display xterm123:0". If you are trying
to do this via an SSH tunnel (assuming you can actually ssh into the
Xterminal) it will be a little tricky (either use the ssh "-R" option
or consider ssh-ing in the other direction.) In all cases the X11
permissions need to allow the connection.

The response will likely be sluggish (maybe only one "frame" per
second.) This mode is not recommended except for "quick checks" of
hard to get to X servers. Use something like "-wait 150" to cut down
on the polling rate. You may also need -flipbyteorder if the colors
get messed up due to endian byte order differences.


### Q-30: How do I get my X permissions (MIT-MAGIC-COOKIE file) correct for a Unix/Linux machine acting as an Xterminal?

If the X display machine is a traditional Xterminal (where the X
server process runs on the Xterminal box, but all of the X client
applications (firefox, etc) run on a central server (aka "terminal
server")), you will need to log into the Xterminal machine (i.e. get a
shell running there) and then start the x11vnc program. If the
Xterminal Linux/Unix machine is stripped down (e.g. no users besides
root) that may be difficult.

The next problem is the login Display Manager (e.g. gdm, kdm), and
hence the MIT-MAGIC-COOKIE auth files, are on the central server and
not on the Xterminal box where the X server and x11vnc processes are.

So unless X permissions are completely turned off (e.g. "xhost +"), to
run the x11vnc process on the Xterminal box the MIT-MAGIC-COOKIE auth
file data (XAUTHORITY or $HOME/.Xauthority) must be accessible by or
copied to the Xterminal. If $HOME/.Xauthority is exported via NFS
(this is insecure of course, but has been going on for decades), then
x11vnc can simply pick it up via NFS (you may need to use the -auth
option to point to the correct file.) Other options include copying
the auth file using scp, or something like:

```
central-server>  xauth nextract - xterm123:0 | ssh xterm123 xauth nmerge -
```

and then, say, ssh from central-server to xterm123 to start x11vnc.
Here "xterm123" refers to the computer acting as the Xterminal and
"central-server" is the terminal server. You can use "xauth -f
/path/to/cookie-file list" to examine the contents of the cookie(s) in
a file "/path/to/cookie-file". See the xauth(1) manpage for more
details.

If the display name in the cookie file needs to be changed between the
two hosts, see this note on the "xauth add ..." command.

A less secure option is to run something like "xhost +127.0.0.1" while
sitting at the Xterminal box to allow cookie-free local access for
x11vnc. You can run "xhost -127.0.0.1" after x11vnc connects if you
want to go back to the original permissions.

If the Xterminal is really stripped down and doesn't have any user
accounts, NFS, etc. you'll need to contact your system administrator
to set something up. It can be done!!!  Some Xterminal projects have
actually enabled "run locally" facilities for the running of an
occasional app more efficiently locally on the Xterminal box (e.g.
realplayer.)

Not recommended, but as a last resort, you could have x11vnc poll the
Xterminal Display over the network. For this you would run a "x11vnc
-noshm ..." process on the central-server (and hope the network admin
doesn't get angry...)

Note: use of Display Manager (gdm, kdm, ...) auth cookie files (i.e.
from /var/...,  /tmp/..., or elsewhere) may require modification via
xauth(1) to correctly include the display x11vnc refers to (e.g.
"xauth -f cookie-file add :0 . 45be51ae2ce9dfbacd882ab3ef8e96b1",
where the "45be51..." cookie value was found from an "xauth -f
/path/to/original/cookie-file list") or other reasons. See xauth(1)
manpage for full details on how to transfer an MIT-MAGIC-COOKIE
between machines and displays.

VNCviewer performance on Xterminals:  This isn't related to x11vnc on
Xterminals, but we mention it here anyway because of the similar
issues. If you are on an Xterminal and want to use vncviewer to
connect to a VNC server somewhere, then performance would be best if
you ran the viewer on the Xterminal box. Otherwise, (i.e. running the
viewer process on the central-server) all of the vncviewer screen
drawing is done more inefficiently over the network. Something to
consider, especially on a busy network. (BTW, this has all of the
above permission, etc, problems: both vncviewer and x11vnc are X
client apps desired to be run on the Xterminal box.)

## Sun Rays

### Q-31: I'm having trouble using x11vnc with my Sun Ray session.

The Sun Ray technology is a bit like "VNC done in hardware" (the Sun
Ray terminal device, DTU, playing the role of the vncviewer.)
Completely independent of that, the SunRay user's session is still an
X server that speaks the X11 protocol and so x11vnc simply talks to
the X server part to export the SunRay desktop to any place in the
world (i.e. not only to a Sun Ray terminal device), creating a sort of
"Soft Ray". Please see this discussion of Sun Ray issues for solutions
to problems.

Also see the Sun Ray Remote Control Toolkit that uses x11vnc.


## Remote Control

### Q-32: How do I stop x11vnc once it is running in the background?

As of Dec/2004 there is a remote control feature. It can change a huge
number of parameters on the fly: see the -remote and -query options.
To shut down the running x11vnc server just type "x11vnc -R stop". To
disconnect all clients do "x11vnc -R disconnect:all", etc.

If the -forever option has not been supplied, x11vnc will
automatically exit after the first client disconnects. In general if
you cannot use the remote control, then you will have to kill the
x11vnc process This can be done via: "kill NNNNN" (where NNNNN is the
x11vnc process id number found from ps(1)), or "pkill x11vnc", or
"killall x11vnc" (Linux only.)

If you have not put x11vnc in the background via the -bg option or
shell & operator, then simply press Ctrl-C in the shell where x11vnc
is running to stop it.

Potential Gotcha: If somehow your Keypress of Ctrl-C went through
x11vnc to the Xserver that then delivered it to x11vnc it is possible
one or both of the Ctrl or C keys will be left stuck in the pressed
down state in the Xserver. Tapping the stuck key (either via a new
x11vnc or at the physical console) will release it from the stuck
state. If the keyboard seems to be acting strangely it is often fixed
by tapping Ctrl, Shift, and Alt. Alternatively, the -clear_mods option
and -clear_keys option can be used to release pressed keys at startup
and exit. The option -clear_all will also try to unset Caps_Lock,
Num_Lock, etc.


### Q-33: Can I change settings in x11vnc without having to restart it?  Can I remote control it?

Look at the -remote (an alias is -R) and -query (an alias is -Q)
options added in Dec/2004. They allow nearly everything to be changed
dynamically and settings to be queried. Examples: "x11vnc -R shared",
"x11vnc -R forever", "x11vnc -R scale:3/4", "x11vnc -Q modtweak",
"x11vnc -R stop", "x11vnc -R disconnect:all", etc..

These commands do not start a x11vnc server, but rather communicate
with one that is already running. The X display (X11VNC_REMOTE
property) is used as the communication channel, so the X permissions
and DISPLAY must be set up correctly for communication to be possible.

There is also a simple Tcl/Tk gui based on this remote control
mechanism. See the -gui option for more info. You will need to have
Tcl/Tk (i.e. /usr/bin/wish) installed for it to work. It can also run
in the system tray: "-gui tray" or as a standalone small icon window:
"-gui icon". Use "-gui tray=setpass" for a naive user "Share My
Desktop" mode.


## Security and Permissions

### Q-34: How do I create a VNC password for use with x11vnc?

You may already have one in $HOME/.vnc/passwd if you have used, say,
the vncserver program from the regular RealVNC or TightVNC packages
(i.e. launching the Xvnc server.) Otherwise, you could use the
vncpasswd(1) program from those packages.

As of Jun/2004 x11vnc supports the -storepasswd "pass" "file" option,
which is the same functionality of storepasswd. Be sure to quote the
"pass" if it contains shell meta characters, spaces, etc. Example:

```
x11vnc -storepasswd 'sword*fish' $HOME/myvncpasswd
```

You then use the password via the x11vnc option: "-rfbauth
$HOME/myvncpasswd"

As of Jan/2006 if you do not supply any arguments:

```
x11vnc -storepasswd
```

you will be prompted for a password to save to ~/.vnc/passwd (your
keystrokes when entering the password will not be echoed to the
screen.) If you supply one argument, e.g. "x11vnc -storepasswd
~/.mypass", the password you are prompted for will be stored in that
file.

x11vnc also has the -passwdfile and -passwd/-viewpasswd plain text
(i.e. not obscured like the -rfbauth VNC passwords) password options.

You can use the -usepw option to automatically use any password file
you have in ~/.vnc/passwd or ~/.vnc/passwdfile (the latter is used
with the -passwdfile option.)

```
x11vnc -usepw -display :0 ...
```

If neither file exists you are prompted to store a password in
~/.vnc/passwd. If a password file cannot be found or created x11vnc
exits immediately. An admin may want to set it up this way for users
who do not know better.


### Q-35: Can I make it so -storepasswd doesn't show my password on the screen?

You can use the vncpasswd program from RealVNC or TightVNC mentioned
above. As of Jan/2006 the -storepasswd option without any arguments
will not echo your password as you type it and save the file to
~/.vnc/passwd:

```
# x11vnc -storepasswd
Enter VNC password:
Verify password:
Write password to /home/myname/.vnc/passwd?  [y]/n
Password written to: /home/myname/.vnc/passwd
```

You can also give it an alternate filename, e.g. "x11vnc -storepasswd
~/.mypass"


### Q-36: Can I have two passwords for VNC viewers, one for full access and the other for view-only access to the display?

Yes, as of May/2004 there is the -viewpasswd option to supply the
view-only password. Note the full-access password option -passwd must
be supplied at the same time. E.g.: -passwd sword -viewpasswd fish.

To avoid specifying the passwords on the command line (where they
could be observed via the ps(1) command by any user) you can use the
-passwdfile option to specify a file containing plain text passwords.
Presumably this file is readable only by you, and ideally it is
located on the machine x11vnc is run on (to avoid being snooped on
over the network.) The first line of this file is the full-access
password. If there is a second line in the file and it is non-blank,
it is taken as the view-only password. (use "__EMPTY__" to supply an
empty one.)

View-only passwords currently do not work for the -rfbauth password
option (standard VNC password storing mechanism.) FWIW, note that
although the output (usually placed in $HOME/.vnc/passwd) by the
vncpasswd or storepasswd programs (or from x11vnc -storepasswd) looks
encrypted they are really just obscured to avoid "casual" password
stealing. It takes almost no skill to figure out how to extract the
plain text passwords from $HOME/.vnc/passwd since it is very
straight-forward to work out what to do from the VNC source code.


### Q-37: Can I have as many full-access and view-only passwords as I like?

Yes, as of Jan/2006 in the libvncserver CVS the -passwdfile option has
been extended to handle as many passwords as you like. You put the
view-only passwords after a line __BEGIN_VIEWONLY__.

You can also easily annotate and comment out passwords in the file.
You can have x11vnc re-read the file dynamically when it is modified.


### Q-38: Does x11vnc support Unix usernames and passwords? Can I further limit the set of Unix usernames who can connect to the VNC desktop?

Update: as of Feb/2006 x11vnc has the -unixpw option that does this
outside of the VNC protocol and libvncserver. The standard su(1)
program is used to validate the user's password. A familiar "login:"
and "Password:" dialog is presented to the user on a black screen
inside the vncviewer. The connection is dropped if the user fails to
supply the correct password in 3 tries or does not send one before a
25 second timeout. Existing clients are view-only during this period.
A list of allowed Unix usernames may also be supplied along with
per-user settings.

There is also the -unixpw_nis option for non-shadow-password
(typically NIS environments, hence the name) systems where the
traditional getpwnam() and crypt() functions are used instead of
su(1). The encrypted user passwords must be accessible to the user
running x11vnc in -unixpw_nis mode, otherwise the logins will always
fail even when the correct password is supplied. See ypcat(1) and
shadow(5).

Two settings are enforced in the -unixpw and -unixpw_nis modes to
provide extra security: the 1) -localhost and 2) -stunnel or -ssl
options. Without these one might send the Unix username and password
data in clear text over the network which is a very bad idea. They can
be relaxed if you want to provide encryption other than stunnel or
-ssl (the constraint is automatically relaxed if SSH_CONNECTION is set
and indicates you have ssh-ed in, however the -localhost requirement
is still enforced.)

The two -unixpw modes have been tested on Linux, Solaris, Mac OS X,
HP-UX, AIX, Tru64, FreeBSD, OpenBSD, and NetBSD. Additional testing is
appreciated. For the last 4 it appears that su(1) will not prompt for
a password if su-ing to oneself. Since x11vnc requires a password
prompt from su, x11vnc forces those logins to fail even when the
correct password is supplied. On *BSD it appears this can be corrected
by removing the pam_self.so entry in /etc/pam.d/su.


Previous older discussion (prior to the -unixpw option):

Until the VNC protocol and libvncserver support this things will be
approximate at best.

One approximate method involves starting x11vnc with the -localhost
option. This basically requires the viewer user to log into the
workstation where x11vnc is running via their Unix username and
password, and then somehow set up a port redirection of his vncviewer
connection to make it appear to emanate from the local machine. As
discussed above, ssh is useful for this: "ssh -L 5900:localhost:5900
user@hostname ..." See the ssh wrapper scripts mentioned elsewhere on
this page. stunnel does this as well.

Of course a malicious user could allow other users to get in through
his channel, but that is a problem with every method. Another thing to
watch out for is a malicious user on the viewer side (where ssh is
running) trying to sneak in through the ssh port redirection there.

Regarding limiting the set of Unix usernames who can connect, the
traditional way would be to further require a VNC password to supplied
(-rfbauth, -passwd, etc) and only tell the people allowed in what the
VNC password is. A scheme that avoids a second password involves using
the -accept option that runs a program to examine the connection
information to determine which user is connecting from the local
machine. That may be difficult to do, but, for example, the program
could use the ident service on the local machine (normally ident
should not be trusted over the network, but on the local machine it
should be accurate: otherwise root has been compromised and so there
are more serious problems! Unfortunately recent Linux distros seem to
provide a random string (MD5 hash?) instead of the username.) An
example script passed in via -accept scriptname that deduces the Unix
username and limits who can be accepted might look something like
this:

```
#!/bin/sh
if [ "$RFB_CLIENT_IP" != "127.0.0.1" -o "$RFB_SERVER_IP" != "127.0.0.1" ]; then
        exit 1  # something fishy... reject it.
fi
user=`echo "$RFB_CLIENT_PORT, $RFB_SERVER_PORT" | nc -w 1 $RFB_CLIENT_IP 113 \
        | grep 'USERID.*UNIX' | head -n 1 | sed -e 's/[\r ]//g' | awk -F: '{pri
nt $4}'`

for okuser in fred barney wilma betty
do
        if [ "X$user" = "X$okuser" ]; then
                exit 0  # accept it
        fi
done
exit 1  # reject it

   For this to work with ssh port redirection, the ssh option
   UsePrivilegeSeparation must be enabled otherwise the userid will
   always be "root".

   Here is a similar example based on Linux netstat(1) output:
#!/bin/sh
#
# accept_local_netstat:  x11vnc -accept command to accept a local
# vncviewer connection from acceptable users.  Linux netstat -nte is used.

PATH=/bin:/usr/bin:$PATH; export PATH;  # set to get system utils

allowed="`id -u fred`";                 # add more user numbers if desired.

# check required settings
ok=1
if [ "X$allowed" = "X" ]; then
        ok=0;   # something wrong with allowed list
fi
if [ "X$RFB_CLIENT_IP" != "X127.0.0.1" -o "X$RFB_SERVER_IP" != "X127.0.0.1" ];
then
        ok=0;   # connection not over localhost
fi
if [ "$RFB_CLIENT_PORT" -le 0 -o "$RFB_SERVER_PORT" -le 0 ]; then
        ok=0;   # something wrong with tcp port numbers
fi
if [ "$ok" = 0 ]; then
        echo "$0: invalid setting:" 1>&2
        env | grep ^RFB | sort 1>&2
        exit 1
fi

# Linux netstat -nte:
# Proto Recv-Q Send-Q Local Address           Foreign Address         State
   User       Inode
# 0     0      0      RFB_CLIENT              RFB_SERVER           ESTABLISHED
   nnnn       ....
#
user=`netstat -nte | grep ESTABLISHED \
        | grep " $RFB_CLIENT_IP:$RFB_CLIENT_PORT  *$RFB_SERVER_IP:$RFB_SERVER_P
ORT "`

echo "netstat match: $user" 1>&2
user=`echo "$user" | head -n 1 | sed -e 's/^.*ESTABLISHED/ /' | awk '{print $1}
'`

ok=0
for u in $allowed
do
        if [ "X$user" = "X$u" ]; then
                ok=1
                break
        fi
done

if [ "X$ok" = "X1" ]; then
        echo "$0: user accepted: '$user'" 1>&2
        exit 0
else
        echo "$0: user '$user' invalid:" 1>&2
        echo "$0: allowed: $allowed" 1>&2
        env | grep ^RFB | sort 1>&2
        exit 1
fi
```

### Q-39: Can I supply an external program to provide my own custom login method (e.g. Dynamic/One-time passwords or non-Unix (LDAP) usernames and passwords)?

Yes, there are several possibilities. For background see the FAQ on
the -accept where an external program may be run to decide if a VNC
client should be allowed to try to connect and log in. If the program
(or local user prompted by a popup) answers "yes", then -accept
proceeds to the normal VNC and x11vnc authentication methods,
otherwise the connection is dropped.

To provide more direct coupling to the VNC client's username and/or
supplied password the following options were added in Sep/2006:

* -unixpw_cmd command
* -passwdfile cmd:command
* -passwdfile custom:command

In each case "command" is an external command run by x11vnc. You
supply it. For example, it may couple to your LDAP system or other
servers you set up.

For -unixpw_cmd the normal -unixpw Login: and Password: prompts are
supplied to the VNC viewer and the strings the client returns are then
piped into "command" as the first two lines of its standard input. If
the command returns success, i.e. exit(0), the VNC client is accepted,
otherwise it is rejected.

For "-passwdfile cmd:command" the command is run and it returns a
password list (like a password file, see the -passwdfile read:filename
mode.) Perhaps a dynamic, one-time password is retrieved from a server
this way.

For "-passwdfile custom:command" one gets complete control over the
VNC challenge-response dialog with the VNC client. x11vnc sends out a
string of random bytes (16 by the VNC spec) and the client returns the
same number of bytes in a way the server can verify only the
authorized user could have created. The VNC protocol specifies DES
encryption with a password. If you are willing to modify the VNC
viewers, you can have it be anything you want, perhaps a less
crackable MD5 hash scheme or one-time pad. Your program will read from
its standard input the size of the challenge-response followed by a
newline, then the challenge bytes followed by the response bytes. If
your command then returns success, i.e. exit(0), the VNC client is
accepted, otherwise it is rejected.

In all cases the "RFB_*" environment variables are set as under
-accept. These variables can provide useful information for the
externally supplied program to use.


### Q-40: Why does x11vnc exit as soon as the VNC viewer disconnects? And why doesn't it allow more than one VNC viewer to connect at the same time?

These defaults are simple safety measures to avoid someone unknowingly
leaving his X11 desktop exposed (to the internet, say) for long
periods of time. Use the -forever option (aka -many) to have x11vnc
wait for more connections after the first client disconnects. Use the
-shared option to have x11vnc allow multiple clients to connect
simultaneously.

Recommended additional safety measures include using ssh (see above),
stunnel, -ssl, or a VPN to authenticate and encrypt the viewer
connections or to at least use the -rfbauth passwd-file option to use
VNC password protection (or -passwdfile) It is up to YOU to apply
these security measures, they will not be done for you automatically.


### Q-41: Can I limit which machines incoming VNC clients can connect from?

Yes, look at the -allow and -localhost options to limit connections by
hostname or IP address. E.g.

```
x11vnc -allow 192.168.0.1,192.168.0.2
```

for those two hosts or

```
x11vnc -allow 192.168.0.
```

for a subnet. For individual hosts you can use the hostname instead of
the IP number, e.g.: "-allow snoopy", and "-allow darkstar,wombat".
Note that -localhost achieves the same thing as "-allow 127.0.0.1"

For more control, build libvncserver with libwrap support
(tcp_wrappers) and then use /etc/hosts.allow See hosts_access(5) for
complete details.


### Q-42: How do I build x11vnc/libvncserver with libwrap (tcp_wrappers) support?

Here is one way to pass this information to the configure script:

```
env CPPFLAGS=-DUSE_LIBWRAP LDFLAGS=-lwrap ./configure
```

then run make as usual. This requires libwrap and its development
package (tcpd.h) to be installed on the build machine. If additional
CPPFLAGS or LDFLAGS options are needed supply them as well using
quotes.

The resulting x11vnc then uses libwrap/tcp_wrappers for connections.
The service name you will use in /etc/hosts.allow and /etc/hosts.deny
is "vnc", e.g.:

```
vnc: 192.168.100.3 .example.com
```

Note that if you run x11vnc out of inetd you do not need to build
x11vnc with libwrap support because the /usr/sbin/tcpd reference in
/etc/inetd.conf handles the tcp_wrappers stuff.


### Q-43: Can I have x11vnc only listen on one network interface (e.g. internal LAN) rather than having it listen on all network interfaces and relying on -allow to filter unwanted connections out?

As of Mar/2005 there is the "-listen ipaddr" option that enables this.
For ipaddr either supply the desired network interface's IP address
(or use a hostname that resolves to it) or use the string "localhost".
For additional filtering simultaneously use the "-allow host1,..."
option to allow only specific hosts in.

This option is useful if you want to insure that no one can even begin
a dialog with x11vnc from untrusted network interfaces (e.g. ppp0.)
The option -localhost now implies "-listen localhost" since that is
what most people expect it to do.


### Q-44: Now that -localhost implies listening only on the loopback interface, how I can occasionally allow in a non-localhost via the -R allowonce remote control command?

To do this specify "-allow localhost". Unlike -localhost this will
leave x11vnc listening on all interfaces (but of course only allowing
in local connections, e.g. ssh redirs.) Then you can later run "x11vnc
-R allowonce:somehost" or use to gui to permit a one-shot connection
from a remote host.


### Q-45: Can I fine tune what types of user input are allowed? E.g. have some users just be able to move the mouse, but not click or type anything?

As of Feb/2005, the -input option allows you to do this. "K", "M",
"B", "C", and "F" stand for Keystroke, Mouse-motion, Button-clicks,
Clipboard, and File-Transfer, respectively. The setting: "-input M"
makes attached viewers only able to move the mouse. "-input KMBC,M"
lets normal clients do everything and enables view-only clients to
move the mouse.

These settings can also be applied on a per-viewer basis via the
remote control mechanism or the GUI. E.g. x11vnc -R input:hostname:M


### Q-46: Can I prompt the user at the local X display whether the incoming VNC client should be accepted or not? Can I decide to make some clients view-only? How about running an arbitrary program to make the decisions?

Yes, look at the "-accept command" option, it allows you to specify an
external command that is run for each new client. (use quotes around
the command if it contains spaces, etc.) If the external command
returns 0 (success) the client is accepted, otherwise with any other
return code the client is rejected. See below how to also accept
clients view-only.

The external command will have the RFB_CLIENT_IP environment variable
set to the client's numerical IP address, RFB_CLIENT_PORT its port
number. Similarly for RFB_SERVER_IP and RFB_SERVER_PORT to allow
identification of the tcp virtual circuit. DISPLAY will be set to that
of the X11 display being polled. Also, RFB_X11VNC_PID is set to the
x11vnc process id (e.g. in case you decided to kill it), RFB_CLIENT_ID
will be an id number, and RFB_CLIENT_COUNT the number of other clients
currently connected. RFB_MODE will be "accept".

Built-in Popup Window: As a special case, "-accept popup" will
instruct x11vnc to create its own simple popup window. To accept the
client press "y" or click mouse on the "Yes" button. To reject the
client press "n" or click mouse on the "No" button. To accept the
client View-only, press "v" or click mouse on the "View" button. If
the -viewonly option has been supplied, the "View" action will not be
present: the whole display is view only in that case.

The popup window times out after 120 seconds, to change this behavior
use "-accept popup:N" where N is the number of seconds (use 0 for no
timeout.) More tricks: "-accept popupmouse" will only take mouse click
responses, while "-accept popupkey" will only take keystroke responses
(popup takes both.) After any of the 3 popup keywords you can supply a
position of the window: +N+M, (the default is to center the window)
e.g. -accept popupmouse+10+10.

Also as a special case "-accept xmessage" will run the xmessage(1)
program to prompt the user whether the client should be accepted or
not. This requires that you have xmessage installed and available via
PATH. In case it is not already on your system, the xmessage program
is available at ftp://ftp.x.org/
(End of Built-in Popup Window:)

To include view-only decisions for the external commands, prefix the
command something like this: "yes:0,no:*,view:3 mycommand ..." This
associates the three actions: yes(accept), no(reject), and
view(accept-view-only), with the numerical return (i.e. exit()) codes.
Use "*" instead of a number to set the default action (e.g. in case
the external command returns an unexpected return code.)

Here is an example -accept script called accept_or_lock. It uses
xmessage and xlock (replace with your screen lock command, maybe it is
"xscreensaver-command -lock", or kdesktop_lock, or "dtaction
LockDisplay".) It will prompt the user at the X display whether to
accept, reject, or accept view-only the client, but if the prompt
times out after 60 seconds the screen is locked and the VNC client is
accepted. This allows the remote access when no one is at the display.

```
#!/bin/sh
#
# accept_or_lock: prompt user at X display whether to accept an incoming
#                 VNC connection.  If timeout expires, screen is locked
#                 and the VNC viewer is accepted (allows remote access
#                 when no one is sitting at the display.)
#
# usage: x11vnc ... -forever -accept 'yes:0,no:*,view:4 accept_or_lock'
#
xmessage -buttons yes:2,no:3,view-only:4 -center \
         -timeout 60 "x11vnc: accept connection from $RFB_CLIENT_IP?"
rc=$?
if [ $rc = 0 ]; then
        xlock & # or "xlock -mode blank" for no animations.
        sleep 5
        exit 0
elif [ $rc = 2 ]; then
        exit 0
elif [ $rc = 4 ]; then
        exit 4
fi
exit 1
```

Stefan Radman has written a nice dtksh script dtVncPopup for use in
CDE environments to do the same sort of thing. Information on how to
use it is found at the top of the file. He encourages you to provide
feedback to him to help improve the script.

Note that in all cases x11vnc will block while the external command or
popup is being run, so attached clients will not receive screen
updates, etc during this period.

To run a command when a client disconnects, use the "-gone command"
option. This is for the user's convenience only: the return code of
the command is not interpreted by x11vnc. The same environment
variables are set as in "-accept command" (except that RFB_MODE will
be "gone".)

As of Jan/2006 the "-afteraccept command" option will run the command
only after the VNC client has been accepted and authenticated. Like
-gone the return code is not interpreted. RFB_MODE will be
"afteraccept".)


### Q-47: I start x11vnc as root because it is launched via inetd(8) or a display manager like gdm(1). Can I have x11vnc later switch to a different user?

As of Feb/2005 x11vnc has the -users option that allows things like
this. Please read the documentation on it (also in the x11vnc -help
output) carefully for features and caveats. It's use can often
decrease security unless care is taken.

BTW, a nice use of it is "-users +nobody" that switches to the Unix
user nobody right after connections to the X display are established.

In any event, while running x11vnc as root, remember it comes with no
warranty ;-).


### Q-48: I use a screen-lock when I leave my workstation (e.g. xscreensaver or xlock.) When I remotely access my workstation desktop via x11vnc I can unlock the desktop fine, but I am worried people will see my activities on the physical monitor. What can I do to prevent this, or at least make it more difficult?

Probably most work environments would respect your privacy if you
powered off the monitor. Also remember if people have physical access
to your workstation they basically can do anything they want with it
(e.g. install a backdoor for later use, etc.)

In any event, as of Jun/2004 there is an experimental utility to make
it more difficult for nosey people to see your x11vnc activities. The
source for it is blockdpy.c The idea behind it is simple (but
obviously not bulletproof): when a VNC client attaches to x11vnc put
the display monitor in the DPMS "off" state, if the DPMS state ever
changes immediately start up the screen-lock program. The x11vnc user
will notice something is happening and think about what to do next
(while the screen is in a locked state.)

This works (or at least has a chance of working) because if the
intruder moves the mouse or presses a key on the keyboard, the monitor
wakes up out of the DPMS off state, and this induces the screen lock
program to activate as soon as possible. Of course there are cracks in
this, the eavesdropper could detach your monitor and insert a non-DPMS
one, and there are race conditions. As mentioned above this is not
bulletproof. A really robust solution would likely require X server
and perhaps even video hardware support.

The blockdpy utility is launched by the -accept option and told to
exit via the -gone option (the vnc client user should obviously
re-lock the screen before disconnecting!) Instructions can be found in
the source code for the utility at the above link. Roughly it is
something like this:

```
x11vnc ... -accept "blockdpy -bg -f $HOME/.bdpy" -gone "touch $HOME/.bdpy"
```

but please read the top of the file.

Update: As of Feb/2007 there is some builtin support for this:
-forcedpms and -clientdpms however, they are probably less robust than
the above blockdpy.c scheme, since if the person floods the physical
machine with mouse or pointer input he can usually see flashes of the
screen before the monitor is powered off again. See also the -grabkbd,
-grabptr, and -grabalways options.


### Q-49: Can I have x11vnc automatically lock the screen when I disconnect the VNC viewer?

Yes, a user mentions he uses the -gone option under CDE to run a
screen lock program:

```
x11vnc -display :0 -forever -gone 'dtaction LockDisplay'
```

Other possibilities are:

```
x11vnc -display :0 -forever -gone 'xscreensaver-command -lock'
x11vnc -display :0 -forever -gone 'kdesktop_lock'
x11vnc -display :0 -forever -gone 'xlock &'
x11vnc -display :0 -forever -gone 'xlock -mode blank &'
```

Here is a scheme using the -afteraccept option (in version 0.8) to
unlock the screen after the first valid VNC login and to lock the
screen after the last valid VNC login disconnects:

```
x11vnc -display :0 -forever -shared -afteraccept ./myxlocker -gone ./myxlocker
```

Where the script ./myxlocker is:

```
#!/bin/sh

#/usr/bin/env | grep RFB_ | sort        # for viewing RFB_* settings.

if [ "X$RFB_MODE" = "Xafteraccept" ]; then
        if [ "X$RFB_STATE" = "XNORMAL" ]; then  # require valid login
                if [ "X$RFB_CLIENT_COUNT" = "X1" ]; then
                        killall xlock   # Linux only.
                fi
        fi
elif [ "X$RFB_MODE" = "Xgone" ]; then
        if [ "X$RFB_STATE" = "XNORMAL" ]; then  # require valid login
                if [ "X$RFB_CLIENT_COUNT" = "X0" ]; then
                        xlock -mode blank &
                fi
        fi
fi
```

Note the xlock option "-mode blank" to avoid animations.

There is a problem if you have x11vnc running this way in -forever
mode and you hit Ctrl-C to stop it. The xlock (or other program) will
get killed too. To work around this make a little script called
setpgrp that looks like:

```
#!/usr/bin/perl
setpgrp(0, 0);
exec @ARGV;
```

then use -gone "setpgrp xlock &", etc.


## Encrypted Connections

### Q-50: How can I tunnel my connection to x11vnc via an encrypted SSH channel between two Unix machines?

See the description earlier on this page on how to tunnel VNC via SSH
from Unix to Unix. A number of ways are described along with some
issues you may encounter.

Other secure encrypted methods exists, e.g. stunnel, IPSEC, various
VPNs, etc.

See also the Enhanced TightVNC Viewer (SSVNC) page where much of this
is now automated.


### Q-51: How can I tunnel my connection to x11vnc via an encrypted SSH channel from Windows using an SSH client like Putty?

Above we described how to tunnel VNC via SSH from Unix to Unix, you
may want to review it. To do this from Windows using Putty it would go
something like this:

* In the Putty dialog window under 'Session' enter the hostname or
  IP number of the Unix machine with display to be viewed.
* Make sure the SSH protocol is selected and the server port is
  correct.
* Under 'Connections/SSH/Tunnels' Add a Local connection with
  'Source port:  5900' and 'Destination:  localhost:5900'
* Log into the remote machine by pressing 'Open' and supplying
  username, password, etc.
* In that SSH shell, start up x11vnc by typing the command: x11vnc
  -display :0 plus any other desired options (e.g. -localhost.)
* Finally, start up your VNC Viewer in Windows and enter
  'localhost:0' as the VNC server.

You can keep all of the settings in a Putty 'Saved Session'. Also,
once everything is working, you can consider putting x11vnc -display
:0 (plus other cmdline options) in the 'Remote command' Putty setting
under 'Connections/SSH'.

See also the Enhanced TightVNC Viewer (SSVNC) page where much of this
is now automated via the Putty plink utility.

For extra protection feel free to run x11vnc with the -localhost and
-rfbauth/-passwdfile options.

If the machine you SSH into via Putty is not the same machine with the
X display you wish to view (e.g. your company provides incoming SSH
access to a gateway machine), then you need to change the above Putty
dialog setting to: 'Destination: otherhost:5900', Once logged in,
you'll need to do a second login (ssh or rsh) to the workstation
machine 'otherhost' and then start up x11vnc on it. This can also be
automated by Chaining SSH's.

As discussed above another option is to first start the VNC viewer in
"listen" mode, and then launch x11vnc with the "-connect localhost"
option to establish the reverse connection. In this case a Remote port
redirection (not Local) is needed for port 5500 instead of 5900 (i.e.
'Source port:  5500' and 'Destination:  localhost:5500' for a Remote
connection.)


### Q-52: How can I tunnel my connection to x11vnc via an encrypted SSL channel using an external tool like stunnel?

It is possible to use a "lighter weight" encryption setup than SSH or
IPSEC. SSL tunnels such as stunnel (also stunnel.org) provide an
encrypted channel without the need for Unix users, passwords, and key
passphrases required for ssh (and at the other extreme SSL can also
provide a complete signed certificate chain of trust.) On the other
hand, since SSH is usually installed everywhere and firewalls often
let its port through, ssh is frequently the path of least resistance
(it also nicely manages public keys for you.)

Update: As of Feb/2006 x11vnc has the options -ssl, -stunnel, and
-sslverify to provide integrated SSL schemes. They are discussed in
the Next FAQ (you probably want to skip to it now.)

We include these non-built-in method descriptions below for historical
reference. They are handy because can be used to create SSL tunnels to
any VNC (or other type of) server.


Here are some basic examples using stunnel but the general idea for
any SSL tunnel utility is the same:

* Start up x11vnc and constrain it to listen on localhost.
* Then start up the SSL tunnel running on the same machine to
  forward incoming connections to that x11vnc.
* Set up and run a similar SSL tunnel for the outgoing connection on
  the VNC viewer machine pointing it to the SSL/x11vnc server.
* Optionally, set up server (or even client) public/private keys for
  use in authenticating one side to the other.
* Finally, start the VNC Viewer and tell it to connect to the local
  port (e.g. a vnc display localhost:0) where its outgoing SSL
  tunnel is listening.

We'll first use the stunnel version 3 syntax since it is the most
concise and Unixy.

Start up x11vnc listening on port 5900:

```
x11vnc -display :0 -rfbport 5900 -localhost -bg -passwdfile ~/mypass
```

Then start stunnel (version 3, not 4) with this command:

```
stunnel -d 5901 -r 5900 -p /path/to/stunnel.pem
```

The above two commands are run on host "far-away.east". The
stunnel.pem is the self-signed PEM file certificate created when
stunnel is built. One can also create certificates signed by
Certificate Authorities or self-signed if desired using the x11vnc
utilities described there.

SSL Viewers:  Next, on the VNC viewer side we need an SSL tunnel to
encrypt the outgoing connection. The nice thing is any SSL tunnel can
be used because the protocol is a standard. For this example we'll
also use stunnel on the viewer side on Unix. First start up the
client-side stunnel (version 3, not 4):

```
stunnel -c -d localhost:5902 -r far-away.east:5901
```

Then point the viewer to the local tunnel on port 5902:

```
vncviewer -encodings "copyrect tight zrle hextile" localhost:2
```

That's it.  Note that the ss_vncviewer script can automate this
easily, and so can the Enhanced TightVNC Viewer (SSVNC) package.

Be sure to use a VNC password because unlike ssh by default the
encrypted SSL channel provides no authentication (only privacy.) With
some extra configuration one could also set up certificates to provide
authentication of either or both sides as well (and hence avoid
man-in-the-middle attacks.) See the stunnel and openssl documentation
and also the key management section for details.

stunnel has also been ported to Windows, and there are likely others
to choose from for that OS. Much info for using it on Windows can be
found at the stunnel site and in this article The article also shows
the detailed steps to set up all the authentication certificates. (for
both server and clients, see also the x11vnc utilities that do this.)
The default Windows client setup (no certs) is simpler and only 4
files are needed in a folder: stunnel.exe, stunnel.conf, libssl32.dll,
libeay32.dll. We used an stunnel.conf containing:

```
# stunnel.conf:
client = yes
options = ALL
[myvncssl]
accept = localhost:5902
connect = far-away.east:5901
```

then double click on the stunnel.exe icon to launch it (followed by
pointing the VNC viewer to localhost:2).

stunnel inetd-like mode:

As an aside, if you don't like the little "gap" of unencrypted TCP
traffic (and a localhost listening socket) on the local machine
between stunnel and x11vnc it can actually be closed by having stunnel
start up x11vnc in -inetd mode:

```
stunnel -p /path/to/stunnel.pem -P none -d 5900 -l ./x11vnc_sh
```

Where the script x11vnc_sh starts up x11vnc:

```
#!/bin/sh
x11vnc -q -inetd -display :0 -passwdfile ~/mypass
```

Note that this creates a separate x11vnc process for each incoming
connection (as any inetd x11vnc usage would), but for the case of
normally just one viewer at a time it should not be a big problem.


stunnel 4 syntax:

Somewhat sadly, the stunnel version 4 syntax is not so amenable to the
command line or scripts. You need to create a config file with the
parameters. E.g.:

```
stunnel x11vnc.cfg
```

Where the file x11vnc.cfg contains:

```
foreground = yes
pid =
cert = /path/to/stunnel.pem
[x11vnc_stunnel]
accept  = 5901
connect = 5900
```

One nice thing about version 4 is often the PEM file does not need to
be specified because stunnel finds it in its installed area. One other
gotcha the PEM file is usually only readable by root (it has the
private key afterall), so you'll need to relax the permissions or make
a copy that the user running x11vnc/stunnel can read.


SSL VNC Viewers:

Regarding VNC viewers that "natively" do SSL unfortunately there do
not seem to be many. The SingleClick UltraVNC Java Viewer is SSL and
is compatible with x11vnc's -ssl option and stunnel.) Commercial
versions of VNC seem to have some SSL-like encryption built in, but we
haven't tried those either and they probably wouldn't work since their
(proprietary) SSL-like negotiation is likely embedded in the VNC
protocol unlike our case where it is external.

Note: as of Mar/2006 libvncserver/x11vnc provides a SSL-enabled Java
applet that can be served up via the -httpdir or -http options when
-ssl is enabled. It will also be served via HTTPS via either the VNC
port (e.g. https://host:5900/) or a 2nd port via the -https option.

In general current SSL VNC solutions are not particularly "seemless".
But it can be done, and with a wrapper script on the viewer side and
the -stunnel or -ssl option on the server side it works well and is
convenient. Here is a simple script ss_vncviewer that automates
running stunnel on the VNC viewer side on Unix a little more carefully
than the commands printed above. (One could probably do a similar
thing with a .BAT file on Windows in the stunnel folder.)

Update Jul/2006: we now provide an Enhanced TightVNC Viewer (SSVNC)
package that starts up STUNNEL automatically along with some other
features. All binaries (stunnel, vncviewer, and some utilities) are
provided in the package. It works on Unix, Mac OS X, and Windows.


### Q-53: Does x11vnc have built-in SSL tunneling?

You can read about non-built-in methods in the Previous FAQ for
background.

SSL tunnels provide an encrypted channel without the need for Unix
users, passwords, and key passphrases required for ssh (and at the
other extreme SSL can also provide a complete signed certificate chain
of trust.) On the other hand, since SSH is usually installed
everywhere and firewalls often let its port through, ssh is frequently
the path of least resistance.

Built-in SSL x11vnc options:

As of Feb/2006 the x11vnc -ssl option automates the SSL tunnel
creation on the x11vnc server side. An SSL-enabled Java Viewer applet
is also provided that can be served via HTTP or HTTPS to automate SSL
on the client side.

The -ssl mode uses the www.openssl.org library if available at build
time.

The mode requires an SSL certificate and key (i.e. .pem file.) These
are usually created via the openssl(1) program (in fact in for "-ssl"
(same as "-ssl SAVE") it will run openssl for you automatically.) So
the SSL is not completely "built-in" since this external tool needs to
be installed, but at least x11vnc runs it for you automatically.

An -ssl example:

```
x11vnc -display :0 -ssl -passwdfile ~/mypass
```

You'll get output like this:

```
09/04/2006 19:27:35 Creating a self-signed PEM certificate...
09/04/2006 19:27:35
...

The SSL VNC desktop is:  far-away.east:0
PORT=5900
SSLPORT=5900
```

In this case openssl(1) was used to create a PEM automatically. It
will prompt you if you want to protect it with with a passphrase. Use
"-ssl SAVE_NOPROMPT" to not be prompted. Use "-ssl TMP" to create a
temporary self-signed cert that will be discarded when x11vnc exits.

Update: As of Nov/2008 x11vnc also supports the VeNCrypt SSL/TLS
tunnel extension to the VNC protocol. The older ANONTLS method (vino)
is also supported. This support is on by default when the -ssl option
is in use and can be fine-tuned using these options: -vencrypt,
-anontls, and -sslonly.

The normal x11vnc -ssl operation is somewhat like a URL method
vncs://hostname if vnc://hostname indicates a standard unencrypted VNC
connection. Just as https://hostname is an SSL encrypted version of
http://hostname. The entire VNC session goes through the SSL tunnel.
VeNCrypt, on the other hand, switches to SSL/TLS early in the VNC
protocol handshake. x11vnc 0.9.6 supports both simultaneously when
-ssl is active.

Note: With the advent of OpenSSL 1.1.0, SSLv2 is dropped and SSLv3
deactivated per default. A couple broken ciphers have also gone, most
importantly though is that clients trying to connect to x11vnc will
now have to support TLS if encryption is to be used. You can of
course always cook up your own build and run time OpenSSL 1.1.x if
SSLv3 is absolutely required, but it isn't wise from a security point
of view.


SSL VNC Viewers:. Viewer-side will need to use SSL as well. See the
next FAQ and here for SSL enabled VNC Viewers, including SSVNC, to
connect to the above x11vnc via SSL.


As seen above, the PEM (privacy enhanced mail) file does not need to
be supplied if the openssl(1) command is available in PATH, in that
case a self-signed, certificate good the current and subsequent x11vnc
sessions is created (this may take a while on very slow machines.)

In general, the PEM file contains both the Certificate (i.e. public
key) and the Private Key. Because of the latter, the file should be
protected from being read by untrusted users. The best way to do this
is to encrypt the key with a passphrase (note however this requires
supplying the passphrase each time x11vnc is started up.)

See the discussion on x11vnc Key Management for some utilities
provided for creating and managing certificates and keys and even for
creating your own Certificate Authority (CA) for signing VNC server
and client certificates. This may be done by importing the certificate
into Web Browser or Java plugin keystores, or pointing stunnel to it.
The wrapper script ss_vncviewer provides an example on unix (see the
-verify option.)

Here are some notes on the simpler default (non-CA) operation. To have
x11vnc save the generated certificate and key, use the "SAVE" keyword
like this:

```
x11vnc -ssl SAVE -display :0 ...
```

(this is the same as the default: "-ssl".) This way it will be saved
in the default directory ~/.vnc/certs/ as server.crt (the certificate
only) and server.pem (both certificate and private key.) This opens up
the possibility of copying the server.crt to machines where the VNC
Viewer will be run to enable authenticating the x11vnc SSL VNC server
to the clients. When authentication takes place this way (or via the
more sophisticated CA signing described here), then
Man-In-The-Middle-Attacks are prevented. Otherwise, the SSL encryption
only provides protection against passive network traffic "sniffing"
(i.e. you are not protected against M-I-T-M attacks.) Nowadays, most
people seem mostly concerned mainly about passive sniffing (and the
default x11vnc SSL mode protects against it.) Note that there are
hacker tools like dsniff/webmitm and cain that implement SSL
Man-In-The-Middle attacks. They rely on the client not bothering to
check the cert.


One can test to some degree that SSL is working after starting x11vnc
with the -stunnel or -ssl option. From another machine one can use the
openssl command something like this:

```
openssl s_client -debug -msg -showcerts -connect far-away.east:5900
```

After all of the debugging output and informational messages you'll
see the string "RFB 003.008" that came from x11vnc. Pointing a web
browser connecting to: https://far-away.east:5900/ and then viewing
the SSL certificate information about the connection in the panels
will also work.

Note: If you serve up the SSL enabled Java VNC Viewer via something
like:

```
x11vnc -ssl -httpdir /usr/local/share/x11vnc/classes/ssl
```

(or just the -http option), you can test it out completely using that,
including using https to download it into the browser and connect to
x11vnc.


The older -stunnel option: Before the -ssl option there was a
convenience option -stunnel that would start an external SSL tunnel
for you using stunnel. The -ssl method is the preferred way, but for
historical reference we keep the -stunnel info here.

The -stunnel mode requires the stunnel.mirt.net command stunnel(8) to
be installed on the system.

Some -stunnel examples:

```
x11vnc -display :0 -stunnel /path/to/stunnel.pem -passwdfile ~/mypass

x11vnc -display :0 -stunnel SAVE ...
```

You'll get output like this:

```
The VNC desktop is:      localhost:50
The SSL VNC desktop is:  far-away.east:0
PORT=5950
SSLPORT=5900
```

That indicates stunnel is listening on port 5900 for incoming
SSL-wrapped VNC connections from viewers. x11vnc is listening for
local connections on port 5950 in this case (remote viewers cannot
connect to it directly.) For -stunnel to work the stunnel command must
be installed on the machine and available in PATH (note stunnel is
often installed in sbin directories rather than bin.) Note that the
default "-stunnel" by itself creates a temporary cert (as in "-ssl
TMP".)


### Q-54: How do I use VNC Viewers with built-in SSL tunneling?

Notes on using "native" VNC Viewers with SSL:

There aren't any native VNC Viewers that do SSL (ask your VNC viewer
developer to add the feature.) So a tunnel must be setup that you
point the VNC Viewer to. This is often STUNNEL. You can do this
manually, or use the ss_vncviewer script on Unix, or our Enhanced
TightVNC Viewer (SSVNC) package on Unix, Windows, or MacOSX. See the
next section for Java Web browser SSL VNC Viewers (you only need a
Java-enabled Web browser for it to work.)

Notes on the SSL enabled Java VNC Viewer provided in x11vnc
classes/ssl/VncViewer.jar:

A Java applet VNC Viewer allows you to connect to a VNC Server from a
Java-enabled Web browser.

The SSL enabled Java VNC Viewer (VncViewer.jar) in the x11vnc package
supports only SSL based connections by default. As mentioned above the
-httpdir can be used to specify the path to .../classes/ssl. A typical
location might be /usr/local/share/x11vnc/classes/ssl. Or -http can be
used to try to have it find the directory automatically.

Also note that the SingleClick UltraVNC Java Viewer is compatible with
x11vnc's -ssl SSL mode. (We tested it this way: "java -cp
./VncViewer.jar VncViewer HOST far-away.east PORT 5900 USESSL 1
TRUSTALL 1")

The Java viewer uses SSL to communicate securely with x11vnc. Note
that the applet can optionally also be downloaded into your web
browser via HTTPS (which is HTTP over SSL.) This way the HTML page and
the Java applet itself are also delivered securely with SSL (as
opposed to only the VNC traffic being encrypted with SSL.)

For this case the output will be something like this:

```
x11vnc -ssl SAVE -http
...
The SSL VNC desktop is:  far-away.east:0
Java SSL viewer URL:     https://far-away.east:5900/
Java SSL viewer URL:     http://far-away.east:5800/
PORT=5900
SSLPORT=5900
```

Indicating the two URLs (the first one encrypted, the second not) one
could point the web browser at to get the VNC viewer applet. E.g. put
this

```
http://far-away.east:5800/
```

or:

```
https://far-away.east:5900/
```

into your Java-enabled Web browser.

Note that KDE's Konqueror web browser seems to have problems with
https Java applets, so you'll have to use the http/5800 with it (if
you get https/5900 working let us know how you did it.)

If you are using a router/firewall with port-redirection, and you are
redirecting ports other than the default ones (5800, 5900) listed
above see here.

The https service provided thru the actual VNC port (5900 in the above
example) can occasionally be slow or unreliable (it has to read some
input and try to guess if the connection is VNC or HTTP.) If it is
unreliable for you and you still want to serve the Java applet via
https, use the -https option to get an additional port dedicated to
https (its URL will also be printed in the output.)

Another possibility is to add the GET applet parameter:

```
https://far-away.east:5900/?GET=1
```

This will have the VNC Viewer send a special HTTP GET string "GET
/request.https.vnc.connection HTTP/1.0" that x11vnc will notice more
quickly as a request for a VNC connection. Otherwise it must wait for
a timeout to expire before it assumes a VNC connection.

You may also use "urlPrefix=somestring" to have /somestring prepended
to /request.https.vnc.connection". Perhaps you are using a web server
proxy scheme to enter a firewall or otherwise have rules applied to
the URL. If you need to have any slashes "/" in "somestring" use
"_2F_" (a deficiency in libvncserver prevents using the more natural
"%2F".)

You apply multiple applet parameters in the regular URL way, e.g.:

```
https://far-away.east:5900/?GET=1&urlPrefix=mysubdir&...
```

All of the x11vnc Java Viewer applet parameters are described in the
file classes/ssl/README


Tips on Getting the SSL Java Applet Working the First Time:
Unfortunately, it can be a little tricky getting the SSL VNC Java
Viewer working with x11vnc. Here are some tips to getting working the
first time (afterwards you can incrementally customize with more
complex settings.)

* First try it on the LAN: Do NOT try to have it work the first time
  going through firewalls, Web proxies, home router port
  redirections, or Apache portal. Just try a direct connection over
  your LAN first (if you only have 1 machine and no LAN, just do a
  direct connection to the same machine: localhost.) If the LAN
  machine you run x11vnc on has its own host-level firewall (most
  linux machine come with that on by default), disable it or at
  least let tcp ports 5800-6000 through.
* First try HTTP to download the Java Applet: x11vnc can serve both
  the Java Applet jar file and VNC out of the same port (both
  tunneled through SSL, see below.) But it can lead to timing and
  other problems. So first try HTTP instead of HTTPS to download the
  Applet jar file (VncViewer.jar.) That is to say try
  http://hostname:5800 in your web browser first before trying
  https://hostname:5900. x11vnc will print out the ports and URLs it
  is using, so use the HTTP one it prints out.
* Always Restart the Browser: If you are having failures and have to
  repeatedly retry things ALWAYS restart the browser (i.e.
  completely exit it and then start a new browser process) each
  time. Otherwise as you are changing things the browser may
  "remember" failed applet downloads, etc. and just add to the
  confusion and irreproducibility. If you see it trying to download
  VncViewer.class (instead of VncViewer.jar) you know it is really
  confused and needs to be restarted.
* Step Lively: If you get Browser or Java VM or VNC Viewer applet
  dialog boxes saying things like "Do you want to trust this
  certificate?" or "The hostname does not match the one on the
  certificate", etc. just go through them as quickly as possible.
  x11vnc cannot wait forever for each SSL connection, and so if you
  dawdle too long inspecting the certs, etc it can lead to problems.
  Get it working first before taking your time to read the details
  in the dialogs, etc.
* No inetd, Please: Even if you intend to deploy via inetd or xinetd
  eventually, get that working later (and remember do not use
  something like "-ssl TMP" that creates a new temporary SSL
  certificate for every new socket connection.)
* Nothing Fancy: Do not try fancy stuff like -svc, -create, -unixpw,
  "-users unixpw=", "-users sslpeer=", -sslverify, etc. Just get the
  simplest connection working first and then incrementally add what
  you need.

So the recommended test command lines are:

```
x11vnc -ssl SAVE -http
x11vnc -ssl SAVE -httpdir /path/to/x11vnc/classes/ssl
```

Use the latter if x11vnc cannot automatically find the classes/ssl
directory (this what the -http option instructs it to do.) Then point
your browser to the HTTP (not HTTPS) URL it prints out.

Following the above guidelines, did it work? If so, Congratulations!!
you created an SSL encrypted connection between the SSL Java applet
running in your web browser and x11vnc. The fact that you used HTTP
instead of HTTPS to download the applet is not the end of the world
(some users do it this way), the main thing is that the VNC traffic is
encrypted with SSL. If you are having trouble even with the above
baseline test case feel free to contact me (please send the Full
x11vnc output, not just part of it; the complete x11vnc command line;
the URL(s) entered in the browser; the full Java Console output; and
anything else you can think of.)

Next, you can add the features you want one by one testing it still
works each time. I suggest first turning on the HTTPS applet download
(https://hostname:5900) if that is what you intend to use. That one
gives the most trouble because of the ambiguity of passing two
different protocols (HTTP and VNC) through the same SSL service port.

Next, turn on inetd if you intend to use that (this can be tricky too,
be sure to use -oa logfile and inspect it carefully if there are
problems.) If you are going to use non-standard ports (e.g. "-rfbport
443" as root), work on that next. Then enable the firewall, router
port redirection channel (you will somehow need to be outside to do
that, maybe test that through another VNC session.)

Then, if you plan to use them, enable "fancy stuff" like "-svc" or
"-unixpw", etc, etc. Be sure to add a password either "-rfbauth" or
"-unixpw" or both. If you need to have the web browser use a corporate
Web Proxy (i.e. it cannot connect directly) work on that last. Ditto
for the Apache portal.


Router/Firewall port redirs:  If you are doing port redirection at
your router to an internal machine running x11vnc AND the internet
facing port is different from the internal machine's VNC port, you
will need to apply the PORT applet parameter to indicate to the applet
the Internet facing port number (otherwise by default the internal
machine's port, say 5900, is sent and that of course is rejected at
the firewall/router.) For example:

```
https://far-away.east:443/?GET=1&PORT=443
```

So in this example the user configures his router to redirect
connections to port 443 on his Internet side to, say, port 5900 on the
internal machine running x11vnc. See also the -httpsredir option that
will try to automate this for you.

To configure your router to do port redirection, see its instructions.
Typically, from the inside you point a web browser to a special URL
(e.g. http://192.168.1.1) and you get a web interface to configure it.
Look for something like "Port Redirection" or "Port Forwarding",
probably under "Advanced" or something like that. If you have a Linux
or Unix system acting as your firewall/router, see its firewall
configuration.

You can also use x11vnc options -rfbport NNNNN and -httpport NNNNN to
match the ports that your firewall will be redirecting to the machine
where x11vnc is run.

Tedious Dialogs: If you do serve the SSL enabled Java viewer via https
be prepared for quite a number of "are you sure you trust this site?"
dialogs:

* First from the Web browser that cannot verify the self-signed
  certificate when it downloads index.vnc.
* From the Web browser again noting that the common name on the
  certificate does not match the hostname of the remote machine.
* Next from the Java VM that cannot verify the self-signed
  certificate when it downloads VncViewer.jar.
* And also from the Java VM again noting that the common name on the
  certificate does not match the hostname of the remote machine.
* Finally from the Java VncViewer applet itself saying it cannot
  verify the certificate! (or a popup asking you if you want to see
  the certificate.)

Note that sometimes if you pause too long at one of the above dialogs
then x11vnc may exceed a timeout and assume the current socket
connection is VNC instead of the HTTPS it actually is (but since you
have paused too long at the dialog the GET request comes too late.)
Often hitting Reload and going through the dialogs more quickly will
let you connect. The Java VM dialogs are the most important ones to
NOT linger at. If you see in the x11vnc output a request for
VncViewer.class instead of VncViewer.jar it is too late... you will
need to completely restart the Web browser to get it to try for the
jar again. You can use the -https option if you want a dedicated port
for HTTPS connections instead of sharing the VNC port.

To see example x11vnc output for a successful https://host:5900/
connection with the Java Applet see This Page. And here is a newer
example including the Java Console output.

All of the x11vnc Java Viewer applet parameters are described in the
file classes/ssl/README


Notes on the VNC Viewer ss_vncviewer wrapper script:

If you want to use a native VNC Viewer with the SSL enabled x11vnc you
will need to run an external SSL tunnel on the Viewer side. There do
not seem to be any native SSL VNC Viewers outside of our x11vnc and
SSVNC packages. The basic ideas of doing this were discussed for
external tunnel utilities here.

The ss_vncviewer script provided with x11vnc and SSVNC can set up the
stunnel tunnel automatically on unix as long as the stunnel command is
installed on the Viewer machine and available in PATH (and vncviewer
too of course.) Note that on a Debian based system you will need to
install the package stunnel4 not stunnel. You can set the environment
variables STUNNEL and VNCVIEWERCMD to point to the correct programs if
you want to override the defaults.

Here are some examples:

```
1)  ss_vncviewer far-away.east:0

2)  ss_vncviewer far-away.east:0 -encodings "copyrect tight zrle hextile"

3)  ss_vncviewer -verify ./server.crt far-away.east:0

4)  ss_vncviewer -mycert ./client.pem far-away.east:0

5)  ss_vncviewer -proxy far-away.east:8080 myworkstation:0
```

The first one is the default mode and accepts the x11vnc certificate
without question. The second one is as the first, but adds the
-encodings options to the vncviewer command line.

The third one requires that the x11vnc server authenticate itself to
the client against the certificate in the file ./server.crt (e.g. one
created by "x11vnc -ssl SAVE" and safely copied to the VNC viewer
machine.)

The fourth one is for VNC Viewer authentication, it uses ./client.pem
to authenticate itself to x11vnc. One can supply both -verify and
-mycert simultaneously.

The fifth one shows that Web proxies can be used if that is the only
way to get out of the firewall. If the "double proxy" situation arises
separate the two by commas. See this page for more information on how
Web proxies come into play.

If one uses a Certificate Authority (CA) scheme described here, the
wrapper script would use the CA cert instead of the server cert:

```
3')  ss_vncviewer -verify ./cacert.crt far-away.east:0
```

Update Jul/2006: we now provide an Enhanced TightVNC Viewer (SSVNC)
package that starts up STUNNEL automatically along with some other
features. All binaries (stunnel, vncviewer, and some utilities) are
provided in the package. It works on Unix, Mac OS X, and Windows.


### Q-55: How do I use the Java applet VNC Viewer with built-in SSL tunneling when going through a Web Proxy?

The SSL enabled Java VNC Viewer and firewall Proxies:

SSL and HTTPS aside, there is a general problem with Firewall Proxies
and Java Applets that open sockets. The applet is downloaded
successfully (through the browser) using HTTP and the proxy, but when
the applet tries to reconnect to the originating host (the only one
allowed by security) it does not use the proxy channel. So it cannot
reconnect to the server the applet came from!

We have found a convenient workaround: in the directory where
VncViewer.jar resides there is a digitally signed version of the same
applet called SignedVncViewer.jar. Since the applet is digitally
signed, there will be an additional dialog from the Java VM plugin
asking you if you want to trust the applet fully.

You should say "Yes". If you do, the applet will be run in a mode
where it can try to determine the firewall proxy host name and port
(it will ask you for them if it cannot find them.) This way it can
connect directly to the Proxy and then request the CONNECT method to
be redirected to the originating host (the x11vnc VNC Server.) SSL is
then layered over this socket.

To do this you should use the proxy.vnc HTML file like via this URL in
your browser:

```
https://yourmachine.com:5900/proxy.vnc
```

(instead of the unsigned one in https://yourmachine.com:5900/ that
gives the default index.vnc)

Proxies that limit CONNECT to ports 443 and 563:

Things become trickier if the Web proxy restricts which CONNECT ports
can be redirected to. For security, some (most?) proxies only allow
port 443 (HTTPS) and 563 (SNEWS) by default. In this case, the only
thing to do is run x11vnc on that low port, e.g. "-rfbport 443", (or
use a port redirection on, say, a firewall or router port 443 to the
internal machine.)

If you do such a redirection to an internal machine and x11vnc is not
listening on port 443, you will probably need to edit proxy.vnc.
Suppose the SSL x11vnc server was listening on port 5901. You should
change the line in proxy.vnc from:

```
<param name=PORT value=$PORT>
```
to:

```
<param name=PORT value=443>
```

Since otherwise $PORT will be expanded to 5901 by x11vnc and the
viewer applet will fail to connect to that port on the firewall.

Another way to achieve the same thing is to use the applet PORT
parameter:

```
https://yourmachine.com/proxy.vnc?PORT=443
```

this is cleaner because it avoids editing the file, but requires more
parameters in the URL. See also the -httpsredir x11vnc option that
will try to automate this for you. To use the GET trick discussed
above, do:

```
https://yourmachine.com/proxy.vnc?GET=1&PORT=443
```

All of the x11vnc Java Viewer applet parameters are described in the
file classes/ssl/README

Here is an example of Java Console and x11vnc output for the Web proxy
case.


Note that both the ss_vncviewer stunnel Unix wrapper script and
Enhanced TightVNC Viewer (SSVNC) can use Web proxies as well even
though they do not involve a Web browser.


### Q-56: Can Apache web server act as a gateway for users to connect via SSL from the Internet with a Web browser to x11vnc running on their workstations behind a firewall?

Yes. You will need to configure apache to forward these connections.
It is discussed here. This SSL VNC portal provides a clean alternative
to the traditional method where the user uses SSH to log in through
the gateway to create the encrypted port redirection to x11vnc running
on her desktop.

Also see the desktop.cgi CGI script method that achieves much of what
this Apache VNC SSL portal method does (as long as desktop.cgi's 'port
redirection' mode is enabled.)


### Q-57: Can I create and use my own SSL Certificate Authority (CA) with x11vnc?

Yes, see this page for how to do this and the utility commands x11vnc
provides to create and manage many types of certificates and private
keys.


## Display Managers and Services

### Q-58: How can I run x11vnc as a "service" that is always available?

There are a number of ways to do this. The primary thing you need to
decide is whether you want x11vnc to connect to the X session on the
machine 1) regardless of who (or if anyone) has the X session, or 2)
only if a certain user has the X session. Because X sessions are
protected by X permissions (MIT-MAGIC-COOKIE files XAUTHORITY and
$HOME/.Xauthority) the automatically started x11vnc will of course
need to have sufficient permissions to connect to the X display.

Here are some ideas:

* Use the description under "Continuously" in the FAQ on x11vnc and
  Display Managers
* Use the description in the FAQ on x11vnc and inetd(8)
* Use the description in the FAQ on Unix user logins and inetd(8)
* Start x11vnc from your $HOME/.xsession (or $HOME/.xinitrc or
  autostart script or ...)
* Although less reliable, see the x11vnc_loop rc.local hack below.

The display manager scheme will not be specific to which user has the
X session unless a test is specifically put into the display startup
script (often named Xsetup.) The inetd(8) scheme may or may not be
specific to which user has the X session (and it may not be able to do
all users via the XAUTHORITY permission issues.)

The .xsession/.xinitrc scheme is obviously is specific to a particular
user and only when they are logged into X. If you do not know what a
$HOME/.xsession script is or how to use one, perhaps your desktop has
a "session startup commands" configuration option. The command to be
run in the .xsession or .xinitrc file may look like this:

```
x11vnc -logfile $HOME/.x11vnc.log -rfbauth $HOME/.vnc/passwd -forever -bg
```

plus any other options you desire.

Depending on your desktop and/or OS/distribution the automatically run
X startup scripts (traditionally .xsession/.xinitrc) may have to be in
a different directory or have a different basename. One user
recommends the description under 'Running Scripts Automatically' at
this link.

Firewalls: note all methods will require the host-level firewall to be
configured to allow connections in on a port. E.g. 5900 (default VNC
port) or 22 (default SSH port for tunnelling VNC.) Most systems these
days have firewalls turned on by default, so you will actively have to
do something to poke a hole in the firewall at the desired port
number. See your system administration tool for Firewall settings
(Yast, Firestarter, etc.)


### Q-59: How can I use x11vnc to connect to an X login screen like xdm, GNOME gdm, KDE kdm, or CDE dtlogin? (i.e. nobody is logged into an X session yet.)

We describe two scenarios here. The first is called 'One time only'
meaning you just need to do it quickly once and don't want to repeat;
and the second is called 'Continuously' meaning you want the access to
be available after every reboot and after every desktop logout.
    ---

One time only:   If the X login screen is running and you just want to
connect to it once (i.e. a one-shot):

It is usually possible to do this by just adjusting the XAUTHORITY
environment variable to point to the correct MIT-COOKIE auth file
while running x11vnc as root, e.g. for the gnome display manager, GDM:

```
x11vnc -auth /var/gdm/:0.Xauth -display :0
```

(the -auth option sets the XAUTHORITY variable for you.)

There will be a similar thing to do for xdm using however a different
auth directory path (perhaps something like
/var/lib/xdm/authdir/authfiles/A:0-XQvaJk) for the xdm greeter or
/var/lib/kdm/A:0-crWk72 (or /var/run/xauth/A:0-qQPftr, etc. etc) for
the kdm greeter. Of course, the random characters in the file basename
will vary and you will need to use the actual filename on your system.
Read your system docs to find out where the display manager cookie
files are kept.

Trick: sometimes ps(1) can reveal the X server process -auth argument
(e.g. "ps wwaux | grep auth") and hence the path to the auth file.

x11vnc must be run as root for this because the /var/gdm/:0.Xauth,
/var/lib/kdm/A:0-crWk72, etc. auth files are only readable by root. If
you do not want to run x11vnc as root, you can copy (as root or sudo)
the auth file to some location and make it readable by your userid.
Then run x11vnc as your userid with -auth pointed to the copied file.

Update Dec/2009: use "-auth guess" to have x11vnc try to guess the
location of the auth file for you.

You next connect to x11vnc with a VNC viewer, give your username and
password to the X login prompt to start your session.

Note:  GDM: gdm seems to have an annoying setting that causes x11vnc
(and any other X clients) to be killed after the user logs in. Setting
KillInitClients=false in the [daemon] section of /etc/X11/gdm/gdm.conf
(or /etc/gdm/gdm.conf, etc.) avoids this. Otherwise, just restart
x11vnc and then reconnect your viewer. Other display managers (kdm,
etc) may also have a similar problem. One user reports having to alter
"gdm.conf-custom" as well.

Note:  Solaris: For dtlogin in addition to the above sort of trick
(BTW, the auth file should be in /var/dt), you'll also need to add
something like Dtlogin*grabServer:False to the Xconfig file
(/etc/dt/config/Xconfig or /usr/dt/config/Xconfig on Solaris, see the
example at the end of this FAQ.) Then restart dtlogin, e.g.:
/etc/init.d/dtlogin stop; /etc/init.d/dtlogin start or reboot.

Update Nov/2008: Regarding GDM KillInitClients: see the -reopen option
for another possible workaround.

Update Oct/2009: Regarding GDM KillInitClients: starting with x11vnc
0.9.9 it will try to apply heuristics to detect if a window manager is
not running (i.e. whether the Display Manager Greeter Login panel is
still up.) If it thinks the display manager login is still up it will
delay creating windows or using XFIXES. The former is what GDM uses to
kill the initial clients, use of the latter can cause a different
problem: an Xorg server crash. So with 0.9.9 and later it should all
work without needing to set KillInitClients=false (which is a good
because recent GDM, v2.24, has removed this option) or use -noxfixes.
To disable the heuristics and delaying set X11VNC_AVOID_WINDOWS=never;
to set the delay time explicitly use, e.g., X11VNC_AVOID_WINDOWS=120
(delays for 120 seconds after the VNC connection; you have that long
to log in.)

Continuously:   Have x11vnc reattach each time the X server is
restarted (i.e. after each logout and reboot):

To make x11vnc always attached to the X server including the login
screen you will need to add a command to a display manager startup
script.

Please consider the security implications of this! The VNC display for
the X session always accessible (but hopefully password protected.)
Add -localhost if you only plan to access via a SSH tunnel.

The name of the display manager startup script file depends on desktop
used and seem to be:

```
GDM (GNOME)  /etc/X11/gdm/Init/Default
             /etc/gdm/Init/Default
KDM (KDE)    /etc/kde*/kdm/Xsetup
XDM          /etc/X11/xdm/Xsetup          (or sometimes xdm/Xsetup_0)
CDE          /etc/dt/config/Xsetup
```

although the exact location can be operating system, distribution, and
time dependent. See the documentation for your display manager:
gdm(1), kdm(1), xdm(1), dtlogin(1) for additional details. There may
also be display number specific scripts: e.g. Xsetup_0 vs. Xsetup, you
need to watch out for.

Note:  You should read and understand all of the Note's and Update's
in the 'One time only' section above. All of the GDM topics apply here
as well:

Note:  GDM: The above (in 'One time only') gdm setting of
KillInitClients=false in /etc/X11/gdm/gdm.conf (or /etc/gdm/gdm.conf,
etc.) for GDM is needed here as well. Other display managers (KDM,
etc) may also have a similar problem.

Also see the Update Oct/2009 above where x11vnc 0.9.9 and later
automatically avoids being killed.

Note:  DtLogin: The above (in 'One time only')
Dtlogin*grabServer:False step for Solaris will be needed for dtlogin
here as well.

In any event, the line you will add to the display manager script
(Xsetup, Default, or whatever) will look something like:

```
/usr/local/bin/x11vnc -rfbauth /path/to/the/vnc/passwd -o /var/log/x11vnc.log -forever -bg
```

where you should customize the exact command to your needs (e.g.
-localhost for SSH tunnel-only access; -ssl SAVE for SSL access; etc.)

Happy, happy, joy, joy:  Note that we do not need to specify -display
or -auth because happily they are already set for us in the DISPLAY
and XAUTHORITY environment variables for the Xsetup script!!!

You may also want to force the VNC port with something like "-rfbport
5900" (or -N) to avoid autoselecting one if 5900 is already taken.

Fedora/gdm: Here is an example of what we did on a vanilla install of
Fedora-C3 (seems to use gdm by default.) Add a line like this to
/etc/X11/gdm/Init/:0

```
/usr/local/bin/x11vnc -rfbauth /etc/x11vnc.passwd -forever -bg -o /var/log/x11vnc.log
```

And then add this line to /etc/X11/gdm/gdm.conf (or /etc/gdm/gdm.conf,
etc.) in the [daemon] section:

```
KillInitClients=false
```

Then restart: /usr/sbin/gdm-restart (or reboot.) The
KillInitClients=false setting is important: without it x11vnc will be
killed immediately after the user logs in. Here are full details on
how to configure gdm
    ---

Solaris/dtlogin: Here is an example of what we did on a vanilla
install of Solaris:
Make the directory /etc/dt/config:

```
mkdir -p /etc/dt/config
```

Copy over the Xconfig file for customization:

```
cp /usr/dt/config/Xconfig /etc/dt/config/Xconfig
```

Edit /etc/dt/config/Xconfig and uncomment the line:

```
Dtlogin*grabServer:        False
```

Next, copy over Xsetup for customization:

```
cp /usr/dt/config/Xsetup /etc/dt/config/Xsetup
```

Edit /etc/dt/config/Xsetup and at the bottom put a line like:

```
/usr/local/bin/x11vnc -forever -o /var/log/x11vnc.log -bg
```
(tweaked to your local setup and preferences, a password via -rfbauth,
etc. would be a very good idea.)

Restart the X server and dtlogin:

```
/etc/init.d/dtlogin stop
/etc/init.d/dtlogin start
```

(or reboot or maybe just restart the X session.)

    ---

KDM: One user running the kdm display manager reports putting this
line:

```
x11vnc -forever -rfbauth /home/xyz/.vnc/passwd -bg -o /var/log/x11vnc.log
```

in /etc/kde/kdm/Xsetup. After rebooting the system it all seemed to
work fine.

    ---


If you do not want to deal with any display manager startup scripts,
here is a kludgey script that can be run manually or out of a boot
file like rc.local: x11vnc_loop It will need some local customization
before running. Because the XAUTHORITY auth file must be guessed by
this script, use of the display manager script method described above
is greatly preferred. There is also the -loop option that does
something similar.

If the machine is a traditional Xterminal you may want to read this
FAQ.

Firewalls: note all methods will require the host-level firewall to be
configured to allow connections in on a port. E.g. 5900 (default VNC
port) or 22 (default SSH port for tunnelling VNC.) Most systems these
days have firewalls turned on by default, so you will actively have to
do something to poke a hole in the firewall at the desired port
number. See your system administration tool for Firewall settings
(Yast, Firestarter, etc.)


### Q-60: Can I run x11vnc out of inetd(8)? How about xinetd(8)?

Yes, perhaps a line something like this in /etc/inetd.conf will do it
for you:

```
5900 stream tcp nowait root /usr/sbin/tcpd /usr/local/bin/x11vnc_sh
```

where the shell script /usr/local/bin/x11vnc_sh uses the -inetd option
and looks something like (you'll need to customize to your settings.)

```
#!/bin/sh
/usr/local/bin/x11vnc -inetd -display :0 -auth /home/fred/.Xauthority \
        -rfbauth /home/fred/.vnc/passwd -o /var/log/x11vnc_sh.log
```

Important:  Note that you must redirect the standard error output to a
log file (e.g. -o logfile) or "2>/dev/null" for proper operation via
inetd (otherwise the standard error also goes to the VNC vncviewer,
and that confuses it greatly, causing it to abort.) If you do not use
a wrapper script as above but rather call x11vnc directly in
/etc/inetd.conf and do not redirect stderr to a file, then you must
specify the -q (aka -quiet) option: "/usr/local/bin/x11vnc -q -inetd
...". When you supply both -q and -inet and no "-o logfile" then
stderr will automatically be closed (to prevent, e.g. library stderr
messages leaking out to the viewer.) The recommended practice is to
use "-o logfile" to collect the output in a file or wrapper script
with "2>logfile" redirection because the errors and warnings printed
out are very useful in troubleshooting problems.

Note also the need to set XAUTHORITY via -auth to point to the
MIT-COOKIE auth file to get permission to connect to the X display
(setting and exporting the XAUTHORITY variable accomplishes the same
thing.) See the x11vnc_loop file in the previous question for more
ideas on what that auth file may be, etc. The scheme described in the
FAQ on Unix user logins and inetd(8) works around the XAUTHORITY issue
nicely.

Note:  On Solaris you cannot have the bare number 5900 in
/etc/inetd.conf, you'll need to replace it with a word like x11vnc an
then put something like "x11vnc 5900/tcp" in /etc/services.

Since the process runs as root, it might be a bad idea to have the
logfile in a world-writable area like /tmp if there are untrustworthy
users on the machine. Perhaps /var/log is a better place.

Be sure to look at your /etc/hosts.allow and /etc/hosts.deny settings
to limit the machines that can connect to this service (your desktop!)
For the above example with /etc/hosts.allow:

```
x11vnc_sh : 123.45.67.89
```

A really safe way to do things is to limit the above inetd to
localhost only (via /etc/hosts.allow) and use ssh to tunnel the
incoming connection. Using inetd for this prevents there being a tiny
window of opportunity between x11vnc starting up and your vncviewer
connecting to it. Always use a VNC password to further protect against
unwanted access.

For xinetd(8), one user reports he created the file
/etc/xinetd.d/x11vncservice containing the following:

```
# default: off
# description:
service x11vncservice
{
        flags           = REUSE NAMEINARGS
        port            = 5900
        type            = UNLISTED
        socket_type     = stream
        protocol        = tcp
        wait            = no
        user            = root
        server          = /usr/sbin/tcpd
        server_args     = /usr/local/bin/x11vnc_sh
        disable         = no
}
```

With the contents of /usr/local/bin/x11vnc_sh similar to the example
given above. One user reports this works with avoiding the wrapper
script:

```
service x11vncservice
{
        port            = 5900
        type            = UNLISTED
        socket_type     = stream
        protocol        = tcp
        wait            = no
        user            = root
        server          = /usr/local/bin/x11vnc
        server_args     = -inetd -q -display :0 -auth /var/gdm/:0.Xauth
        disable         = no
}
```

(or one can replace the -q with say "-o /var/log/x11vnc.log" to
capture a log)

The above works nicely for GDM because the -auth file is a fixed name.
For KDM or XDM the filename varies. Here is one idea for a x11vnc_sh
wrapper to try to guess the name:

```
#!/bin/sh
COLUMNS=256
export COLUMNS
authfile=`ps wwaux | grep '/X.*-auth' | grep -v grep | sed -e 's/^.*-auth *//'
-e 's/ .*$//' | head -n 1`

if [ -r "$authfile" ]; then
        exec /usr/local/bin/x11vnc -inetd -o /var/log/x11vnc.log -display :0 -a
uth "$authfile"
fi
exit 1
```

Starting with x11vnc 0.9.3 this can be automated by:

```
#!/bin/sh
exec /usr/local/bin/x11vnc -inetd -o /var/log/x11vnc.log -find -env FD_XDM=1
```


### Q-61: Can I have x11vnc advertise its VNC service and port via mDNS/Zeroconf (e.g. Avahi) so VNC viewers on the local network can detect it automatically?

Yes, as of Feb/2007 x11vnc supports mDNS / Zeroconf advertising of its
service via the Avahi client library. Use the option -avahi (same as
-mdns or -zeroconf) to enable it. Depending on your setup you may need
to install Avahi (including the development/build packages), enable
the server: avahi-daemon and avahi-dnsconfd, and possibly open up UDP
port 5353 on your firewall.

If the Avahi client library or build environment is not available at
build-time, then at run-time x11vnc will try to look for external
helper programs, avahi-browse(1) or dns-sd(1), to do the work.

The service was tested with Chicken of the VNC ("Use Bonjour"
selected) on a Mac on the same network and the service was noted and
listed in the servers list. Clicking on it and then "Connect"
connected automatically w/o having to enter any hostnames or port
numbers.

It appears SuSE 10.1 comes with avahi (or you can add packages, e.g.
avahi-0.6.5-27) but not the development package (you can use the
OpenSuSE avahi-devel rpm.) Unfortunately, you may need to disable
another Zeroconf daemon "/etc/init.d/mdnsd stop", before doing
"/etc/init.d/avahi-daemon start" and "/etc/init.d/avahi-dnsconfd
start". We also had to comment out the browse-domains line in
/etc/avahi/avahi-daemon.conf. Hopefully there is "LessConf" to do on
other distros/OS's...


### Q-62: Can I have x11vnc allow a user to log in with her UNIX username and password and then have it find her X session display on that machine and then attach to it? How about starting an X session if one cannot be found?

The easiest way to do this is via inetd(8) using the -unixpw and
-display WAIT options. The reason inetd(8) makes this easier is that
it starts a new x11vnc process for each new user connection. Otherwise
a wrapper would have to listen for connections and spawn new x11vnc's
(see this example and also the -loopbg option.) inetd(8) is not
required for this, but it makes some aspects more general.

Also with inetd(8) users always connect to a fixed VNC display, say
hostname:0, and do not need to memorize a special VNC display number
just for their personal use, etc.

Update: Use the -find, -create, -svc, and -xdmsvc options that are
shorthand for common FINDCREATEDISPLAY usage modes (e.g. terminal
services) described below. (i.e. simply use "-svc" instead of the
cumbersome "-display WAIT:cmd=FINDCREATEDISPLAY-Xvfb -unixpw -users
unixpw= -ssl SAVE")

The -display WAIT option makes x11vnc wait until a VNC viewer is
connected before attaching to the X display.

Additionally it can be used to run an external command that returns
the DISPLAY and XAUTHORITY data. We provide some useful builtin ones
(FINDDISPLAY and FINDCREATEDISPLAY below), but in principle one could
supply his own script: "-display WAIT:cmd=/path/to/find_display" where
the script find_display might look something like this.

A default script somewhat like the above is used under "-display
WAIT:cmd=FINDDISPLAY" (same as -find) The format for any such command
is that it returns DISPLAY=:disp as the first line and any remaining
lines are either XAUTHORITY=file or raw xauth data (the above example
does the latter.) If applicable (-unixpw mode), the program is run as
the Unix user name who logged in.

On Linux if the virtual terminal is known the program appends ",VT=n"
to the DISPLAY line; a chvt n will be attempted automatically. Or if
only the X server process ID is known it appends ",XPID=n" (a chvt
will be attempted by x11vnc.)

Tip: Note that the -find option is an alias for "-display
WAIT:cmd=FINDDISPLAY". Use it!

   The -unixpw option allows UNIX password logins. It conveniently knows
   the Unix username whose X display should be found. Here are a couple
   /etc/inetd.conf examples of this usage:

```
5900  stream  tcp  nowait  nobody  /usr/sbin/tcpd /usr/local/bin/x11vnc -inetd -unixpw \
      -find -o /var/log/x11vnc.log -ssl SAVE -ssldir /usr/local/certs
5900  stream  tcp  nowait  root    /usr/sbin/tcpd /usr/local/bin/x11vnc -inetd -unixpw \
      -find -o /var/log/x11vnc.log -ssl SAVE -users unixpw=
```

Note we have used the -find alias and the very long lines have been
split. An alternative is to use a wrapper script, e.g.
/usr/local/bin/x11vnc.sh that has all of the options. (see also the
-svc alias.)

In the first inetd line x11vnc is run as user "nobody" and stays user
nobody during the whole session. The permissions of the log files and
certs directory will need to be set up to allow "nobody" to use them.

In the second one x11vnc is run as root and switches to the user that
logs in due to the "-users unixpw=" option.

Note that SSL is required for this mode because otherwise the Unix
password would be passed in clear text over the network. In general
-unixpw is not required for this sort of scheme, but it is convenient
because it determines exactly who the Unix user is whose display
should be sought. Otherwise the find_display script would have to use
some method to work out DISPLAY, XAUTHORITY, etc (perhaps you use
multiple inetd ports and hardwire usernames for different ports.)

If you really want to disable the SSL or SSH -localhost constraints
(this is not recommended unless you really know what you are doing:
Unix passwords sent in clear text is a very bad idea...) read the
-unixpw documentation.

   A inetd(8) scheme for a fixed user that doesn't use SSL or unix
   passwds could be:

```
/usr/local/bin/x11vnc -inetd -users =fred -find -rfbauth /home/fred/.vnc/pass
wd -o /var/log/x11vnc.log
```

The "-users =fred" option will cause x11vnc to switch to user fred and
then find his X display. The VNC password (-rfbauth) as opposed to
Unix password (-unixpw) is used to authenticate the VNC client.

Similar looking commands to the above examples can be run directly and
do not use inetd (just remove the -inetd option and run from the
cmdline, etc.)


X Session Creation: An added (Nov/2006) extension to FINDDISPLAY is
FINDCREATEDISPLAY where if it does not find an X display via the
FINDDISPLAY method it will create an X server session for the user
(i.e. desktop/terminal server.) This is the only time x11vnc actually
tries to start up an X server (normally it just attaches to an
existing one.)

For virtual sessions you will need to install the Xvfb program (e.g.
apt-get install xvfb) or our Xdummy program (see below.)

By default it will only try to start up virtual (non-hardware) X
servers: first Xvfb and if that is not available then Xdummy (included
in the x11vnc source code.) Note that Xdummy only works on Linux
whereas Xvfb works just about everywhere (and in some situations
Xdummy must be run as root.) An advantage of Xdummy over Xvfb is that
Xdummy supports RANDR dynamic screen resizing, which can be handy if
the user accesses the desktop from different sized screens (e.g.
workstation and laptop.)

   So an inetd(8) example might look like:

```
5900 stream tcp nowait root /usr/sbin/tcpd /usr/local/bin/x11vnc -inetd \
      -o /var/log/x11vnc.log -http -prog /usr/local/bin/x11vnc \
      -ssl SAVE -unixpw -users unixpw= -display WAIT:cmd=FINDCREATEDISPLAY
```

Where the very long lines have been split. See below where that long
and cumbersome last line is replaced by the -svc alias.

The above mode will allow direct SSL (e.g. ss_vncviewer or SSVNC)
access and also Java Web browers access via: https://hostname:5900/.

Tip: Note that the -create option is an alias for "-display
WAIT:cmd=FINDCREATEDISPLAY-Xvfb".

Tip: Note that -svc is a short hand for the long "-ssl SAVE -unixpw
-users unixpw= -display WAIT:cmd=FINDCREATEDISPLAY" part. Unlike
-create, this alias also sets up SSL encryption and Unix password
login.

The above inetd example then simplifies to:

```
5900 stream tcp nowait root /usr/sbin/tcpd /usr/local/bin/x11vnc -inetd \
      -o /var/log/x11vnc.log -http -prog /usr/local/bin/x11vnc \
      -svc
```

Tip: In addition to the usual unixpw parameters, inside the VNC viewer
the user can specify after his username (following a ":" see -display
WAIT for details) for FINDCREATEDISPLAY they can add "geom=WxH" or
"geom=WxHxD" to specify the width, height, and optionally the color
depth. E.g. "fred:geom=800x600" at the login: prompt. Also if the env.
var X11VNC_CREATE_GEOM is set to the desired WxH or WxHxD that will be
used by x11vnc.

You can set the env. var X11VNC_SKIP_DISPLAY to a comma separated list
of displays to ignore in the FINDDISPLAY process (to force creation of
new displays in some cases.) The user logging in via the vncviewer can
also set this via username:nodisplay=...)

If you do not plan on using the Java Web browser applet you can remove
the -http (and -prog) option since this will speed up logging-in by a
few seconds (x11vnc will not have to wait to see if a connection is
HTTPS or VNC.)

For reference, xinetd format in the file, say, /etc/xinetd.d/x11vnc:

```
service x11vnc
{
        type            = UNLISTED
        port            = 5900
        socket_type     = stream
        protocol        = tcp
        wait            = no
        user            = root
        server          = /usr/local/bin/x11vnc
        server_args     = -inetd -o /var/log/x11vnc.log -http -prog /usr/local/
bin/x11vnc -svc
        disable         = no
}
```

To print out the script in this case use "-display
WAIT:cmd=FINDCREATEDISPLAY-print". To change the preference of
Xservers and which to try list them, e.g.: "-display
WAIT:cmd=FINDCREATEDISPLAY-X,Xvfb,Xdummy" or use "-create_xsrv
X,Xvfb,Xdummy". The "X" one means to try to start up a real, hardware
X server, e.g. startx(1) (if there is already a real X server running
this may only work on Linux and the chvt program may need to be run to
switch to the correct Linux virtual terminal.) x11vnc will try to run
chvt automatically if it can determine which VT should be switched to.

XDM/GDM/KDM Login Greeter Panel: If you want to present the user with
a xdm/gdm/kdm display manager "greeter" login you can use Xvfb.xdmcp
instead of Xvfb, etc in the above list. However, you need to configure
xdm/gdm/kdm to accept localhost XDMCP messages, this can be done by
(from -help output):

```
If you want the FINDCREATEDISPLAY session to contact an XDMCP login
manager (xdm/gdm/kdm) on the same machine, then use "Xvfb.xdmcp"
instead of "Xvfb", etc.  The user will have to supply his username
and password one more time (but he gets to select his desktop
type so that can be useful.)  For this to work, you will need to
enable localhost XDMCP (udp port 177) for the display manager.
This seems to be:

 for gdm in gdm.conf:   Enable=true in section [xdmcp]
 for kdm in kdmrc:      Enable=true in section [Xdmcp]
 for xdm in xdm-config: DisplayManager.requestPort: 177
```

Unless you are also providing XDMCP service to xterminals or other
machines, make sure that the host access list only allows local
connections (the name of this file is often Xaccess and it is usually
setup by default to do just that.) Nowadays, host level firewalling
will also typically block UDP (port 177 for XDMCP) by default
effectively limiting the UDP connections to localhost.

   Tip: Note that -xdmsvc is a short hand alias for the long "-ssl SAVE
   -unixpw -users unixpw= -display
   WAIT:cmd=FINDCREATEDISPLAY-Xvfb.xdmcp". So we simply use:

```
service x11vnc
{
        type            = UNLISTED
        port            = 5900
        socket_type     = stream
        protocol        = tcp
        wait            = no
        user            = root
        server          = /usr/local/bin/x11vnc
        server_args     = -inetd -o /var/log/x11vnc.log -xdmsvc
        disable         = no
}
```

(Note: use "-svc" instead of "-xdmsvc" for no XDMCP login greeter.)


Local access (VNC Server and VNC Viewer on the same machine): To
access your virtual X display session locally (i.e. while sitting at
the same machine it is running on) one can perhaps have something like
this in their $HOME/.xinitrc

```
#!/bin/sh
x11vnc -create -rfbport 5905 -env WAITBG=1
vncviewer -geometry +0+0 -encodings raw -passwd $HOME/.vnc/passwd localhost:5
```

You may not need the -passwd. Recent RealVNC viewers might be this:

```
#!/bin/sh
x11vnc -create -rfbport 5905 -env WAITBG=1
vncviewer -FullScreen -PreferredEncoding raw -passwd $HOME/.vnc/passwd localhost:5
```

This way a bare X server is run with no window manager or desktop; it
simply runs only the VNC Viewer on the real X server. The Viewer then
draws the virtual X session on to the real one. On your system it
might not be $HOME/.xinitrc, but rather .xsession, .Xclients, or
something else. You will need to figure out what it is for your system
and configuration.

There may be a problem if the resolution (WxH) of the virtual X
display does not match that of the physical X display.

If you do not want to or cannot figure out the X startup script name
(.xinitrc, etc) you could save the above commands to a shell script,
say "vnclocal", and the log in via the normal KDM or GDM greeter
program using the "Failsafe" option. Then in the lone xterm that comes
up type "vnclocal" to connect to your virtual X display via x11vnc and
vncviewer.

---

Summary: The "-display WAIT:cmd=FINDCREATEDISPLAY" scheme can be used
to provide a "desktop service" (i.e. terminal service) on the server
machine: you always get some desktop there, either a real hardware X
server or a virtual one (depending on how you set things up.)

So it provides simple "terminal services" based on Unix username and
password. The created X server sessions (virtual or real hardware)
will remain running after you disconnect the VNC viewer and will be
found again on reconnecting via VNC and logging in. To terminate them
use the normal way to Exit/LogOut from inside your X session. The user
does not have to memorize which VNC display number is his. They all go
the same one (e.g. hostname:0) and it switches based on username.


### Q-63: Can I have x11vnc restart itself after it terminates?

One could do this in a shell script, but now there is an option -loop
that makes it easier. Of course when x11vnc restarts it needs to have
permissions to connect to the (potentially new) X display. This mode
could be useful if the X server restarts often. Use e.g. "-loop5000"
to sleep 5000 ms between restarts. Also "-loop2000,5" to sleep 2000 ms
and only restart 5 times.

One can also use the -loopbg to emulate inetd(8) to some degree, where
each connected process runs in the background. It could be combined,
say, with the -svc option to provide simple terminal services without
using inetd(8).


### Q-64: How do I make x11vnc work with the Java VNC viewer applet in a web browser?

To have x11vnc serve up a Java VNC viewer applet to any web browsers
that connect to it, run x11vnc with this option:

```
  -httpdir /path/to/the/java/classes/dir
```

(this directory will contain the files index.vnc and, for example,
VncViewer.jar) Note that libvncserver contains the TightVNC Java
classes jar file for your convenience. (it is the file
classes/VncViewer.jar in the source tree.)

You will see output something like this:

```
14/05/2004 11:13:56 Autoprobing selected port 5900
14/05/2004 11:13:56 Listening for HTTP connections on TCP port 5800
14/05/2004 11:13:56   URL http://walnut:5800
14/05/2004 11:13:56 screen setup finished.
14/05/2004 11:13:56 The VNC desktop is walnut:0
PORT=5900
```

then you can connect to that URL with any Java enabled browser. Feel
free to customize the default index.vnc file in the classes directory.

As of May/2005 the -http option will try to guess where the Java
classes jar file is by looking in expected locations and ones relative
to the x11vnc binary.

Also note that if you wanted to, you could also start the Java viewer
entirely from the viewer-side by having the jar file there and using
either the java or appletviewer commands to run the program.

```
java -cp ./VncViewer.jar VncViewer HOST far-away.east PORT 5900
```

Proxies: See the discussion here if the web browser must use a web
proxy to connect to the internet. It is tricky to get Java applets to
work in this case: a signed applet must be used so it can connect to
the proxy and ask for the redirection to the VNC server. One way to do
this is to use the signed SSL one referred to in classes/ssl/proxy.vnc
and set disableSSL=yes (note that this has no encryption; please use
SSL or SSH as discuss elsewhere on this page) in the URL or the file.


### Q-65: Are reverse connections (i.e. the VNC server connecting to the VNC viewer) using "vncviewer -listen" and vncconnect(1) supported?

As of Mar/2004 x11vnc supports reverse connections. On Unix one starts
the VNC viewer in listen mode: "vncviewer -listen" (see your
documentation for Windows, etc), and then starts up x11vnc with the
-connect option. To connect immediately at x11vnc startup time use the
"-connect host:port" option (use commas for a list of hosts to connect
to.) The ":port" is optional (default is VNC listening port is 5500.)

If a file is specified instead: -connect /path/to/some/file then that
file is checked periodically (about once a second) for new hosts to
connect to.

The -remote control option (aka -R) can also be used to do this during
an active x11vnc session, e.g.:

```
x11vnc -display :0 -R connect:hostname.domain
```

Use the "-connect_or_exit" option to have x11vnc exit if the reverse
connection fails. Also, note the "-rfbport 0" option disables TCP
listening for connections (potentially useful for reverse connection
mode, assuming you do not want any "forward" connections.)

Note that as of Mar/2006 x11vnc requires password authentication for
reverse connections as well as for forward ones (assuming password
auth has been enabled, e.g. via -rfbauth, -passwdfile, etc.) Many VNC
servers do not require any password for reverse connections. To regain
the old behavior supply this option "-env
X11VNC_REVERSE_CONNECTION_NO_AUTH=1" to x11vnc.

Vncconnect command: To use the vncconnect(1) program (from the core
VNC package at www.realvnc.com) specify the -vncconnect option to
x11vnc (Note: as of Dec/2004 -vncconnect is now the default.)
vncconnect(1) must be pointed to the same X11 DISPLAY as x11vnc (since
it uses X properties to communicate with x11vnc.) If you do not have
or do not want to get the vncconnect(1) program, the following script
(named "Vncconnect") may work if your xprop(1) supports the -set
option:

```
#!/bin/sh
# usage: Vncconnect <host>
#        Vncconnect <host:port>
# note: not all xprop(1) support -set.
#
xprop -root -f VNC_CONNECT 8s -set VNC_CONNECT "$1"
```


### Q-66: Can reverse connections be made to go through a Web or SOCKS proxy or SSH?

Yes, as of Oct/2007 x11vnc supports reverse connections through
proxies: use the "-proxy host:port" option. The default is to assume
the proxy is a Web proxy. Note that most Web proxies only allow proxy
destination connections to ports 443 (HTTPS) and 563 (SNEWS) and so
this might not be too useful unless the proxy has been modified
(AllowCONNECT apache setting) or the VNC viewer listens on one of
those ports (or the router does a port redir.) A web proxy may also be
specified via "-proxy http://host:port"

For SOCKS4 and SOCKS4a proxies use this format "-proxy
socks://host:port". If the reverse connection hostname is a numerical
IP or "localhost" then SOCKS4 (no host lookup) is used, otherwise
SOCKS4a will be used. For SOCKS5 (proxy will do lookup and many other
things) use "-proxy socks5://host:port". Note that the SSH builtin
SOCKS proxy "ssh -D port" only does SOCKS4 or SOCKS5, so use socks5://
for a ssh -D proxy.

The proxying works for both SSL encrypted and normal reverse
connections.

An experimental mode is "-proxy http://host:port/..." where the URL
(e.g. a CGI script) is retrieved via the GET method. See -proxy for
more info.

Another experimental mode is "-proxy ssh://user@host" in which case a
SSH tunnel is used for the proxying. See -proxy for more info.

Up to 3 proxies may be chained together by listing them by commas
e.g.: "-proxy http://host1:port1,socks5://host2:port2" in case one
needs to ricochet off of several machines to ultimately reach the
listening viewer.


### Q-67: Can x11vnc provide a multi-user desktop web login service as an Apache CGI or PHP script?

Yes. See the example script desktop.cgi for ideas. It is in the source
tree in the directory x11vnc/misc. It serves x11vnc's SSL enabled Java
Applet to the web browser with the correct connection information for
the user's virtual desktop (an Xvfb session via -create; be sure to
add the Xvfb package.) HTTPS/SSL enabled Apache should be used to
serve the script to avoid unix and vnc passwords from being sent in
cleartext and sniffed.

By default it uses a separate VNC port for each user desktop (either
by autoprobing in a range of ports or using a port based on the userid
number.) The web server's firewall must allow incoming connections to
these ports.

It is somewhat difficult to do all of this with x11vnc listening on a
single port, however there is also a 'fixed port' scheme described in
the script based on -loopbg that works fairly well (but more
experience is needed to see what problems contention for the same port
causes; however at worst one user may need to re-login.)

There is also an optional 'port redirection' mode for desktop.cgi that
allows redirection to other machines inside the firewall already
running SSL enabled VNC servers. This provides much of the
functionality as the SSL Portal and is easier to set up.


### Q-68: Can I use x11vnc as a replacement for Xvnc? (i.e. not for a real display, but for a virtual one I keep around.)

You can, but you would not be doing this for performance reasons (for
virtual X sessions via VNC, Xvnc should give the fastest response.)
You may want to do this because Xvnc is buggy and crashes, does not
support an X server extension you desire, or you want to take
advantage of one of x11vnc's unending number of options and features.

One way to achieve this is to have a Xvfb(1) virtual framebuffer X
server running in the background and have x11vnc attached to it.
Another method, faster and more accurate, is to use the "dummy" Device
Driver in XFree86/Xorg (see below.)

For these virtual sessions you will need to install the Xvfb program
(e.g. apt-get install xvfb) or our Xdummy program (see below.)

In either case, one can view this desktop both remotely and also
locally using vncviewer. Make sure vncviewer's "-encodings raw" is in
effect for local viewing (compression seems to slow things down
locally.) For local viewing you set up a "bare" window manager that
just starts up vncviewer and nothing else (See how below.)

Here is one way to start up Xvfb:

```
xinit -- /usr/bin/Xvfb :1 -cc 4 -screen 0 1024x768x16
```

This starts up a 16bpp virtual display. To export it via VNC use

```
x11vnc -display :1 ...
```

Then have the remote vncviewer attach to x11vnc's VNC display (e.g. :0
which is port 5900.)

The "-cc 4" Xvfb option is to force it to use a TrueColor visual
instead of DirectColor (this works around a recent bug in the Xorg
Xvfb server.)

One good thing about Xvfb is that the virtual framebuffer exists in
main memory (rather than in the video hardware), and so x11vnc can
"screen scrape" it very efficiently (more than, say, 100X faster than
normal video hardware.)

Update Nov/2006: See the FINDCREATEDISPLAY discussion of the "-display
WAIT:cmd=FINDDISPLAY" option where virtual (Xvfb or Xdummy, or even
real ones by changing an option) X servers are started automatically
for new users connecting. This provides a "desktop service" for the
machine. You either get your real X session or your virtual
(Xvfb/Xdummy) one whenever you connect to the machine (inetd(8) is a
nice way to provide this service.) The -find, -create, -svc, and
-xdmsvc aliases can also come in handy here.

There are some annoyances WRT Xvfb however. The default keyboard
mapping seems to be very poor. One should run x11vnc with -add_keysyms
option to have keysyms added automatically. Also, to add the Shift_R
and Control_R modifiers something like this is needed:

```
#!/bin/sh
xmodmap -e "keycode any = Shift_R"
xmodmap -e "add Shift = Shift_L Shift_R"
xmodmap -e "keycode any = Control_R"
xmodmap -e "add Control = Control_L Control_R"
xmodmap -e "keycode any = Alt_L"
xmodmap -e "keycode any = Alt_R"
xmodmap -e "keycode any = Meta_L"
xmodmap -e "add Mod1 = Alt_L Alt_R Meta_L"
```

(note: these are applied automatically in the FINDCREATEDISPLAY mode
of x11vnc.) Perhaps the Xvfb options -xkbdb or -xkbmap could be used
to get a better default keyboard mapping...

Dummy Driver:  A user points out a faster and more accurate method is
to use the "dummy" Device Driver of XFree86/Xorg instead of Xvfb. He
uses this to create a persistent and resizable desktop accessible from
anywhere. In the Device Section of the config file set Driver "dummy".
You may also need to set VideoRam NNN to be large enough to hold the
framebuffer. The framebuffer is kept in main memory like Xvfb except
that the server code is closely correlated with the real XFree86/Xorg
Xserver unlike Xvfb.

The main drawback to this method (besides requiring extra
configuration and possibly root permission) is that it also does the
Linux Virtual Console/Terminal (VC/VT) switching even though it does
not need to (since it doesn't use a real framebuffer.) There are some
"dual headed" (actually multi-headed/multi-user) patches to the X
server that turn off the VT usage in the X server. Update: As of
Jul/2005 we have an LD_PRELOAD script Xdummy that allows you to use a
stock (i.e. unpatched) Xorg or XFree86 server with the "dummy" driver
and not have any VT switching problems! An advantage of Xdummy over
Xvfb is that Xdummy supports RANDR dynamic screen resizing.

   The standard way to start the "dummy" driver would be:

```
startx -- :1 -config /etc/X11/xorg.conf.dummy
```

   where the file /etc/X11/xorg.conf.dummy has its Device Section
   modified as described above. To use the LD_PRELOAD wrapper script:

```
startx -- /path/to/Xdummy :1
```

An xdm(1) example is also provided.

In general, one can use these sorts of schemes to use x11vnc to export
other virtual X sessions, say Xnest or even Xvnc itself (useful for
testing x11vnc.)

Local access (VNC Server and VNC Viewer on the same machine): You use
a VNC viewer to access the display remotely; to access your virtual X
display locally (i.e. while sitting at the same machine it is running
on) one can perhaps have something like this in their $HOME/.xinitrc

```
#!/bin/sh
x11vnc -display :5 -rfbport 5905 -bg
vncviewer -geometry +0+0 -encodings raw -passwd $HOME/.vnc/passwd localhost:5
```

The display numbers (VNC and X) will likely be different (you could
also try -find), and you may not need the -passwd. Recent RealVNC
viewers might be this:

```
#!/bin/sh
x11vnc -display :5 -rfbport 5905 -bg
vncviewer -FullScreen -PreferredEncoding raw -passwd $HOME/.vnc/passwd localhost:5
```

This way a bare X server is run with no window manager or desktop; it
simply runs only the VNC Viewer on the real X server. The Viewer then
draws the virtual X session on to the real one. On your system it
might not be $HOME/.xinitrc, but rather .xsession, .Xclients, or
something else. You will need to figure out what it is for your system
and configuration.


XDM/GDM/KDM One-Shot X sessions: For the general replacement of Xvnc
by Xvfb+x11vnc, one user describes a similar setup he created where
the X sessions are one-shot's (destroyed after the vncviewer
disconnects) and it uses the XDM/GDM/KDM login greeter here.


### Q-69: How can I use x11vnc on "headless" machines? Why might I want to?

An interesting application of x11vnc is to let it export displays of
"headless" machines. For example, you may have some lab or server
machines with no keyboard, mouse, or monitor, but each one still has a
video card. One can use x11vnc to provide a simple "desktop service"
from these server machines.

An X server can be started on the headless machine (sometimes this
requires configuring the X server to not fail if it cannot detect a
keyboard or mouse, see the next paragraph.) Then you can export that X
display via x11vnc (e.g. see this FAQ) and access it from anywhere on
the network via a VNC viewer.

Some tips on getting X servers to start on machines without keyboard
or mouse: For XFree86/Xorg the Option "AllowMouseOpenFail" "true"
"ServerFlags" config file option is useful. On Solaris Xsun the
+nkeyboard and +nmouse options are useful (put them in the server
command line args in /etc/dt/config/Xservers.) There are patches
available for Xsun at lease back to Solaris 8 that support this. See
Xserver(1) for more info.

Although this usage may sound strange it can be quite useful for a GUI
(or other) testing or QA setups: the engineers do not need to walk to
lab machines running different hardware, OS's, versions, etc (or have
many different machines in their office.) They just connect to the
various test machines over the network via VNC. The advantage to
testing this way instead of using Xvnc or even Xvfb is that the test
is done using the real X server, fonts, video hardware, etc. that will
be used in the field.

One can imagine a single server machine crammed with as many video
cards as it can hold to provide multiple simultaneous access or
testing on different kinds of video hardware.

See also the FINDCREATEDISPLAY discussion of the "-display
WAIT:cmd=FINDDISPLAY" option where virtual Xvfb or Xdummy, or real X
servers are started automatically for new users connecting. The -find,
-create, -svc, and -xdmsvc aliases can also come in handy here.


## Resource Usage and Performance

### Q-70: I have lots of memory, but why does x11vnc fail with shmget: No space left on device    or    Minor opcode of failed request: 1 (X_ShmAttach)?

It is not a matter of free memory, but rather free shared memory (shm)
slots, also known as shm segments. This often occurs on a public
Solaris machine using the default of only 100 slots. You (or the owner
or root) can clean them out with ipcrm(1). x11vnc tries hard to
release its slots, but it, and other programs, are not always able to
(e.g. if kill -9'd.)

Sometimes x11vnc will notice the problem with shm segments and tries
to get by with fewer, only giving a warning like this:

```
19/03/2004 10:10:58 shmat(tile_row) failed.
shmat: Too many open files
19/03/2004 10:10:58 error creating tile-row shm for len=4
19/03/2004 10:10:58 reverting to single_copytile mode
```

Here is a shell script shm_clear to list and prompt for removal of
your unattached shm segments (attached ones are skipped.) I use it
while debugging x11vnc (I use "shm_clear -y" to assume "yes" for each
prompt.) If x11vnc is regularly not cleaning up its shm segments,
please contact me so we can work to improve the situation.

Longer term, on Solaris you can put something like this in
/etc/system:

```
set shmsys:shminfo_shmmax = 0x2000000
set shmsys:shminfo_shmmni = 0x1000
```

to sweep the problem under the rug (4096 slots.) On Linux, examine
/proc/sys/kernel/shmmni; you can modify the value by writing to that
file.

Things are even more tight on Solaris 8 and earlier, there is a
default maximum number of shm segments per process of 6. The error is
the X server (not x11vnc) being unable to attach to the segments, and
looks something like this:

```
30/04/2004 14:04:26 Got connection from client 192.168.1.23
30/04/2004 14:04:26   other clients:
X Error of failed request:  BadAccess (attempt to access private resource denied)
   Major opcode of failed request:  131 (MIT-SHM)
   Minor opcode of failed request:  1 (X_ShmAttach)
   Serial number of failed request:  14
   Current serial number in output stream:  17
```

This tight limit on Solaris 8 can be increased via:

```
set shmsys:shminfo_shmseg = 100
```

in /etc/system. See the next paragraph for more workarounds.

To minimize the number of shm segments used by x11vnc try using the
-onetile option (corresponds to only 3 shm segments used, and adding
-fs 1.0 knocks it down to 2.) If you are having much trouble with shm
segments, consider disabling shm completely via the -noshm option.
Performance will be somewhat degraded but when done over local machine
sockets it should be acceptable (see an earlier question discussing
-noshm.)


### Q-71: How can I make x11vnc use less system resources?

The -nap (now on by default; use -nonap to disable) and "-wait n"
(where n is the sleep between polls in milliseconds, the default is 30
or so) option are good places to start. In addition, something like
"-sb 15" will cause x11vnc to go into a deep-sleep mode after 15
seconds of no activity (instead of the default 60.)

Reducing the X server bits per pixel depth (e.g. to 16bpp or even
8bpp) will further decrease memory I/O and network I/O. The ShadowFB X
server setting will make x11vnc's screen polling less severe. Using
the -onetile option will use less memory and use fewer shared memory
slots (add -fs 1.0 for one less slot.)


### Q-72: How can I make x11vnc use MORE system resources?

You can try -threads (note this mode can be unstable and/or crash; and
as of May/2008 is strongly discouraged, see the option description) or
dial down the wait time (e.g. -wait 1) and possibly dial down -defer
as well. Note that if you try to increase the "frame rate" too much
you can bog down the server end with the extra work it needs to do
compressing the framebuffer data, etc.

That said, it is possible to "stream" video via x11vnc if the video
window is small enough. E.g. a 256x192 xawtv TV capture window (using
the x11vnc -id option) can be streamed over a LAN or wireless at a
reasonable frame rate. If the graphics card's framebuffer read rate is
faster than normal then the video window size and frame rate can be
much higher. The use of TurboVNC and/or TurboJPEG can make the frame
rate somewhat higher still (but most of this hinges on the graphics
card's read rate.)


### Q-73: I use x11vnc over a slow link with high latency (e.g. dialup modem or broadband), is there anything I can do to speed things up?

Some things you might want to experiment with (many of which will help
performance on faster links as well):

X server/session parameters:

* Configure the X server bits per pixel to be 16bpp or even 8bpp.
  (reduces amount of data needed to be polled, compressed, and sent)
* Use a smaller desktop size (e.g. 1024x768 instead of 1280x1024)
* Make sure the desktop background is a solid color (the background
  is resent every time it is re-exposed.) Consider using the -solid
  [color] option to try to do this automatically.
* Configure your window manager or desktop "theme" to not use fancy
  images, shading, and gradients for the window decorations, etc.
  Disable window animations, etc. Maybe your desktop has a "low
  bandwidth" theme you can easily switch into and out of. Also in
  Firefox disable eye-candy, e.g.: Edit -> Preferences -> Advanced
  -> Use Smooth Scrolling (deselect it.)
* Avoid small scrolls of large windows using the Arrow keys or
  scrollbar. Try to use PageUp/PageDown instead. (not so much of a
  problem in x11vnc 0.7.2 if -scrollcopyrect is active and detecting
  scrolls for the application.)
* If the -wireframe option is not available (earlier than x11vnc
  0.7.2 or you have disabled it via -nowireframe) then Disable
  Opaque Moves and Resizes in the window manager/desktop.
* However if -wireframe is active (on by default in x11vnc 0.7.2)
  then you should Enable Opaque Moves and Resizes in the window
  manager! This seems counter-intuitive, but because x11vnc detects
  the move/resize events early there is a huge speedup over a slow
  link when Opaque Moves and Resizes are enabled. (e.g. CopyRect
  encoding will be used.)
* Turn off Anti-aliased fonts on your system, web browser, terminal
  windows, etc. AA fonts do not compress as well as traditional
  fonts (sometimes 10X less.)
* On Firefox/Mozilla (and anything else) turn off "Smooth Scroll"
  animations. In Firefox put in the URL "about:config" and set
  general.smoothScroll to false.
* On Xorg/XFree86 turn on the Shadow Framebuffer to speed up
  reading. (Option "ShadowFB" "true" in the Device section of
  /etc/X11/XF86Config) This disables 2D acceleration on the physical
  display and so may not be worth it (if you play games, etc), but
  could be of use in some situations. Note: If the network link is
  very slow, this speedup may not be noticed.

VNC viewer parameters:

* Use a TightVNC enabled viewer! (Actually, RealVNC 4.x viewer with
  ZRLE encoding is not too bad either; some claim it is faster.)
* Make sure the tight (or zrle) encoding is being used (look at
  vncviewer and x11vnc outputs)
* Request 8 bits per pixel using -bgr233 (up to 4X speedup over
  depth 24 TrueColor (32bpp), but colors will be off)
* RealVNC 4.x viewer has some extremely low color modes (only 64 and
  even 8 colors.) SSVNC does too. The colors are poor, but it is
  usually noticeably faster than bgr233 (256 colors.)
* Try increasing the TightVNC -compresslevel (compresses more on
  server side before sending, but uses more CPU)
* Try reducing the TightVNC -quality (increases JPEG compression,
  but is lossy with painting artifacts)
* Try other VNC encodings via -encodings (tight may be the fastest,
  but you should compare it to zrle and maybe some of the others)
* On the machine where vncviewer is run, make sure Backing Store is
  enabled (Xorg/XFree86 disables it by default causing re-exposures
  of vncviewer to be very slow) Option "backingstore" in config
  file.

x11vnc parameters:

* Make sure the -wireframe option is active (it should be on by
  default) and you have Opaque Moves/Resizes Enabled in the window
  manager.
* Make sure the -scrollcopyrect option is active (it should be on by
  default.) This detects scrolls in many (but not all) applications
  an applies the CopyRect encoding for a big speedup.
* Enforce a solid background when VNC viewers are connected via
  -solid
* Try x11vnc's client-side caching client-side caching scheme:
  -ncache
* Specify -speeds modem to force the wireframe and scrollcopyrect
  heuristic parameters (and any future ones) to those of a dialup
  modem connection (or supply the rd,bw,lat numerical values that
  characterize your link.)
* If wireframe and scrollcopyrect aren't working, try using the more
  drastic -nodragging (no screen updates when dragging mouse, but
  sometimes you miss visual feedback)
* Set -fs 1.0 (disables fullscreen updates)
* Try increasing -wait or -defer (reduces the maximum "frame rate",
  but won't help much for large screen changes)
* Try the -progressive pixelheight mode with the block pixelheight
  100 or so (delays sending vertical blocks since they may change
  while viewer is receiving earlier ones)
* If you just want to watch one (simple) window use -id or -appshare
  (cuts down extraneous polling and updates, but can be buggy or
  insufficient)
* Set -nosel (disables all clipboard selection exchange)
* Use -nocursor and -nocursorpos (repainting the remote cursor
  position and shape takes resources and round trips)
* On very slow links (e.g. <= 28.8) you may need to increase the
  -readtimeout n setting if it sometimes takes more than 20sec to
  paint the full screen, etc.
* Do not use -fixscreen to automatically refresh the whole screen,
  tap three Alt_L's then the screen has painting errors (rare
  problem.)


Example for the KDE desktop:

Launch the "KDE Control Center" utility. Sometimes this is called
"Personal Settings".

Select "Desktop".

Then Select "Window Behavior". In the "Moving" Tab set these:

* YES - Display content in moving windows
* YES - Display content in resizing windows
* NO   - Display window geometry when moving or resizing
* NO   - Animate minimize and restore

In the "Translucency" Tab set:

* NO   - Use translucency/shadows

Next hit "Back" and then select "Panels".

In the "Appearance" Tab set:

* NO   - Enable icon mouseover effects
* NO   - Enable transparency

Now go all the way back up to the top and Select "Appearance &
Themes".

Select "Background" and set:

* YES - No picture
* Colors: Single Color

Select "Fonts" and disable anti-aliased fonts if you are bold enough.

Select "Launch Feedback" and set:

* Busy Cursor: No Busy Cursor
* NO   - Enable taskbar notification

Select "Screen Saver" and set:

* Screen Saver: Blank Screen

Select "Style" and in the "Effects" Tab set:

* NO   - Enable GUI effects


Example for the GNOME desktop:

* TBD.


### Q-74: Does x11vnc support the X DAMAGE Xserver extension to find modified regions of the screen quickly and efficiently?

Yes, as of Mar/2005 x11vnc will use the X DAMAGE extension by default
if it is available on the display. This requires libXdamage to be
available in the build environment as well (recent Linux distros and
Solaris 10 have it.)

The DAMAGE extension enables the X server to report changed regions of
the screen back to x11vnc. So x11vnc doesn't have to guess where the
changes are (by polling every pixel of the entire screen every 2-4
seconds.) The use of X DAMAGE dramatically reduces the load when the
screen is not changing very much (i.e. most of the time.) It also
noticeably improves updates, especially for very small changed areas
(e.g. clock ticking, cursor flashing, typing, etc.)

Note that the DAMAGE extension does not speed up the actual reading of
pixels from the video card framebuffer memory, by, say, mirroring them
in main memory. So reading the fb is still painfully slow (e.g.
5MB/sec), and so even using X DAMAGE when large changes occur on the
screen the bulk of the time is still spent retrieving them. Not ideal,
but use of the ShadowFB XFree86/Xorg option speeds up the reading
considerably (at the cost of h/w acceleration.)

Unfortunately the current Xorg DAMAGE extension implementation can at
times be overly conservative and report very large rectangles as
"damaged" even though only a small portion of the pixels have actually
been modified. This behavior is often the fault of the window manager
(e.g. it redraws the entire, unseen, frame window underneath the
application window when it gains focus), or the application itself
(e.g. does large, unnecessary repaints.)

To work around this deficiency, x11vnc currently only trusts small
DAMAGE rectangles to contain real damage. The larger rectangles are
only used as hints to focus the traditional scanline polling (i.e. if
a scanline doesn't intersect a recent DAMAGE rectangle, the scan is
skipped.) You can use the "-xd_area A" option to adjust the size of
the trusted DAMAGE rectangles. The default is 20000 pixels (e.g. a
140x140 square, etc.) Use "-xd_area 0" to disable the cutoff and trust
all DAMAGE rectangles.

The option "-xd_mem f" may also be of use in tuning the algorithm. To
disable using DAMAGE entirely use "-noxdamage".


### Q-75: My OpenGL application shows no screen updates unless I supply the -noxdamage option to x11vnc.

One user reports in his environment (MythTV using the NVIDIA OpenGL
drivers) he gets no updates after the initial screen is drawn unless
he uses the "-noxdamage" option.

This seems to be a bug in the X DAMAGE implementation of that driver.
You may have to use -noxdamage as well. A way to autodetect this will
be tried, probably the best it will do is automatically stop using X
DAMAGE.

A developer for MiniMyth reports that the 'alphapulse' tag of the
theme G.A.N.T. can also cause problems, and should be avoided when
using VNC.

Update: see this FAQ too.


### Q-76: When I drag windows around with the mouse or scroll up and down things really bog down (unless I do the drag in a single, quick motion.) Is there anything to do to improve things?

This problem is primarily due to slow hardware read rates from video
cards: as you scroll or move a large window around the screen changes
are much too rapid for x11vnc to keep up them (it can usually only
read the video card at about 5-10 MB/sec, so it can take a good
fraction of a second to read the changes induce from moving a large
window, if this to be done a number of times in succession the window
or scroll appears to "lurch" forward.) See the description in the
-pointer_mode option for more info. The next bottleneck is compressing
all of these changes and sending them out to connected viewers,
however the VNC protocol is pretty much self-adapting with respect to
that (updates are only packaged and sent when viewers ask for them.)

As of Jan/2004 there are some improvements to libvncserver. The
default should now be much better than before and dragging small
windows around should no longer be a huge pain. If for some reason
these changes make matters worse, you can go back to the old way via
the "-pointer_mode 1" option.

Also added was the -nodragging option that disables all screen updates
while dragging with the mouse (i.e. mouse motion with a button held
down.) This gives the snappiest response, but might be undesired in
some circumstances when you want to see the visual feedback while
dragging (e.g. menu traversal or text selection.)

As of Dec/2004 the -pointer_mode n option was introduced. n=1 is the
original mode, n=2 an improvement, etc.. See the -pointer_mode n help
for more info.

Also, in some circumstances the -threads option can improve response
considerably. Be forewarned that if more than one vncviewer is
connected at the same time then libvncserver may not be thread safe
(try to get the viewers to use different VNC encodings, e.g. tight and
ZRLE.) This option can be unstable and so as of Feb/2008 it is
disabled by default. Set env. X11VNC_THREADED=1 to re-enable.

As of Apr/2005 two new options (see the wireframe FAQ and
scrollcopyrect FAQ below) provide schemes to sweep this problem under
the rug for window moves or resizes and for some (but not all) window
scrolls. These are the preferred way of avoiding the "lurching"
problem, contact me if they are not working. Note on SuSE and some
other distros the RECORD X extension used by scrollcopyrect is not
enabled by default, turn it on in xorg.conf:

```
Section "Module"
        ...
        Load  "record"
        ...
EndSection
```


### Q-77: Why not do something like wireframe animations to avoid the windows "lurching" when being moved or resized?

Nice idea for a hack! As of Apr/2005 x11vnc by default will apply
heuristics to try to guess if a window is being (opaquely) moved or
resized. If such a change is detected framebuffer polling and updates
will be suspended and only an animated "wireframe" (a rectangle
outline drawn where the moved/resized window would be) is shown. When
the window move/resize stops, it returns to normal processing: you
should only see the window appear in the new position. This spares you
from interacting with a "lurching" window between all of the
intermediate steps. BTW the lurching is due to slow video card read
rates (see here too.) A displacement, even a small one, of a large
window requires a non-negligible amount of time, a good fraction of a
second, to read in from the hardware framebuffer.

Note that Opaque Moves/Resizes must be Enabled by your window manager
for -wireframe to do any good.

The mode is currently on by default because most people are afflicted
with the problem. It can be disabled with the -nowireframe option (aka
-nowf.) Why might one want to turn off the wireframing? Since x11vnc
is merely guessing when windows are being moved/resized, it may guess
poorly for your window-manager or desktop, or even for the way you
move the pointer. If your window-manager or desktop already does its
own wireframing then this mode is a waste of time and could do the
wrong thing occasionally. There may be other reasons the new mode
feels unnatural. If you have very expensive video hardware (SGI, well
now even proprietary Xorg drivers are fast at reading) or are using an
in-RAM video framebuffer (SunRay, ShadowFB, Xvfb), the read rate from
that framebuffer may be very fast (100's of MB/sec) and so you don't
really see much lurching (at least over a fast LAN): opaque moves look
smooth in x11vnc. Note: ShadowFB is often turned on when you are using
the vesafb or fbdev XFree86 video driver instead of a native one so
you might be using it already and not know.

The heuristics used to guess window motion or resizing are simple, but
are not fool proof: x11vnc is sometimes tricked and so you'll
occasionally see the lurching opaque move and rarely something even
worse.

First it assumes that the move/resize will occur with a mouse button
pressed, held down and dragged (of course this is only mostly true.)
Next it will only consider a window for wireframing if the mouse
pointer is initially "close enough" to the edges of the window frame,
e.g. you have grabbed the title bar or a resizer edge (this
requirement can be disabled and it also not applied if a modifier key,
e.g. Alt, is pressed.) If these are true, it will wait an amount of
time to see if the window starts moving or resizing. If it does, it
starts drawing the wireframe "outline" of where the window would be.
When the mouse button is released, or a timeout occurs, it goes back
to the standard mode to allow the actual framebuffer changes to
propagate to the viewers.

These parameters can be tweaked:

* Color/Shade of the wireframe.
* Linewidth of the outline frame.
* Cutoff size of windows to not apply wireframing to.
* Cutoffs for closeness to Top, Bottom, Left, and Right edges of
  window.
* Modifier keys to enable interior window grabbing.
* Maximum time to wait for dragging pointer events.
* Maximum time to wait for the window to start moving/resizing.
* Maximum time to show a wireframe animation.
* Minimum time between sending wireframe outlines.

See the "-wireframe tweaks" option for more details. On a slow link,
e.g. dialup modem, the parameters may be automatically adjusted for
better response.


CopyRect encoding:  In addition to the above there is the
"-wirecopyrect mode" option. It is also on by default. This instructs
x11vnc to not only show the wireframe animation, but to also instruct
all connected VNC viewers to locally translate the window image data
from the original position to the new position on the screen when the
animation is done. This speedup is the VNC CopyRect encoding: the
framebuffer update doesn't need to send the actual new image data.
This is nice in general, and very convenient over a slow link, but
since it is based on heuristics you may need to disable it with the
-nowirecopyrect option (aka -nowcr) if it works incorrectly or
unnaturally for you.

The -wirecopyrect modes are: "never" (same as -nowirecopyrect); "top",
only apply the CopyRect if the window is appears to be on the top of
the window stack and is not obstructed by other windows; and "always"
to always try to apply the CopyRect (obstructed regions are usually
clipped off and not translated.)

Note that some desktops (KDE and xfce) appear to mess with the window
stacking in ways that are not yet clear. In these cases x11vnc works
around the problem by applying the CopyRect even if obscuring windows'
data is translated! Use -nowirecopyrect if this yields undesirable
effects for your desktop.

Also, the CopyRect encoding may give incorrect results under -scale
(depending on the scale factor the CopyRect operation is often only
approximate: the correctly scaled framebuffer will be slightly
different from the translated one.) x11vnc will try to push a
"cleanup" update after the CopyRect if -scale is in effect. Use
-nowirecopyrect if this or other painting errors are unacceptable.


### Q-78: Can x11vnc try to apply heuristics to detect when a window is scrolling its contents and use the CopyRect encoding for a speedup?

Another nice idea for a hack! As of May/2005 x11vnc will by default
apply heuristics to try to detect if the window that has the input
focus is scrolling its contents (but only when x11vnc is feeding user
input, keystroke or pointer, to the X server.) So, when detected,
scrolls induced by dragging on a scrollbar or by typing (e.g. Up or
Down arrows, hitting Return in a terminal window, etc), will show up
much more quickly than via the standard x11vnc screen polling update
mechanism.

There will be a speedup for both slow and fast links to viewers. For
slow links the speedup is mostly due to the CopyRect encoding not
requiring the image data to be transmitted over the network. For fast
links the speedup is primarily due to x11vnc not having to read the
scrolled framebuffer data from the X server (recall that reading from
the hardware framebuffer is slow.)

To do this x11vnc uses the RECORD X extension to snoop the X11
protocol between the X client with the focus window and the X server.
This extension is usually present on most X servers (but SuSE disables
it for some reason.) On XFree86/Xorg it can be enabled via Load
"record" in the Module section of the config file if it isn't already:

```
Section "Module"
        ...
        Load  "record"
        ...
EndSection
```

Currently the RECORD extension is used as little as possible so as to
not slow down regular use. Only simple heuristics are applied to
detect XCopyArea and XConfigureWindow calls from the application.
These catch a lot of scrolls, e.g. in mozilla/firefox and in terminal
windows like gnome-terminal and xterm. Unfortunately the toolkits KDE
applications use make scroll detection less effective (only rarely are
they detected: i.e. Konqueror and Konsole don't work.) An interesting
project, that may be the direction x11vnc takes, is to record all of
the X11 protocol from all clients and try to "tee" the stream into a
modified Xvfb watching for CopyRect and other VNC speedups. A
potential issue is the RECORD stream is delayed from actual view on
the X server display: if one falls too far behind it could become a
mess...

The initial implementation of -scrollcopyrect option is useful in that
it detects many scrolls and thus gives a much nicer working
environment (especially when combined with the -wireframe
-wirecopyrect options, which are also on by default; and if you are
willing to enable the ShadowFB things are very fast.) The fact that
there aren't long delays or lurches during scrolling is the primary
improvement.

But there are some drawbacks:

* Not all scrolls are detected. Some apps scroll windows in ways
  that cannot currently be detected, and other times x11vnc "misses"
  the scroll due to timeouts, etc. Sometimes it is more distracting
  that a speedup occasionally doesn't work as opposed to being
  consistently slow!
* For rapid scrolling (i.e. sequence of many scrolls over a short
  period) there can be painting errors (tearing, bunching up, etc.)
  during the scroll. These will repair themselves after the scroll
  is over, but when they are severe it can be distracting. Try to
  think of the approximate window contents as a quicker and more
  useful "animation" compared to the slower polling scheme...
* Scrolling inside shells in terminal windows (gnome-terminal,
  xterm), can lead to odd painting errors. This is because x11vnc
  did not have time to detect a screen change just before the scroll
  (most common is the terminal undraws the block cursor before
  scrolling the text up: in the viewer you temporarily see multiple
  block cursors.) Another issue is with things like more(1): scroll
  detection for 5-6 lines happens nicely, but then it can't keep up
  and so there is a long pause for the standard polling method to
  deliver the remaining updates.
* More rarely sometimes painting errors are not repaired after the
  scroll is over. This may be a bug in x11vnc or libvncserver, or it
  may be an inescapable fact of the CopyRect encoding and the delay
  between RECORD callbacks and what is actually on the X display.
  One can tap the Alt_L key (Left "Alt" key) 3 times in a row to
  signal x11vnc to refresh the screen to all viewers. Your
  VNC-viewer may have its own screen refresh hot-key or button. See
  also: -fixscreen
* Some applications, notably OpenOffice, do XCopyArea scrolls in
  weird ways that assume ancestor window clipping is taking place.
  See the -scr_skip option for ways to tweak this on a
  per-application basis.
* Selecting text while dragging the mouse may be slower, especially
  if the Button-down event happens near the window's edge. This is
  because the scrollcopyrect scheme is watching for scrolls via
  RECORD and has to wait for a timeout to occur before it does the
  update.
* For reasons not yet understood the RECORD extension can stop
  responding (and hence scrolls are missed.) As a workaround x11vnc
  attempts to reset the RECORD connection every 60 seconds or so.
  Another workaround is to type 4 Super_L (Left Super/Windows-Flag
  key) in a row to reset RECORD. Work is in progress to try to fix
  this bug.
* Sometimes you need to "retrain" x11vnc for a certain window
  because it fails to detect scrolls in it. Sometimes clicking
  inside the application window or selecting some text in it to
  force the focus helps.
* When using the -scale option there will be a quick CopyRect
  scroll, but it needs to be followed by a slower "cleanup" update.
  This is because for a fixed finite screen resolution (e.g. 75 dpi)
  scaling and copyrect-ing are not exactly independent. Scaling
  involves a blending of nearby pixels and if you translate a pixel
  the neighbor pixel weighting may be different. So you have to wait
  a bit for the cleanup update to finish. On slow links x11vnc may
  automatically decide to not detect scrolls when -scale is in
  effect. In general it will also try to defer the cleanup update if
  possible.

If you find the -scrollcopyrect behavior too approximate or
distracting you can go back to the standard polling-only update method
with the -noscrollcopyrect (or -noscr for short.) If you find some
extremely bad and repeatable behavior for -scrollcopyrect please
report a bug.

Alternatively, as with -wireframe, there are many tuning parameters to
try to improve the situation. You can also access these parameters
inside the gui under "Tuning". These parameters can be tweaked:

* The minimum pixel area of a rectangle to be watched for scrolls.
* A list if application names to skip scroll detection.
* Which keystrokes should trigger scroll detection.
* Which applications should have a "terminal" tweak applied to them.
* When repeating keys (e.g. Up arrow) should be discarded to
  preserve a scroll.
* Cutoffs for closeness to Top, Bottom, Left, and Right edges of
  window for mouse induced scrolls.
* Set timeout parameters for keystroke induced scrolls.
* Set timeout parameters for mouse pointer induced scrolls.
* Have the full screen be periodically refreshed to fix painting
  errors.


### Q-79: Can x11vnc do client-side caching of pixel data? I.e. so when that pixel data is needed again it does not have to be retransmitted over the network.

As of Dec/2006 in the 0.9 development tarball there is an experimental
client-side caching implementation enabled by the "-ncache n" option.
In fact, during the test period it was on by default with n set to 10.
To disable it use "-noncache".

It is a simple scheme where a (very large) lower portion of the
framebuffer (i.e. starting just below the user's actual desktop
display) is used for storing pixel data. CopyRect; a fast, essentially
local viewer-side VNC encoding; is used to swap the pixel data in and
out of the actual display area. It gives an excellent speedup for
iconifying/deiconifying and moving windows and re-posting of menus
(often it doesn't feel like VNC at all: there is no delay waiting for
the pixel data to fill in.)

This scheme is nice because it does all of this within the existing
VNC protocol, and so it works with all VNC viewers.

A challenge to doing more sophisticated (e.g. compressed and/or
shared) client-side caching is that one needs to extend the VNC
protocol, modify a viewer and then also convince users to adopt your
modified VNC Viewer (or get the new features to be folded into the
main VNC viewers, patches accepted, etc... likely takes many years
before they might be deployed in the field.) So it is convenient that
the "-ncache n" works with any unaltered VNC viewer.

A drawback of the "-ncache n" method is that in the VNC Viewer you can
scroll down and actually see the cached pixel data. So it looks like
there is a bug: you can scroll down in your viewer and see a strange
"history" of windows on your desktop. This is working as intended. One
will need to try to adjust the size of his VNC Viewer window so the
cache area cannot be seen. SSVNC (see below) can do this
automatically.

At some point LibVNCServer may implement a "rfbFBCrop" pseudoencoding
that viewers can use to learn which portion of the framebuffer to
actually show to the users (with the hidden part used for caching, or
perhaps something else, maybe double buffering or other offscreen
rendering...)

The Enhanced TightVNC Viewer (SSVNC) Unix viewer has a nice -ycrop
option to help hide the pixel cache area from view. It will turn on
automatically if the framebuffer appears to be very tall (height more
than twice the width), or you can supply the actual value for the
height. If the screen is resized by scaling, etc, the ycrop value is
scaled as well. In fullscreen mode you cannot scroll past the end of
the actual screen, and in non-fullscreen mode the window manager frame
is adjusted to fit the actual display (so you don't see the pixel
cache region) and the scrollbars are very thin to avoid distraction
and trouble fitting inside your display. Use the "-sbwidth n" viewer
option to make the scrollbars thicker if you like.

Another drawback of the scheme is that it is VERY memory intensive,
the n in "-ncache n" is the factor of increase over the base
framebuffer size to use for caching. It is an even integer and should
be fairly large, 6-12, to achieve good response. This usually requires
about 50-100MB of additional RAM on both the client and server sides.
For example with n=6 a 1280x1024 display will use a framebuffer that
is 1280x7168: everything below row 1024 is the pixel buffer cache. If
you are running on low memory machines or memory is tight because of
other running applications you should not use -ncache.

The reason for so much memory is because the pixel data is not
compressed and so the whole window to be saved must be stored
"offscreen". E.g. for a large web browser window this can be nearly 1
million pixels, and that is only for a single window! One typically
wants to cycle between 5-10 large active windows. Also because both
backing-store (the window's actual contents) and save-unders (the
pixels covered up by the window) are cached offscreen that introduces
an additional factor of 2 in memory use.

However, even in the smallest usage mode with n equal 2 and
-ncache_no_rootpixmap set (this requires only 2X additional
framebuffer memory) there is still a noticable improvement for many
activities, although it is not as dramatic as with, say n equal 12 and
rootpixmap (desktop background) caching enabled.

The large memory consumption of the current implementation can be
thought of as a tradeoff to providing caching and being compatible
with all VNC viewers and also ease of implementing. Hopefully it can
be tuned to use less, or the VNC community will extend the protocol to
allow caching and replaying of compressed blobs of data.

Another option to experiment with is "-ncache_cr". By specifying it,
x11vnc will try to do smooth opaque window moves instead of its
wireframe. This can give a very nice effect (note: on Unix the realvnc
viewer seems to be smoother than the tightvnc viewer), but can lead to
some painting problems, and can be jerky in some circumstances.

Surprisingly, for very slow connections, e.g. modem, the -ncache_cr
option can actually improve window drags. This is probably because no
pixel data (only CopyRect instructions) are sent when dragging a
window. Normally, the wireframe must be sent and this involves
compressing and sending the lines that give rise to the moving box
effect (note that real framebuffer data is sent to "erase" the white
lines of the box.)

If you experience painting errors you can can tap the Alt_L key (Left
"Alt" key) 3 times in a row to signal x11vnc to refresh the screen to
all viewers. You may also need to iconify and then deiconify any
damaged windows to correct their cache data as well. Note that if you
change color viewer depth (e.g. 8bpp to full color) dynamically that
will usually lead to the entire extended framebuffer being resent
which can take a long time over very slow links: it may be better to
reconnect and reset the format right after doing so. x11vnc will try
to detect the format change and clear (make completely black) the
cache region.

Gotcha for older Unix VNC Viewers: The older Unix VNC viewers (e.g.
current TightVNC Unix Viewer) require X server backingstore to keep
off-viewer screen data local. If the viewer-side X server has
backingstore disabled (sadly, currently the default on Linux, etc),
then to get the offscreen pixels the viewer has to ask for a refresh
over the network, thereby defeating the caching. Use something like
this in your viewer-side /etc/X11/xorg.conf file (or otherwise get
your viewer-side system to do it)

```
Section "Device"
        ...
        Option  "backingstore"
        ...
EndSection
```

No problems like this have been observed with Windows VNC Viewers:
they all seem to keep their entire framebuffer in local memory.

Gotcha for KDE krdc VNC Viewer: One user found that KDE's krdc viewer
has some sort of hardwired limit on the maximum size of the
framebuffer (64MB?). It fails quickly saying "The connection to the
host has been interrupted." The workaround for his 1280x1024
x11vnc-side display was to run with "-ncache 10", i.e. a smaller value
to be under the krdc threshold.

Although this scheme is not as quick (nor as compressed) as
nx/nomachine, say, it does provide a good step in the direction of
improving VNC performance by client side caching.


### Q-80: Does x11vnc support TurboVNC?

As of Feb/2009 (development tarball) there is an experimental kludge
to let you build x11vnc using TurboVNC's modified TightVNC encoding.
TurboVNC is part of the VirtualGL project. It does two main things to
speed up the TightVNC encoding:

* It eliminates bottlenecks, overheads, wait-times in the TightVNC
  encoding implementation and instead only worries about sending
  very well (and quickly) compressed JPEG data.
* A fast proprietary JPEG implemention is used (Intel IPP on x86)
  instead of the usual libjpeg implementation. TurboJPEG is an
  interface library, libturbojpeg, provided by the project that
  achieves this.

TurboVNC works very well over LAN and evidently fast Broadband too.
When using it with x11vnc in such a situation you may want to dial
down the delays, e.g. "-wait 5" and "-defer 5" (or even a smaller
setting) to poll and pump things out more quickly.

See the instructions in "x11vnc/misc/turbovnc/README" for how to build
x11vnc with TurboVNC support. You will also need to download the
TurboJPEG software.

   In brief, the steps look like this:

```
cd x11vnc-x.y.z/x11vnc/misc/turbovnc
./apply_turbovnc
cd ../../..
env LDFLAGS='-L/DIR -Xlinker --rpath=/DIR' ./configure
make AM_LDFLAGS='-lturbojpeg'
```

where you replace "/DIR" with the directory containing libturbojpeg.so
you downloaded separately. If it works out well enough TurboVNC
support will be integrated into x11vnc and more of its tuning features
will be implemented. Support for TurboVNC in SSVNC viewer has been
added as an experiment as well. If you try either one, let us know how
it went.

There also may be some Linux.i686 and Darwin.i386 x11vnc binaries with
TurboVNC support in the misc. bins directory. For other platforms you
will need to compile yourself.

On relatively cheap and old hardware (Althon64 X2 5000+ / GeForce
6200) x11vnc and SSVNC, both TurboVNC enabled, were able to sustain
13.5 frames/sec (fps) and 15 Megapixels/sec using the VirtualGL
supplied OpenGL benchmark program glxspheres. VirtualGL on higher-end
hardware can sustain 20-30 fps with the glxspheres benchmark.

Potential Slowdown: As we describe elsewhere, unless you use x11vnc
with an X server using, say, NVidia proprietary drivers (or a virtual
X server like Xvfb or Xdummy, or in ShadowFB mode), then the read rate
from the graphics card can be rather slow (e.g. 10 MB/sec) and becomes
the bottleneck when using x11vnc over fast networks. Note that all of
Xorg's drivers currently (2009) have slow read rates (only proprietary
drivers appear to have optimized reads.)

So under these (more or less typical) conditions, the speed
improvement provided by TurboVNC may only be marginal. Look for this
output to see your read rate:

```
28/02/2009 11:11:07 Autoprobing TCP port
28/02/2009 11:11:07 Autoprobing selected port 5900
28/02/2009 11:11:08 fb read rate: 10 MB/sec
28/02/2009 11:11:08 screen setup finished.
```

A rate of 10 MB/sec means a 1280x1024x24 screen takes 0.5 seconds to
read in. TurboVNC compresses that to JPEG in a much shorter time. On
the other hand, an NVidia driver may have a read rate of 250 MB/sec
and so only takes 0.02 seconds to read the entire screen in.


## Mouse Cursor Shapes

### Q-81: Why isn't the mouse cursor shape (the little icon shape where the mouse pointer is) correct as I move from window to window?

On X servers supporting XFIXES or Solaris/IRIX Overlay extensions it
is possible for x11vnc to do this correctly. See a few paragraphs down
for the answer.

Historically, the X11 mouse cursor shape (i.e. little picture: an
arrow, X, I-beam, resizer, etc) is one of the few WRITE-only objects
in X11. That is, an application can tell the X server what the cursor
shape should be when the pointer is in a given window, but a program
(like x11vnc) unfortunately cannot read this information. I believe
this is because the cursor shape is often downloaded to the graphics
hardware (video card), but I could be mistaken.

A simple kludge is provided by the "-cursor X" option that changes the
cursor when the mouse is on the root background (or any window has the
same cursor as the root background.) Note that desktops like GNOME or
KDE often cover up the root background, so this won't work for those
cases. Also see the "-cursor some" option for additional kludges.

Note that as of Aug/2004 on Solaris using the SUN_OVL overlay
extension and IRIX, x11vnc can show the correct mouse cursor when the
-overlay option is supplied. See this FAQ for more info.

Also as of Dec/2004 XFIXES X extension support has been added to allow
exact extraction of the mouse cursor shape. XFIXES fixes the problem
of the cursor-shape being write-only: x11vnc can now query the X
server for the current shape and send it back to the connected
viewers. XFIXES is available on recent Linux Xorg based distros and
Solaris 10.

The only XFIXES issue is the handling of alpha channel transparency in
cursors. If a cursor has any translucency then in general it must be
approximated to opaque RGB values for use in VNC. There are some
situations where the cursor transparency can also handled exactly:
when the VNC Viewer requires the cursor shape be drawn into the VNC
framebuffer or if you apply a patch to your VNC Viewer to extract
hidden alpha channel data under 32bpp. Details can be found here.


### Q-82: When using XFIXES cursorshape mode, some of the cursors look really bad with extra black borders around the cursor and other cruft.  How can I improve their appearance?

This happens for cursors with transparency ("alpha channel"); regular
X cursors (bitmaps) should be correct. Unfortunately x11vnc 0.7 was
released with a very poor algorithm for approximating the
transparency, which led to the ugly black borders.

The problem is as follows: XFIXES allows x11vnc to retrieve the
current X server cursor shape, including the alpha channel for
transparency. For traditional bitmap cursors the alpha value will be 0
for completely transparent pixels and 255 for completely opaque
pixels; whereas for modern, eye-candy cursors an alpha value between 0
and 255 means to blend in the background colors to that degree with
the cursor colors. The pixel color blending formula is something like
this: Red = Red_cursor * a + Red_background * (1 - a), (where here 0
=< a =< 1), with similar for Green and Blue. The VNC protocol does not
currently support an alpha channel in cursors: it only supports
regular X bitmap cursors and Rich Cursors that have RGB (Red, Green,
Blue) color data, but no "A" = alpha data. So in general x11vnc has to
approximate a cursor with transparency to create a Rich Cursor. This
is easier said than done: some cursor themes have cursors with
complicated drop shadows and other forms of translucency.

Anyway, for the x11vnc 0.7.1 release the algorithm for approximating
transparency is much improved and hopefully gives decent cursor shapes
for most cursor themes and you don't have to worry about it.

In case it still looks bad for your cursor theme, there are (of
course!) some tunable parameters. The "-alphacut n" option lets you
set the threshold "n" (between 0 and 255): cursor pixels with alpha
values below n will be considered completely transparent while values
equal to or above n will be completely opaque. The default is 240. The
"-alphafrac f" option tries to correct individual cursors that did not
fare well with the default -alphacut value: if a cursor has less than
fraction f (between 0.0 and 1.0) of its pixels selected by the default
-alphacut, the threshold is lowered until f of its pixels are
selected. The default fraction is 0.33.

Finally, there is an option -alpharemove that is useful for themes
where many cursors are light colored (e.g. "whiteglass".) XFIXES
returns the cursor data with the RGB values pre-multiplied by the
alpha value. If the white cursors look too grey, specify -alpharemove
to brighten them by having x11vnc divide out the alpha value.

One user played with these parameters and reported back:

```
 Of the cursor themes present on my system:

   gentoo and gentoo-blue:   alphacut:192 - noalpharemove

   gentoo-silver:            alphacut:127 and alpharemove

   whiteglass and redglass (presumably also handhelds, which is based
   heavily on redglass) look fine with the apparent default of alphacut:255.
```

### Q-83: In XFIXES mode, are there any hacks to handle cursor transparency ("alpha channel") exactly?

As of Jan/2005 libvncserver has been modified to allow an alpha
channel (i.e. RGBA data) for Rich Cursors. So x11vnc can now send the
alpha channel data to libvncserver. However, this data will only be
used for VNC clients that do not support the CursorShapeUpdates VNC
extension (or have disabled it.) It can be disabled for all clients
with the -nocursorshape x11vnc option. In this case the cursor is
drawn, correctly blended with the background, into the VNC framebuffer
before being sent out to the client. So the alpha blending is done on
the x11vnc side. Use the -noalphablend option to disable this behavior
(always approximate transparent cursors with opaque RGB values.)

The CursorShapeUpdates VNC extension complicates matters because the
cursor shape is sent to the VNC viewers supporting it, and the viewers
draw the cursor locally. This improves response over slow links. Alpha
channel data for these locally drawn cursors is not supported by the
VNC protocol.

However, in the libvncserver CVS there is a patch to the TightVNC
viewer to make this work for CursorShapeUpdates under some
circumstances. This hack is outside of the VNC protocol. It requires
the screens on both sides to be depth 24 at 32bpp (it uses the extra 8
bits to secretly hide the cursor alpha channel data.) Not only does it
require depth 24 at 32bpp, but it also currently requires the client
and server to be of the same endianness (otherwise the hidden alpha
data gets reset to zero by a libvncserver translation function; we can
fix this at some point if there is interest.) The patch is for the
TightVNC 1.3dev5 Unix vncviewer and it enables the TightVNC viewer to
do the cursor alpha blending locally. The patch code should give an
example on how to change the Windows TightVNC viewer to achieve the
same thing (send me the patch if you get that working.)

This patch is applied to the Enhanced TightVNC Viewer (SSVNC) package
we provide.


## Mouse Pointer

### Q-84: Why does the mouse arrow just stay in one corner in my vncviewer, whereas my cursor (that does move) is just a dot?

This default takes advantage of a tightvnc extension
(CursorShapeUpdates) that allows specifying a cursor image shape for
the local VNC viewer. You may disable it with the -nocursor option to
x11vnc if your viewer does not have this extension.

Note: as of Aug/2004 this should be fixed: the default for
non-tightvnc viewers (or ones that do not support CursorShapeUpdates)
will be to draw the moving cursor into the x11vnc framebuffer. This
can also be disabled via -nocursor.


### Q-85: Can I take advantage of the TightVNC extension to the VNC protocol where Cursor Positions Updates are sent back to all connected clients (i.e. passive viewers can see the mouse cursor being moved around by another viewer)?

Use the -cursorpos option when starting x11vnc. A VNC viewer must
support the Cursor Positions Updates for the user to see the mouse
motions (the TightVNC viewers support this.) As of Aug/2004 -cursorpos
is the default. See also -nocursorpos and -nocursorshape.


### Q-86: Is it possible to swap the mouse buttons (e.g. left-handed operation), or arbitrarily remap them? How about mapping button clicks to keystrokes, e.g. to partially emulate Mouse wheel scrolling?

You can remap the mouse buttons via something like: -buttonmap 13-31
(or perhaps 12-21.) Also, note that xmodmap(1) lets you directly
adjust the X server's button mappings, but in some circumstances it
might be more desirable to have x11vnc do it.

One user had an X server with only one mouse button(!) and was able to
map all of the VNC client mouse buttons to it via: -buttonmap 123-111.

Note that the -debug_pointer option prints out much info for every
mouse/pointer event and is handy in solving problems.

To map mouse button clicks to keystrokes you can use the alternate
format where the keystrokes are enclosed between colons like this
:<KeySym>: in place of the mouse button digit. For a sequence of
keysyms separate them with "+" signs. Look in the include file
<X11/keysymdef.h>, or use xev(1), or -debug_keyboard to find the
keysym names. Button clicks can also be included in the sequence via
the fake keysyms Button1, etc.

As an example, suppose the VNC viewer machine has a mouse wheel (these
generate button 4 and 5 events), but the machine that x11vnc is run on
only has the 3 regular buttons. In normal operation x11vnc will
discard the button 4 and 5 events. However, either of the following
button maps could possibly be of use emulating the mouse wheel events
in this case:

```
-buttonmap 12345-123:Prior::Next:
-buttonmap 12345-123:Up+Up+Up::Down+Down+Down:
```

Exactly what keystroke "scrolling" events they should be bound to
depends on one's taste. If this method is too approximate, one could
consider not using -buttonmap but rather configuring the X server to
think it has a mouse with 5 buttons even though the physical mouse
does not. (e.g. 'Option "ZAxisMapping" "4 5"'.)

Note that when a keysym-mapped mouse button is clicked down this
immediately generates the key-press and key-release events (for each
keysym in turn if the mapping has a sequence of keysyms.) When the
mouse button goes back up nothing is generated.

If you include modifier keys like Shift_L instead of key-press
immediately followed by key-release the state of the modifier key is
toggled (however the initial state of the modifier key is ignored.) So
to map the right button to type my name 'Karl Runge' I could use this:

```
-buttonmap 3-:Shift_L+k+Shift_L+a+r+l+space+Shift_L+r+Shift_L+u+n+g+e:
```

(yes, this is getting a little silly.)

BTW, Coming the other way around, if the machine you are sitting at
does not have a mouse wheel, but the remote machine does (or at least
has 5 buttons configured), this key remapping can be useful:

```
-remap Super_R-Button4,Menu-Button5
```

you just tap those two keys to get the mouse wheel scrolls (this is
more useful than the Up and Down arrow keys because a mouse wheel
"click" usually gives a multi-line scroll.)


## Keyboard Issues

### Q-87: How can I get my AltGr and Shift modifiers to work between keyboards for different languages?

The option -modtweak should help here. It is a mode that monitors the
state of the Shift and AltGr Modifiers and tries to deduce the correct
keycode to send, possibly by sending fake modifier key presses and
releases in addition to the actual keystroke.

Update:  As of Jul/2004 -modtweak is now the default (use -nomodtweak
to get the old behavior.) This was done because it was noticed on
newer XFree86 setups even on bland "us" keyboards like "pc104 us"
XFree86 included a "ghost" key with both "<" and ">" it. This key does
not exist on the keyboard (see this FAQ for more info.) Without
-modtweak there was then an ambiguity in the reverse map keysym =>
keycode, making it so the "<" symbol could not be typed.

Also see the FAQ about the -xkb option for a more powerful method of
modifier tweaking for use on X servers with the XKEYBOARD extension.

When trying to resolve keyboard mapping problems, note that the
-debug_keyboard option prints out much info for every keystroke and so
can be useful debugging things.

Note that for some users, the solution is to disable all of the above,
and use -nomodtweak. This is the simplest form of keystroke insertion and
it actually solved the problem. Try it if the other options don't help.


### Q-88: When I try to type a "<" (i.e. less than) instead I get ">" (i.e. greater than)! Strangely, typing ">" works OK!!

Does your keyboard have a single key with both "<" and ">" on it? Even
if it doesn't, your X server may think your keyboard has such a key
(e.g. pc105 in the XF86Config file when it should be something else,
say pc104.)

Short Cut: Try the -xkb or -sloppy_keys options and see if that helps
the situation. The discussion below is a bit outdated (e.g. -modtweak
is now the default) but it is useful reference for various tricks and
so is kept.


The problem here is that on the Xserver where x11vnc is run there are
two keycodes that correspond to the "<" keysym. Run something like
this to see:

```
xmodmap -pk | egrep -i 'KeyCode|less|greater'
There are 4 KeySyms per KeyCode; KeyCodes range from 8 to 255.
    KeyCode     Keysym (Keysym) ...
     59         0x002c (comma)  0x003c (less)
     60         0x002e (period) 0x003e (greater)
     94         0x003c (less)   0x003e (greater)
```

That keycode 94 is the special key with both "<" and ">". When x11vnc
receives the "<" keysym over the wire from the remote VNC client, it
unfortunately maps it to keycode 94 instead of 59, and sends 94 to the
X server. Since Shift is down (i.e. you are Shifting the comma key),
the X server interprets this as Shifted-94, which is ">".

A workaround in the X server configuration is to "deaden" that special
key:

```
xmodmap -e "keycode 94 = "
```

However, one user said he had to do this:

```
xmodmap -e "keycode 94 = 0x002c 0x003c"
```

(If the numerical values are different for your setup, substitute the
ones that correspond to your display. The above xmodmap scheme can
often be used to work around other ambiguous keysym to keycode
mappings.)

Alternatively, here are some x11vnc options to try to work around the
problem:

```
-modtweak
```

and

```
-remap less-comma
```

These are convenient in that they do not modify the actual X server
settings. The former (-modtweak) is a mode that monitors the state of
the Shift and AltGr modifiers and tries to deduce the correct keycode
sequence to send. Since Jul/2004 -modtweak is now the default. The
latter (-remap less-comma) is an immediate remapping of the keysym
less to the keysym comma when it comes in from a client (so when Shift
is down the comma press will yield "<".)

See also the FAQ about the -xkb option as a possible workaround using
the XKEYBOARD extension.

Note that the -debug_keyboard option prints out much info for every
keystroke to aid debugging keyboard problems.


### Q-89: Extra Character Inserted, E.g.: When I try to type a "<" (i.e. less than) instead I get "<," (i.e. an extra comma.)

This is likely because you press "Shift" then "<" but then released
the Shift key before releasing the "<". Because of a keymapping
ambiguity the last event "< up" is interpreted as "," because that key
unshifted is the comma.

This extra character insertion will happen for other combinations of
characters: in general it can happen whenever the Shift key is
released early.

This should not happen in -xkb mode, because it works hard to resolve
the ambiguities. If you do not want to use -xkb, try the option
-sloppy_keys to attempt a similar type of algorithm.

One user had this problem for Italian and German keyboards with the
key containing ":" and "." When he typed ":" he would get an extra "."
inserted after the ":". The solution was -sloppy_keys.


### Q-90: I'm using an "international" keyboard (e.g. German "de", or Danish "dk") and the -modtweak mode works well if the VNC viewer is run on a Unix/Linux machine with a similar keyboard.   But if I run the VNC viewer on Unix/Linux with a different keyboard (e.g. "us") or Windows with any keyboard, I can't type some keys like:   "@", "$", "<", ">", etc. How can I fix this?

The problem with Windows is it does not seem to handle AltGr well. It
seems to fake it up by sending Control_L+Alt_R to applications. The
Windows VNC viewer sends those two down keystrokes out on the wire to
the VNC server, but when the user types the next key to get, e.g., "@"
the Windows VNC viewer sends events bringing the up the
Control_L+Alt_R keys, and then sends the "@" keysym by itself.

The Unix/Linux VNC viewer on a "us" keyboard does a similar thing
since "@" is the Shift of the "2" key. The keysyms Shift and "@" are
sent to the VNC server.

In both cases no AltGr is sent to the VNC server, but we know AltGr is
needed on the physical international keyboard to type a "@".

This all worked fine with x11vnc running with the -modtweak option (it
figures out how to adjust the Modifier keys (Shift or AltGr) to get
the "@".) However it fails under recent versions of XFree86 (and the
X.org fork.) These run the XKEYBOARD extension by default and make
heavy use of it to handle international keyboards.

To make a long story short, on these newer XFree86 setups the
traditional X keymap lookup x11vnc uses is no longer accurate. x11vnc
can't find the keysym "@" anywhere in the keymapping! (even though it
is in the XKEYBOARD extended keymapping.)

How to Solve: As of Jul/2004 x11vnc has two changes:

* -modtweak (tweak Modifier keys) is now the default (use
  -nomodtweak to go back to the old way)
* there is a new option -xkb to use the XKEYBOARD extension API to
  do the Modifier key tweaking.

The -xkb option seems to fix all of the missing keys: "@", "<", ">",
etc.: it is recommended that you try it if you have this sort of
problem. Let us know if there are any remaining problems (see the next
paragraph for some known problems.) If you specify the -debug_keyboard
(aka -dk) option twice you will get a huge amount of keystroke
debugging output (send it along with any problems you report.)

Update: as of Jun/2005 x11vnc will try to automatically enable -xkb if
it appears that would be beneficial (e.g. if it sees any of "@", "<",
">", "[" and similar keys are mapped in a way that needs the -xkb to
access them.) To disable this automatic check use -noxkb.

Known problems:

* One user had to disable a "ghost" Mode_switch key that was causing
  problems under -xkb. His physical AltGr key was bound to
  ISO_Level3_Shift (which seems to be the XKEYBOARD way of doing
  things), while there was a ghost key Mode_switch (which seems to
  be obsolete) in the mapping as well. Both of these keysyms were
  bound to Mod5 and x11vnc was unfortunately choosing Mode_switch.
  From the x11vnc -xkb -dk -dk output it was noted that Mode_switch
  was attached to keycode 93 (no physical key generates this
  keycode) while ISO_Level3_Shift was attached to keycode 113. The
  keycode skipping option was used to disable the ghost key:
  -skip_keycodes 93
* In implementing -xkb we noticed that some characters were still
  not getting through, e.g. "~" and "^". This is not really an
  XKEYBOARD problem. What was happening was the VNC viewer was
  sending the keysyms asciitilde and asciicircum to x11vnc, but on
  the X server with the international keyboard those keysyms were
  not mapped to any keys. So x11vnc had to skip them (Note: as of
  May/2005 they are added by default see -add_keysyms below.)
  The way these characters are typically entered on international
  keyboards is by "dead" (aka "mute") keys. E.g. to enter "~" at the
  physical display the keysym dead_tilde is pressed and released
  (this usually involves holding AltGr down while another key is
  pressed) and then space is pressed. (this can also be used get
  characters with the "~" symbol on top, e.g. "\E3" by typing "a"
  instead of space.)
  What to do? In general the VNC protocol has not really solved this
  problem: what should be done if the VNC viewer sends a keysym not
  recognized by the VNC server side? Workarounds can possibly be
  created using the -remap x11vnc option:

  ```
  -remap asciitilde-dead_tilde,asciicircum-dead_circumflex
  ```

  etc. Use -remap filename if the list is long. Please send us your
  workarounds for this problem on your keyboard. Perhaps we can have
  x11vnc adjust automatically at some point. Also see the
  -add_keysyms option in the next paragraph.
  Update: for convenience "-remap DEAD" does many of these mappings
  at once.
* To complement the above workaround using the -remap, an option
  -add_keysyms was added. This option instructs x11vnc to bind any
  unknown Keysyms coming in from VNC viewers to unused Keycodes in
  the X server. This modifies the global state of the X server. When
  x11vnc exits it removes the extra keymappings it created. Note
  that the -remap mappings are applied first, right when the Keysym
  is received from a VNC viewer, and only after that would
  -add_keysyms, or anything else, come into play.
  Update: -add_keysyms is now on by default. Use -noadd_keysyms to
  disable.


### Q-91: When typing I sometimes get double, triple, or more of my keystrokes repeated. I'm sure I only typed them once, what can I do?

This may be due to an interplay between your X server's key autorepeat
delay and the extra time delays caused by x11vnc processing.

Short answer: disable key autorepeating by running the command "xset r
off" on the Xserver where x11vnc is run (restore via "xset r on") or
use the new (Jul/2004) -norepeat x11vnc option. You will still have
autorepeating because that is taken care of on your VNC viewer side.

Update: as of Dec/2004 -norepeat is now the default. Use -repeat to
disable it.

Details:
suppose you press a key DOWN and it generates changes in large regions
of the screen. The CPU and I/O work x11vnc does for the large screen
change could be longer than your X server's key autorepeat delay.
x11vnc may not get to processing the key UP event until after the
screen work is completed. The X server believes the key has been held
down all this time, and applies its autorepeat rules.

Even without inducing changes in large regions of the screen, this
problem could arise when accessing x11vnc via a dialup modem or
otherwise high latency link (e.g. > 250 ms latency.)

Look at the output of "xset q" for the "auto repeat delay" setting. Is
it low (e.g. < 300 ms)? If you turn off autorepeat completely: "xset r
off", does the problem go away?

The workaround is to manually apply "xset r off" and "xset r on" as
needed, or to use the -norepeat (which has since Dec/2004 been made
the default.) Note that with X server autorepeat turned off the VNC
viewer side of the connection will (nearly always) do its own
autorepeating so there is no big loss here, unless someone is also
working at the physical display and misses his autorepeating.


### Q-92: The x11vnc -norepeat mode is in effect, but I still get repeated keystrokes!!

Are you using x11vnc to log in to an X session via display manager?
(as described in this FAQ) If so, x11vnc is starting before your
session and it disables autorepeat when you connect, but then after
you log in your session startup (GNOME, KDE, ...) could be resetting
the autorepeat to be on. Or it could be something inside your desktop
trying to be helpful that decides to turn it back on.

x11vnc in -norepeat mode will by default reset autorepeat to off 2
times (to help get thru the session startup problem), but it will not
continue to battle with things turning autorepeat back on. It will
also turn autorepeat off whenever it goes from a state of zero clients
to one client. You can adjust the number of resets via "-norepeat N",
or use "-norepeat -1" to have it keep resetting it whenever autorepeat
gets turned back on when clients are connected.

In general you can manually turn autorepeating off by typing "xset r
off", or a using desktop utility/menu, or "x11vnc -R norepeat". If
something in your desktop is automatically turning it back on you
should figure out how to disable that somehow.


### Q-93: After using x11vnc for a while, I find that I cannot type some (or any) characters or my mouse clicks and drags no longer have any effect, or they lead to strange effects. What happened?

Probably a modifier key, e.g. Control or Alt is "stuck" in a pressed
down state.

This happens for VNC in general by the following mechanism. Suppose on
the Viewer side desktop there is some hot-key to switch
desktops/rooms/spaces, etc. E.g. suppose Alt+LeftArrow moves to the
left desktop/room/space. Or suppose an Alt+hotkey combination
iconifies a window. This can leave the Alt key pressed down on the
remote side.

Consider the sequence that happens. The Alt_L key and then the
LeftArrow key go down. Since you are inside the viewer the Alt_L key
press is sent to the other side (x11vnc) and so it is pressed down in
the remote desktop as well. (by "Alt_L" we mean the Alt key on the
left-hand side of the keyboard.) Your local desktop (where the VNC
Viewer is running) then warps to the new desktop/room/space: Leaving
the Alt_L key still pressed down in the remote desktop.

If someone is sitting at the desktop, or when you return in the viewer
it may be very confusing because the Alt_L is still pressed down but
you (or the person sitting at the desktop) do not realize this.
Depending on which remote desktop (x11vnc side) is used, it can act
very strangely.

A quick workaround when you notice this is to press and release all of
the Alt, Shift, Control, Windows-Flag, modifier keys to free the
pressed one. You need to do this for both the left and right Shift,
Alt, Control, etc. keys to be sure.

Note that many VNC Viewers try to guard against this when they are
notified by the window system that the viewer app has "lost focus".
When it receives the "lost focus" event, the viewer sends VNC
Key-Release events for all modifier keys that are currently pressed
down. This does not always work, however, since it depends on how the
desktop manages these "warps". If the viewer is not notified it cannot
know it needs to release the modifiers.

You can also use the -clear_mods option to try to clear all of the
modifier keys at x11vnc startup. You will still have to be careful
that you do not leave the modifier key pressed down during your
session. It is difficult to prevent this problem from occurring (short
of using -remap to prevent sending all of the problem modifier keys,
which would make the destkop pretty unusable.)

During a session these x11vnc remote control commands can also help:

```
x11vnc -R clear_mods
x11vnc -R clear_keys
x11vnc -R clear_locks
x11vnc -R clear_all
```

A similar problem can occur if you accidentally press the Caps_Lock or
Num_Lock down. When these are locked on the remote side it can
sometimes lead to strange desktop behavior (e.g. cannot drag or click
on windows.) As above you may not notice this because the lock isn't
down on the local (Viewer) side. See this FAQ on lock keys problem.
These options may help avoid the problem: -skip_lockkeys and
-capslock. See also -clear_all.


### Q-94: The machine where I run x11vnc has an AltGr key, but the local machine where I run the VNC viewer does not. Is there a way I can map a local unused key to send an AltGr? How about a Compose key as well?

Something like "-remap Super_R-Mode_switch" x11vnc option may work.
Note that Super_R is the "Right Windoze(tm) Flaggie" key; you may want
to choose another. The -debug_keyboard option comes in handy in
finding keysym names (so does xev(1).)

For Compose how about "-remap Menu-Multi_key" (note that Multi_key is
the official name for Compose.) To do both at the same time: "-remap
Super_R-Mode_switch,Menu-Multi_key" or use "-remap filename" to
specify remappings from a file.


### Q-95: I have a Sun machine I run x11vnc on. Its Sun keyboard has just one Alt key labelled "Alt" and two Meta keys labelled with little diamonds. The machine where I run the VNC viewer only has Alt keys.  How can I send a Meta keypress? (e.g. emacs needs this)

Here are a couple ideas. The first one is to simply use xmodmap(1) to
adjust the Sun X server. Perhaps xmodmap -e "keysym Alt_L = Meta_L
Alt_L" will do the trick. (there are other ways to do it, one user
used: xmodmap -e "keycode 26 = Meta_L" for his setup.)

Since xmodmap(1) modifies the X server mappings you may not want to do
this (because it affects local work on that machine.) Something like
the -remap Alt_L-Meta_L to x11vnc may be sufficient for ones needs,
and does not modify the X server environment. Note that you cannot
send Alt_L in this case, maybe -remap Super_L-Meta_L would be a better
choice if the Super_L key is typically unused in Unix.


### Q-96: Running x11vnc on HP-UX I cannot type "#" I just get a "3" instead.

One user reports this problem on HP-UX Rel_B.11.23. The problem was
traced to a strange keyboard mapping for the machine (e.g. xmodmap -pk
output) that looked like:

```
...
039  2                  at                 at               at
...
047  3                  numbersign         numbersign       numbersign
```

and similar triple mappings (with two in the AltGr/Mode_switch group)
of a keysum to a single keycode.

Use the -nomodtweak option as a workaround. You can also use xmodmap
to correct these mappings in the server, e.g.:

```
xmodmap -e "keycode 47 = 3 numbersign"
```

Also, as of Feb/2007, set the environment variable MODTWEAK_LOWEST=1
(either in your shell or via "-env MODTWEAK_LOWEST=1" option) to
handle these mappings better.


### Q-97: Can I map a keystroke to a mouse button click on the remote machine?

This can be done directly in some X servers using AccessX and
Pointer_EnableKeys, but is a bit awkward. It may be more convenient to
have x11vnc do the remapping. This can be done via the -remap option
using the fake "keysyms" Button1, Button2, etc. as the "to" keys (i.e.
the ones after the "-")

As an example, consider a laptop where the VNC viewer is run that has
a touchpad with only two buttons. It is difficult to do a middle
button "paste" because (using XFree86/Xorg Emulate3Buttons) you have
to click both buttons on the touch pad at the same time. This
remapping:
-remap Super_R-Button2

maps the Super_R "flag" key press to the Button2 click, thereby making
X pasting a bit easier.

Note that once the key goes down, the button down and button up events
are generated immediately on the x11vnc side. When the key is released
(i.e. goes up) no events are generated.

### Q-98: How can I get Caps_Lock to work between my VNC viewer and x11vnc?

This is a little tricky because it is possible to get the Caps_Lock
state out of sync between your viewer-side machine and the x11vnc-side
X server. For best results, we recommend not ever letting the
Caps_Lock keypresses be processed by x11vnc. That way when you press
Caps_Lock in the viewer your local machine goes into the Caps_Lock on
state and sends keysym "A" say when you press "a". x11vnc will then
fake things up so that Shift is held down to generate "A". The
-skip_lockkeys option should help to accomplish this. For finer grain
control use something like: "-remap Caps_Lock-None".

Also try the -nomodtweak and -capslock options.

Another useful option that turns off any Lock keys on the remote side
at startup and end is the -clear_all option. During a session you can
run these remote control commands to modify the Lock keys:
x11vnc -R clear_locks
x11vnc -R clear_all

the former will try to unset any Lock keys, the latter will do same
and also try to make it so no key is pressed down (e.g. "stuck" Alt_L,
etc.)

## Screen Related Issues and Features

### Q-99: The remote display is larger (in number of pixels) than the local display I am running the vncviewer on. I don't like the vncviewer scrollbars, what I can do?

vncviewer has a option (usually accessible via F8 key or -fullscreen
option) for vncviewer to run in full screen, where it will
automatically scroll when the mouse is near the edge of the current
view. For quick scrolling, also make sure Backing Store is enabled on
the machine vncviewer is run on. (XFree86/Xorg disables it by default
for some reason, add Option "backingstore" to XF86Config on the
vncviewer side.)

BTW, contact me if you are having problems with vncviewer in
fullscreen mode with your window manager (i.e. no keyboard response.)
I have a workaround for vncviewer using XGrabServer().

There may also be scaling viewers out there (e.g. TightVNC or UltraVNC
on Windows) that automatically shrink or expand the remote framebuffer
to fit the local display. Especially for hand-held devices. See also
the next FAQ on x11vnc scaling.


### Q-100: Does x11vnc support server-side framebuffer scaling? (E.g. to make the desktop smaller.)

As of Jun/2004 x11vnc provides basic server-side scaling. It is a
global scaling of the desktop, not a per-client setting. To enable it
use the "-scale fraction" option. "fraction" can either be a floating
point number (e.g. -scale 0.75) or the alternative m/n fraction
notation (e.g. -scale 3/4.) Note that if fraction is greater than one
the display is magnified.

Extra resources (CPU, memory I/O, and memory) are required to do the
scaling. If the machine is slow where x11vnc is run with scaling
enabled, the interactive response can be unacceptable. OTOH, if run
with scaling on a fast machine the performance degradation is usually
not a big issue or even noticeable.

It may help to compile x11vnc with compiler option -O3 or -O4 to speed
up the scaling code. Set the CFLAGS env. var. before running
configure.

Also, if you just want a quick, rough "thumbnail" of the display you
can append ":nb" to the fraction to turn on "no blending" mode. E.g.:
"-scale 1/3:nb" Fonts will be difficult to read, but the larger
features will be recognizable. BTW, "no blending" mode is forced on
when scaling 8bpp PseudoColor displays (because blending an indexed
colormap is a bad idea and leads to random colors, use :fb to force it
on.)

One can also use the ":nb" with an integer scale factor (say "-scale
2:nb") to use x11vnc as a screen magnifier for vision impaired
applications. Since with integer scale factors the framebuffers become
huge and scaling operations time consuming, be sure to use ":nb" for
the fastest response.

In general for a scaled display if you are using a TightVNC viewer you
may want to turn off jpeg encoding (e.g. vncviewer -nojpeg host:0.)
There appears to be a noise enhancement effect, especially for regions
containing font/text: the scaling can introduce some pixel artifacts
that evidently causes the tight encoding algorithm to incorrectly
detect the regions as image data and thereby introduce additional
pixel artifacts due to the lossiness of the jpeg compression
algorithm. Experiment to see if -nojpeg vncviewer option improves the
readability of text when using -scale to shrink the display size. Also
note that scaling may actually slow down the transfer of text regions
because after being scaled they do not compress as well. (this can
often be a significant slowdown, e.g. 10X.)

Another issue is that it appears VNC viewers require the screen width
to be a multiple of 4. When scaling x11vnc will round the width to the
nearest multiple of 4. To disable this use the ":n4" sub option (like
":nb" in the previous paragraph; to specify both use a comma:
":nb,n4", etc.)

If one desires per-client scaling for something like 1:1 from a
workstation and 1:2 from a smaller device (e.g. handheld), currently
the only option is to run two (or more) x11vnc processes with
different scalings listening on separate ports (-rfbport option, etc.)

Update: As of May/2006 x11vnc also supports the UltraVNC server-side
scaling. This is a per-client scaling by factors 1/2, 1/3, ... and so
may be useful for PDA's ("-scale 1/2", etc. will give similar results
except that it applies to all clients.) You may need to supply
"-rfbversion 3.6" for this to be recognized by UltraVNC viewers.

BTW, whenever you run two or more x11vnc's on the same X display and
use the GUI, then to avoid all of the x11vnc's simultaneously
answering the gui you will need to use something like "-connect file1
-gui ..." with different connect files for each x11vnc you want to
control via the gui (or remote-control.) The "-connect file1" usage
gives separate communication channels between a x11vnc process and the
gui process. Otherwise they all share the same X property channels:
VNC_CONNECT and X11VNC_REMOTE.

Update: As of Mar/2005 x11vnc now scales the mouse cursor with the
same scale factor as the screen. If you don't want that, use the
"-scale_cursor frac" option to set the cursor scaling to a different
factor (e.g. use "-scale_cursor 1" to keep the cursor at its natural
unscaled size.)


### Q-101: Does x11vnc work with Xinerama? (i.e. multiple monitors joined together to form one big, single screen.)

Yes, it should generally work because it simply polls the big
effective screen.

If the viewing-end monitor is not as big as the remote Xinerama
display, then the vncviewer scrollbars, etc, will have to be used to
pan across the large area. However one user started two x11vnc's, one
with "-clip 1280x1024+0+0" and the other with "-clip 1280x1024+1280+0"
to split the big screen into two and used two VNC viewers to access
them.

As of Jun/2008: Use "-clip xinerama0" to clip to the first xinerama
sub-screen (if xinerama is active.) xinerama1 for the 2nd sub-screen,
etc. This way you don't need to figure out the WxH+X+Y of the desired
xinerama sub-screen. screens are sorted in increasing distance from
the (0,0) origin (I.e. not the Xserver's order.)

There are a couple potential issues with Xinerama however. If the
screen is not rectangular (e.g. 1280x1024 and 1024x768 monitors joined
together), then there will be "non-existent" areas on the screen. The
X server will return "garbage" image data for these areas and so they
may be distracting to the viewer. The -blackout x11vnc option allows
you to blacken-out rectangles by manually specifying their WxH+X+Y
geometries. If your system has the libXinerama library, the -xinerama
x11vnc option can be used to have it automatically determine the
rectangles to be blackened out. (Note on 8bpp PseudoColor displays the
fill color may not be black.) Update: -xinerama is now on by default.

Some users have reported that the mouse does not behave properly for
their Xinerama display: i.e. the mouse cannot be moved to all regions
of the large display. If this happens try using the -xwarppointer
option. This instructs x11vnc to fake mouse pointer motions using the
XWarpPointer function instead of the XTestFakeMotionEvent XTEST
function. (This may be due to a bug in the X server for XTEST when
Xinerama is enabled.) Update: As of Dec/2006 -xwarppointer will be
applied automatically if Xinerama is detected. To disable use:
-noxwarppointer


### Q-102: Can I use x11vnc on a multi-headed display that is not Xinerama (i.e. separate screens :0.0, :0.1, ... for each monitor)?

You can, but it is a little bit awkward: you must start separate
x11vnc processes for each screen, and on the viewing end start up
separate VNC viewer processes connecting to them. e.g. on the remote
end:

```
x11vnc -display :0.0 -bg -q -rfbport 5900
x11vnc -display :0.1 -bg -q -rfbport 5901
```

(this could be automated in the display manager Xsetup for example)
and then on the local machine where you are sitting:

```
vncviewer somehost:0 &
vncviewer somehost:1 &
```

Note: if you are running on Solaris 8 or earlier you can easily hit up
against the maximum of 6 shm segments per process (for Xsun in this
case) from running multiple x11vnc processes. You should modify
/etc/system as mentioned in another FAQ to increase the limit. It is
probably also a good idea to run with the -onetile option in this case
(to limit each x11vnc to 3 shm segments), or even -noshm to use no shm
segments.


### Q-103: Can x11vnc show only a portion of the display? (E.g. for a special purpose application or a very large screen.)

As of Mar/2005 x11vnc has the "-clip WxH+X+Y" option to select a
rectangle of width W, height H and offset (X, Y). Thus the VNC screen
will be the clipped sub-region of the display and be only WxH in size.
One user used -clip to split up a large Xinerama screen into two more
managable smaller screens.

This also works to view a sub-region of a single application window if
the -id or -sid options are used. The offset is measured from the
upper left corner of the selected window.


### Q-104: Does x11vnc support the XRANDR (X Resize, Rotate and Reflection) extension? Whenever I rotate or resize the screen x11vnc just seems to crash.

As of Dec/2004 x11vnc supports XRANDR. You enable it with the -xrandr
option to make x11vnc monitor XRANDR events and also trap X server
errors if the screen change occurred in the middle of an X call like
XGetImage. Once it traps the screen change it will create a new
framebuffer using the new screen.

If the connected vnc viewers support the NewFBSize VNC extension
(Windows TightVNC viewer and RealVNC 4.0 windows and Unix viewers do)
then the viewer will automatically resize. Otherwise, the new
framebuffer is fit as best as possible into the original viewer size
(portions of the screen may be clipped, unused, etc.) For these
viewers you can try the -padgeom option to make the region big enough
to hold all resizes and rotations. We have fixed this problem for the
TightVNC Viewer on Unix: SSVNC

If you specify "-xrandr newfbsize" then vnc viewers that do not
support NewFBSize will be disconnected before the resize. If you
specify "-xrandr exit" then all will be disconnected and x11vnc will
terminate.


### Q-105: Independent of any XRANDR, can I have x11vnc rotate and/or reflect the screen that the VNC viewers see? (e.g. for a handheld whose screen is rotated 90 degrees.)

As of Jul/2006 there is the -rotate option allow this. E.g's: "-rotate
+90", "-rotate -90", "-rotate x", etc.


### Q-106: Why is the view in my VNC viewer completely black? Or why is everything flashing around randomly?

See the next FAQ for a possible explanation.


### Q-107: I use Linux Virtual Terminals (VT's) to implement 'Fast User Switching' between users' sessions (e.g. Betty is on Ctrl-Alt-F7, Bobby is on Ctrl-Alt-F8, and Sid is on Ctrl-Alt-F1: they use those keystrokes to switch between their sessions.)  How come the view in a VNC viewer connecting to x11vnc is either completely black or otherwise all messed up unless the X session x11vnc is attached to is in the active VT?

This seems to have to do with how applications (the X server processes
in this case) must "play nicely" if they are not on the active VT
(sometimes called VC for virtual console.) That is, they should not
read from the keyboard or mouse or manage the video display unless
they have the active VT. Given that it appears the XGetImage() call
must ultimately retrieve the framebuffer data from the video hardware
itself, it would make sense x11vnc's polling wouldn't work unless the
X session had active control of the VT.

There does not seem to be an easy way to work around this. Even xwd(1)
doesn't work in this case (try it.) Something would need to be done at
a lower level, say in the XFree86/Xorg X server. Also, using the
Shadow Framebuffer (a copy of the video framebuffer is kept in main
memory) does not appear to fix the problem.

If no one is sitting at the workstation and you just want to remotely
switch the VT over to the one associated with your X session (so
x11vnc can poll it correctly), one can use the chvt(1) command, e.g.
"chvt 7" for VT #7.


### Q-108: I am using x11vnc where my local machine has "popup/hidden taskbars" and the remote display where x11vnc runs also has  "popup/hidden taskbars" and they interfere and fight with each other.  What can I do?

When you move the mouse to the edge of the screen where the popups
happen, the taskbars interfere with each other in strange ways. This
sometimes happens where the local machine is GNOME or Mac OS X and the
remote machine is GNOME. Is there a way to temporarily disable one or
both of these magic desktop taskbars?

One x11vnc user suggests: it should be straightforward to right mouse
click on the task bar panel, and uncheck "enable auto-hide" from the
panel properties dialog box. This will make the panel always visible.

### Q-109: Help! x11vnc and my KDE screensaver keep switching each other on and off every few seconds.

This is a new (Jul/2006) problem seen, say, on the version of KDE that
is shipped with SuSE 10.1. It is not yet clear what is causing this...
If you move the mouse through x11vnc the screensaver shuts off like it
should but then a second or two after you stop moving the mouse the
screensaver snaps back on.

This may be a bug in kdesktop_lock. For now the only workaround is to
disable the screensaver. You can try using another one such as
straight xscreensaver (see the instructions here for how to disable
kdesktop_lock.) If you have more info on this or see it outside of KDE
please let us know.

Update: It appears this is due to kdesktop_lock enabling the screen
saver when the Monitor is in DPMS low-power state (e.g. standby,
suspend, or off.) In Nov/2006 the x11vnc -nodpms option was added as a
workaround. Normally it is a good thing that the monitor powers down
(since x11vnc can still poll the framebuffer in this state), but if
you experience the kdesktop_lock problem you can specify the "-nodpms"
option to keep the Monitor out of low power state while VNC clients
are connected. This is basically the same as typing "xset dpms force
on" periodically. (if you don't want to do these things just disable
the screensaver.) Feel free to file a bug against kdesktop_lock with
KDE.

### Q-110: I am running the compiz 3D window manager (or beryl, MythTv, Google Earth, or some other OpenGL app) and I do not get screen updates in x11vnc.

This appears to be because the 3D OpenGL/GLX hardware screen updates
do not get reported via the XDAMAGE mechanism. So this is a bug in
compiz/beryl or XDAMAGE/Xorg or the (possibly 3rd party) video card
driver.

As a workaround apply the -noxdamage option. As of Feb/2007 x11vnc
will try to autodetect the problem and disable XDAMAGE if is appears
to be missing a lot of updates. But if you know you are using compiz
you might as well always supply -noxdamage. Thanks to this user who
reported the problem and discovered the workaround.

A developer for MiniMyth reports that the 'alphapulse' tag of the
theme G.A.N.T. can also cause problems, and should be avoided when
using VNC.

Please report a bug or complaint to Beryl/Compiz and/or Xorg about
this: running x11vnc with -noxdamage disables a nice improvement in
responsiveness (especially for typing) and also leads to unnecessary
CPU and memory I/O load due to the extra polling.

Update: as of May/2010 NVIDIA may have fixed this problem in their
proprietary drivers. See the NVIDIA Release Notes. (look for
'x11vnc'.)

### Q-111: Can I use x11vnc to view my VMWare session remotely?

Yes, since VMWare usually runs as an X application you can view it via
x11vnc in the normal way.

Note that VMWare has several viewing modes:

* Normal X application window  (with window manager frame)
* Quick-Switch mode  (with no window manager frame)
* Fullscreen mode

The way VMWare does Fullscreen mode on Linux is to display the Guest
desktop in a separate Virtual Terminal (e.g. VT 8) (see this FAQ on
VT's for background.) Unfortunately, this Fullscreen VT is not an X
server. So x11vnc cannot access it (however, see this discussion of
-rawfb for a possible workaround.) x11vnc works fine with "Normal X
application window" and "Quick-Switch mode" because these use X.

Update: It appears the in VMWare 5.x the Fullscreen mode is X, so
x11vnc access does work.

One user reports he left his machine with VMWare in the Fullscreen
mode, and even though his X session wasn't in the active VT, he could
still connect x11vnc to the X session and pass the keystrokes Ctrl-Alt
(typing "blind") to the VMWare X app. This induced VMWare to switch
out of Fullscreen into Normal X mode and he could continue working in
the Guest desktop remotely.


Aside: Sometimes it is convenient (for performance, etc.) to start
VMWare in its own X session using startx(1). This can be used to have
a minimal window manger (e.g. twm or even no window manager), to
improve response. One can also cut the display depth (e.g. to 16bpp)
in this 2nd X session to improve video performance. This 2nd X session
emulates Fullscreen mode to some degree and can be viewed via x11vnc
as long as the VMWare X session is in the active VT.

Also note that with a little bit of playing with "xwininfo -all
-children" output one can extract the (non-toplevel) window-id of the
of the Guest desktop only when VMWare is running as a normal X
application. Then one can export just the guest desktop (i.e. without
the VMWare menu buttons) by use of the -id windowid option. The
caveats are the X session VMWare is in must be in the active VT and
the window must be fully visible, so this mode is not terribly
convenient, but could be useful in some circumstances (e.g. running
VMWare on a very powerful server machine in a server room that happens
to have a video card, (but need not have a monitor, Keyboard or
mouse).)


## Exporting non-X11 devices via VNC

### Q-112: Can non-X devices (e.g. a raw framebuffer) be viewed (and even controlled) via VNC with x11vnc?

As of Apr/2005 there is support for this. Two options were added:
"-rawfb string" (to indicate the raw frame buffer device, file, etc.
and its parameters) and "-pipeinput command" (to provide an external
program that will inject or otherwise process mouse and keystroke
input.) Some useful -pipeinput schemes, VID, CONSOLE, and UINPUT, have
since been built into x11vnc for convenience.

This non-X mode for x11vnc is somewhat experimental because it is so
removed in scope from the intended usage of the tool. Incomplete
attempt is made to make all of the other options consistent with non-X
framebuffer polling. So all of the X-related options (e.g.
-add_keysyms, -xkb) are just ignored or may cause an error if used. Be
careful applying such an option via remote control.

The format for the -rawfb string is:

```
-rawfb <type>:<object>@<W>x<H>x<bpp>[-<BPL>][:<R>/<G>/<B>][+<offset>]
```

There are also some useful aliases (e.g. "console".) Some examples:

```
-rawfb shm:210337933@800x600x32:ff/ff00/ff0000

-rawfb map:/dev/fb0@1024x768x16

-rawfb map:/tmp/Xvfb_screen0@640x480x8+3232

-rawfb file:/tmp/my.pnm@250x200x24+37

-rawfb file:/dev/urandom@128x128x8

-rawfb snap:/dev/video0@320x240x24 -24to32

-rawfb console

-rawfb vt2

-rawfb video

-rawfb setup:mycmd.sh
```

So the type can be "shm" for shared memory objects, and "map" or
"file" for file objects. "map" uses mmap(2) to map the file into
memory and is preferred over "file" (that uses the slower lseek(2)
access method.) Only use file if map isn't working. BTW, "mmap" is an
alias for "map" and if you do not supply a type and the file exists,
map is assumed (see the -help output and below for some exceptions to
this.) The "snap:" setting applies the -snapfb option with "file:"
type reading (this is useful for exporting webcams or TV tuner video;
see the next FAQ for more info.)

Also, if the string is of the form "setup:cmd" then cmd is run and the
first line of its output retrieved and used as the rawfb string. This
allows initializing the device, determining WxHxB, etc.

The object will be the numerical shared memory id for the case of shm.
The idea here is some other program has created this shared memory
segment and periodically updates it with new framebuffer data. x11vnc
polls the area for changes. See shmat(2) and ipcs(8) for more info.
The ipcs command will list current shared memory segments on the
system. Sometimes you can snoop on a program's framebuffer it did not
expect you would be polling!

The object will be the path to the regular or character special file
for the cases of map and file. The idea here is that in the case of a
regular file some other program is writing/updating framebuffer image
data to it. In the case of a character special (e.g. /dev/fb0) it is
the kernel that is "updating" the framebuffer data.

In most cases x11vnc needs to be told the width, height, and number of
bits per pixel (bpp) of the framebuffer. This is the @WxHxB field. For
the case of the Linux framebuffer device, /dev/fb0, the fbset(8) may
be of use (but may not always be accurate for what is currently
viewable.) In general some guessing may be required, especially for
the bpp. Update: in "-rawfb console" mode x11vnc will use the linuxfb
API to try to guess (it is still not always accurate.) Also try
"-rawfb vtN" (on x11vnc 0.9.7 and later) for the N-th Linux text
console (aka virtual terminal.) If the number of Bytes Per Line is not
WxHxB/8 (i.e. the framebuffer lines are padded) you can specify this
information after WxHxB via "-BPL", e.g. @800x600x16-2048

Based on the bpp x11vnc will try to guess the red, green, and blue
masks (these indicate which bits correspond to each color.) It if gets
it wrong you can specify them manually via the optional ":R/G/B"
field. E.g. ":0xff0000/0x00ff00/0x0000ff" (this is the default for
32bpp.)

Finally, the framebuffer may not begin at the beginning of the memory
object, so use the optional "+offset" parameter to indicate where the
framebuffer information starts. So as an example, the Xvfb virtual
framebuffer has options -shmem and -fbdir for exporting its virtual
screen to either shm or a mapped file. The format of these is XWD and
so the initial header should be skipped. BTW, since XWD is not
strictly RGB the view will only be approximate, but usable. Of course
for the case of Xvfb x11vnc can poll it much better via the X API, but
you get the idea.

By default in -rawfb mode x11vnc will actually close any X display it
happened to open. This is basically to shake out bugs (e.g it will
crash rather than mysteriously interacting with the X display.) If you
want x11vnc to keep the X display open while polling the raw
framebuffer prefix a "+" sign at the beginning of the string (e.g.
+file:/dev/urandom@64x64x8) This could be convenient for keeping the
remote control channel active (it uses X properties.) The "-connect
/path/to/file" mechanism could also be used for remote control to
avoid the X property channel. Rare usage, but if you also supply
-noviewonly in this "+" mode then the mouse and keyboard input are
still sent to the X display, presumably for doing something amusing
with /dev/fb...

Interesting Devices:. Here are some aliases for interesting device
files that can be polled via -rawfb:

```
-rawfb console               /dev/fb0        Linux Console
-rawfb vt2                   /dev/vcsa2      Linux Console (e.g. virtual terminal #2)
-rawfb video                 /dev/video0     Video4Linux Capture device
-rawfb rand                  /dev/urandom    Random Bytes
-rawfb null                  /dev/zero       Zero Bytes (black screen)
```

The Linux console, /dev/fb0, etc needs to have its driver enabled in
the kernel. Some of the drivers are video card specific and
accelerated. The console is either the Text consoles (usually
tty1-tty6), or X graphical display (usually starting at tty7.) In
addition to the text console other graphical ones may be viewed and
interacted with as well, e.g. DirectFB or SVGAlib apps, VMWare non-X
fullscreen, or Qt-embedded apps (PDAs/Handhelds.) By default the
pipeinput mechanisms UINPUT and CONSOLE (keystrokes only) are
automatically attempted in this mode under "-rawfb console".

The Video4Linux Capture device, /dev/video0, etc is either a Webcam or
a TV capture device and needs to have its driver enabled in the
kernel. See this FAQ for details. If specified via "-rawfb Video" then
the pipeinput method "VID" is applied (it lets you change video
parameters dynamically via keystrokes.)

The last two, /dev/urandom and /dev/zero are just for fun, but are
also useful in testing.

All of the above -rawfb options are just for viewing the raw
framebuffer (although some of the aliases do imply keystroke and mouse
pipeinput methods.) That may be enough for certain applications of
this feature (e.g. suppose a video camera mapped its framebuffer into
memory and you just wanted to look at it via VNC.)
To handle the pointer and keyboard input from the viewer users the
"-pipeinput cmd" option was added to indicate a helper program to
process the user input. The input is streamed to it and looks
something like this:

```
Pointer 1 205 257 0 None
Pointer 1 198 253 0 None
Pointer 1 198 253 1 ButtonPress-1
Pointer 1 198 253 0 ButtonRelease-1
Pointer 1 198 252 0 None
Keysym 1 1 119 w KeyPress
Keysym 1 0 119 w KeyRelease
Keysym 1 1 65288 BackSpace KeyPress
Keysym 1 0 65288 BackSpace KeyRelease
Keysym 1 1 112 p KeyPress
Keysym 1 0 112 p KeyRelease
```

Run "-pipeinput tee:/bin/cat" to get a description of the format. Note
that the -pipeinput option is independent of -rawfb mode and so may
have some other interesting uses. The "tee:" prefix means x11vnc will
both process the user input and pipe it to the command. The default is
to just pipe it to the -pipeinput command.

Note the -pipeinput helper program could actually control the raw
framebuffer. In the libvncserver CVS a simple example program
x11vnc/misc/slide.pl is provided that demonstrates a simple jpeg
"slideshow" application. Also the builtin "-pipeinput VID" mode does
this for webcams and TV capture devices (/dev/video0.)

The -pipeinput program is run with these environment variables set:
X11VNC_PID, X11VNC_PROG, X11VNC_CMDLINE, X11VNC_RAWFB_STR to aid its
knowing what is up.

Another example provided in libvncserver CVS is a script to inject
keystrokes into the Linux console (e.g. the virtual consoles:
/dev/tty1, /dev/tty2, etc) in x11vnc/misc/vcinject.pl. It is based on
the vncterm/LinuxVNC.c program also in the libvncserver CVS. So to
view and interact with VT #2 (assuming it is the active VT) one can
run something like:

```
x11vnc -rawfb map:/dev/fb0@1024x768x16 -pipeinput './vcinject.pl 2'
```

This assumes your Linux framebuffer device (/dev/fb0) is properly
configured. See fbset(8) and other documentation. Try
"file:/dev/fb0@WxHxB" as a last resort. Starting with x11vnc 0.8.1,
the above VT injection is built in, as well as WxHxB determination.
Just use something like:

```
x11vnc -rawfb console
```

this will try to guess the active virtual console (via /dev/tty0) and
also the /dev/fb0 WxHxB and rgb masks automatically. Use, e.g.,
"-rawfb console3" to force the VT number. This input method can be
used generally via "-pipeinput CONSOLE". Also starting with x11vnc
0.8.2 the "-pipeinput UINPUT" mode is tried first (it does both
keyboard and mouse input) and then falls back to CONSOLE mode if it is
not available. Here is the -help output for this mode:

```
If the rawfb string begins with "console" the framebuffer device
/dev/fb0 is opened (this requires the appropriate kernel modules to
be installed) and so is /dev/tty0. The latter is used to inject
keystrokes (not all are supported, but the basic ones are.) You
will need to be root to inject keystrokes. /dev/tty0 refers to the
active VT, to indicate one explicitly, use "console2", etc. using
the VT number.

If the Linux version seems to be 2.6 or later and the "uinput"
module appears to be present, then the uinput method will be used
instead of /dev/ttyN. uinput allows insertion of BOTH keystrokes
and mouse input and so it preferred when accessing graphical (e.g.
Qt-embedded) linux console apps. See -pipeinput UINPUT below for
more information on this mode (you may want to also use the
-nodragging and -cursor none options.) Use "console0", etc or
-pipeinput CONSOLE to force the /dev/ttyN method.

Note you can change VT remotely using the chvt(1) command.
Sometimes switching out and back corrects the framebuffer state.

To skip input injecting entirely use "consolex".

The string "/dev/fb0" (1, etc) can be used instead of "console".
This can be used to specify a different framebuffer device, e.g.
/dev/fb1. As a shortcut the "/dev/" can be dropped. If the name is
something nonstandard, use "console:/dev/foofb"

If you do not want x11vnc to guess the framebuffer's WxHxB and
masks automatically (sometimes the kernel gives inaccurate
information), specify them with a @WxHxB at the end of the string.
```

The above is just an example of what can be done. Note that if you
really want to view and interact with the Linux Text console it is
better to use the more accurate and faster LinuxVNC program. The
advantage x11vnc -rawfb might have is that it can allow interaction
with a non-text application, e.g. one based on SVGAlib or Qt-embedded
Also, for example the VMWare Fullscreen mode is actually viewable
under -rawfb and can be interacted with if uinput is enabled.

If the Linux uinput driver is available then full keystroke and mouse
input into the Linux console can be performed. You may be able to
enable uinput via commands like these:

```
modprobe uinput
mknod /dev/input/uinput c 10 223
```

The -rawfb and -pipeinput features are intended to help one creatively
"get out of a jam" (say on a legacy or embedded device) where X is
absent or doesn't work properly. Feedback and bug reports are welcome.
For more control and less overhead use libvncserver in your own C
program that passes the framebuffer to libvncserver.


### Q-113: Can I export the Linux Console (Virtual Terminals) via VNC using x11vnc?

Yes, you may need to be root to access the devices that make up the
linux console.

To access the active Linux console via the computer's framebuffer try
something like:

```
x11vnc -rawfb console
x11vnc -rawfb console2
```

These will try to access the framebuffer through /dev/fb (or /dev/fb0,
etc.) and if it succeeds it will show any text or graphics that is
currently displayed. Keystrokes will be injected via the device
/dev/tty0 (to force an explicit virtual terminal append a number, e.g.
"console2" to select /dev/tty2.)

If your Linux system does not have a framebuffer device (/dev/fb) you
can get one by adding, e.g., vga=0x31B boot parameter. This enables
the VGA framebuffer device at 1280x1024x24. 0x317 gives 1024x768x16,
etc. You can also enable a Linux framebuffer device by modprobing a
framebuffer driver specific to your video card.

Note that this "-rawfb console" mode shows the contents of the
hardware framebuffer, and so will show whatever is on the screen. It
has no concept of Virtual Terminals WRT what there is to view, it
always shows the active virtual terminal.

Another mode is specific to the Linux text Virtual Terminals, it shows
their text and colors (but no graphics) regardless of whether it is
the active VT or not. It is available on x11vnc 0.9.7 and later.
Enable this mode like this:

```
x11vnc -rawfb vt
x11vnc -rawfb vt2
```

The former will select the active one, the latter the 2nd VT. x11vnc
implements this mode by opening the current console text file
"/dev/vcsa2" instead of "/dev/fb". In this way it provides the basic
functionality of the LibVNCServer LinuxVNC program.

The vt mode can be a useful way to try to get a machine's X server
working remotely, e.g. you edit /etc/X11/xorg.conf and then type
startx (or similar, e.g. gdm) in the virtual terminal. A 2nd x11vnc
could be used to see if the X server is now working correctly.


### Q-114: Can I export via VNC a Webcam or TV tuner framebuffer using x11vnc?

Yes, this is possible to some degree with the -rawfb option. There is
no X11 involved: snapshots from the video capture device are used for
the screen image data. See the previous FAQ on -rawfb for background.
For best results, use x11vnc version 0.8.1 or later.

Roughly, one would do something like this:

```
x11vnc -rawfb snap:/dev/video@320x240x32
```

This requires that the system allows simple read(2) access to the
video device. This is true for video4Linux on Linux kernel 2.6 and
later (it won't work for 2.4, you'll need a separate program to
snapshot to a file that you point -rawfb to; ask me if it is not clear
what to do.)

The "snap:" enforces -snapfb mode which appears to be necessary. The
read pointer for video capture devices cannot be repositioned (which
would be needed for scanline polling), but you can read a full frame
of data from the device.

On Linux, if the Video4Linux API is present or the v4l-info(1) program
(related to xawtv) exists in in PATH, then x11vnc can be instructed to
try it to determine the -rawfb WxHxB parameters for you automatically.
In this case one would just type:

```
x11vnc -rawfb video
```

or "-rawfb video1" for the 2nd video device, etc.

x11vnc has also been extended to use the Video4Linux API over v4l-info
if it is available at build time. This enables setting parameters
(e.g. size and brightness) via x11vnc. See the description below.
Without Video4Linux you will need to initialize the settings of the
video device using something like xawtv or spcaview (and then hope the
settings persist until x11vnc reopens the device.)

Many video4linux drivers tend to set the framebuffer to be 24bpp (as
opposed to 32bpp.) Since this can cause problems with VNC viewers,
etc, the -24to32 option will be automatically imposed when in 24bpp.

Note that by its very nature, video capture involves rapid change in
the framebuffer. This is especially true for cameras where slight
wavering in brightness is always happening. This can lead to much
network bandwidth consumption for the VNC traffic and also local CPU
and I/O resource usage. You may want to experiment with "dialing down"
the framerate via the -wait, -slow_fb, or -defer options. Decreasing
the window size and bpp also helps.


   Setting Camera/Tuner parameters via x11vnc:

   There is also some support for setting parameters of the capture
   device. This is done via "-rawfb video:<settings>". This could be
   useful for unattended startup at boottime, etc. Here is the -help
   description:


```
A more sophisticated video device scheme allows initializing the
device's settings using:

    -rawfb video:<settings>

The prefix could also be, as above, e.g. "video1:" to specify the
device file. The v4l API must be available for this to work.
Otherwise, you will need to try to initialize the device with an
external program, e.g. xawtv, spcaview, and hope they persist when
x11vnc re-opens the device.

<settings> is a comma separated list of key=value pairs. The
device's brightness, color, contrast, and hue can be set to
percentages, e.g. br=80,co=50,cn=44,hu=60.

The device filename can be set too if needed (if it does not start
with "video"), e.g. fn=/dev/qcam.

The width, height and bpp of the framebuffer can be set via, e.g.,
w=160,h=120,bpp=16.

Related to the bpp above, the pixel format can be set via the
fmt=XXX, where XXX can be one of: GREY, HI240, RGB555, RGB565,
RGB24, and RGB32 (with bpp 8, 8, 16, 16, 24, and 32 respectively.)
See http://www.linuxtv.org for more info (V4L api.)

For TV/rf tuner cards one can set the tuning mode via tun=XXX where
XXX can be one of PAL, NTSC, SECAM, or AUTO.

One can switch the input channel by the inp=XXX setting, where XXX
is the name of the input channel (Television, Composite1, S-Video,
etc.) Use the name that is in the information about the device that
is printed at startup.

For input channels with tuners (e.g. Television) one can change
which station is selected by the sta=XXX setting. XXX is the
station number. Currently only the ntsc-cable-us (US cable)
channels are built into x11vnc. See the -freqtab option below to
supply one from xawtv. If XXX is greater than 500, then it is
interpreted as a raw frequency in KHz.

Example:

-rawfb video:br=80,w=320,h=240,fmt=RGB32,tun=NTSC,sta=47

one might need to add inp=Television too for the input channel to
be TV if the card doesn't come up by default in that one.

Note that not all video capture devices will support all of the
above settings.

See the -pipeinput VID option below for a way to control the
settings through the VNC Viewer via keystrokes.

As above, if you specify a "@WxHxB..." after the <settings> string
they are used verbatim: the device is not queried for the current
values. Otherwise the device will be queried.
```

Also, if you supply the "-pipeinput VID" (or use "-rawfb Video")
option you can control the settings to some degree via keystroke
mappings, e.g. B to increase the brightness or Up arrow to change the
TV station:

```
For "-pipeinput VID" and you are using the -rawfb for a video
capture device, then an internal list of keyboard mappings is used
to set parameters of the video. The mappings are:

    "B" and "b" adjust the brightness up and down.
    "H" and "h" adjust the hue.
    "C" and "c" adjust the colour.
    "N" and "n" adjust the contrast.
    "S" and "s" adjust the size of the capture screen.
    "I" and "i" cycle through input channels.
    Up and Down arrows adjust the station (if a tuner)
    F1, F2, ..., F6 will switch the video capture pixel
    format to HI240, RGB565, RGB24, RGB32, RGB555, and
    GREY respectively. See -rawfb video for details.
```

See also the -freqtab option to supply your own xawtv channel to
frequency mappings for your country (only ntsc-cable-us is built into
x11vnc.)


### Q-115: Can I connect via VNC to a Qt-embedded/Qt-enhanced/Qtopia application running on my handheld, cell phone, or PC using the Linux console framebuffer (i.e. not X11)?

Yes, the basic method for this is the -rawfb scheme where the Linux
console framebuffer (usually /dev/fb0) is polled and the uinput driver
is used to inject keystrokes and mouse input. Often you will just have
to type:

```
x11vnc -rawfb console
```

(you may need to enable the uinput driver on the system via "modprobe
uinput; mknod /dev/input/uinput c 10 223") If this does not find the
correct frame buffer properties figure them out or guess them and use
something like:

```
x11vnc -rawfb /dev/fb0@640x480x16
```

Also, to force usage of the uinput injection method use "-pipeinput
UINPUT". See the -pipeinput description for tunable parameters, etc.

One problem with the x11vnc uinput scheme is that it cannot guess the
mouse motion "acceleration" used by the windowing application (e.g.
QWS or X11.) For X11 and Qt-embedded the acceleration is usually 2
(i.e. a dx of 1 from the mouse yields a 2 pixel displacement of the
mouse cursor.) The default x11vnc uses is 2, since that is often used.
However for one Qt-embedded system we needed to do:

```
x11vnc -rawfb console  -pipeinput UINPUT:accel=4.0
```

to get reasonable positioning of the mouse.

Even with the correct acceleration setting there is still some drift
(probably because of the mouse threshold where the acceleration kicks
in) and so x11vnc needs to reposition the cursor from 0,0 about 5
times a second. See the -pipeinput UINPUT option for tuning parameters
that can be set (there are some experimental thresh=N tuning
parameters as well)

Currently, one can expect mouse input to be a little flakey. All in
all, the Linux framebuffer input mechanism for Qt-embedded framebuffer
apps is not perfect, but it is usable.

If you need to create a smaller x11vnc binary for a handheld
environment be sure to run strip(1) on it and also consider
configuring with, e.g. "env CPPFLAGS='-DSMALL_FOOTPRINT=1' ./configure
..." to remove rarely used features and large texts (use 2 or 3
instead of 1 to remove more.) Currently (Jul/2006) this can lower the
size of the x11vnc from 1.1MB to 0.6-0.7MB.

The x11vnc uinput method applies to nearly anything on the Linux
framebuffer console, not just Qt-embedded/Qtopia. DirectFB, SDL using
fbcon driver, SVGAlib applications can also be viewed and interacted
with. Even a Linux X session can be viewed and interacted with without
using X11 (and x11vnc does not have to terminate when the X server
restarts!) The Linux Text consoles (F1-F6) also work.

Note that Qt-embedded supplies its own VNC graphics driver, but it
cannot do both the Linux console framebuffer and VNC at the same time,
which is often what is desired from VNC.

Update: We are finding some setups like Qtopia on the IPAQ do not
allow mouse input via uinput. Please help us debug this problem by
trying x11vnc on your device and letting us know what does and does
not work. See the next FAQ for a possible workaround for touchscreens.


### Q-116: How do I inject touch screen input into an Qt-embedded/Qt-enhanced/Qtopia cell phone such as openmoko/qtmoko Neo Freerunner?

The qtmoko project does not use X11 for the graphical display.
Unfortunately the Linux uinput method described in the previous FAQ
does not work because Qt is using TSLIB (touch screen library) to
process the input and it only reads from one device (often
/dev/input/event1) and not from the new UINPUT device that x11vnc
creates (under -pipeinput UINPUT)

So something else needs to be done. It was discovered that by simply
writing the touchscreen events directly to /dev/input/event1 then
input can be injected into the system. There is no x11vnc builtin mode
for this yet (until we understand it better), but there is a working
script provided in x11vnc/misc/qt_tslib_inject.pl. So one could use it
this way for example:

```
x11vnc ... -rawfb console -pipeinput path/to/qt_tslib_inject.pl -env INJECT_OPTIONS=clickonly,cal=/etc/pointercal
```

Read the script for how to enable other options and what the above
options mean (e.g. /etc/pointercal contains TSLIB's calibration
parameters and are necessary to achieve accurate pointing.)

The x11vnc/misc/qt_tslib_inject.pl script can potentially be modified
to handle other devices where the uinput method fails. It could also
be modified to create 'hot keys', etc.

Please let us know how things go if you try this out; there is much to
learn about synthetic input injection in handhelds and cell phones. As
we learn more we can develop a builtin x11vnc mode for this sort of
injection.

Update Dec/2010: There is experimental built-in UINPUT support in the
x11vnc development tarball for qtmoko with touchpad managed by tslib.
See -pipeinput UINPUT for more info. Here is an example:

```
x11vnc -rawfb console -pipeinput UINPUT:touch,tslib_cal=/etc/pointercal,direct_abs=/dev/input/event1,nouinput,dragskip=3
```


### Q-117: Now that non-X11 devices can be exported via VNC using x11vnc, can I build it with no dependencies on X11 header files and libraries?

Yes, as of Jul/2006 x11vnc enables building for -rawfb only support.
Just do something like when building:

```
./configure --without-x    (plus any other flags)
make
```

You can then test via "ldd x11vnc" that the binary does not depend on
libX11.so, etc. See the previous FAQ's for non-X11 framebuffer usage.
If you use this for an interesting non-X11 application please let us
know what you did.


### Q-118: How do I cross compile x11vnc for a different architecture than my Linux i386 or amd64 PC?

You will need a cross-compiling toolchain. Perhaps your distro
provides these or you can find a HOWTO for your distro. We found a
nice one at qtmoko.org for building armel binaries on Debian Linux
i386 machines. It includes most of the libraries that x11vnc needs. We
use that example here.

We ran this script to set PATH, configure, and build:

```
#!/bin/sh

# toolchain from: qtmoko-debian-toolchain-armv4t-eabi.tar.gz

export PATH=/opt/toolchains/arm920t-eabi/bin:$PATH

env CC=arm-linux-gcc ./configure --host=arm-linux --without-avahi

make

arm-linux-strip ./x11vnc/x11vnc
ls -l ./x11vnc/x11vnc
```

Note we had to include --without-avahi due to lack of
libavahi-client.so.3 supplied by the toolchain we used. One would need
to add it if it was desired on the target machine. We also stripped
the binary to make it smaller.

For an embedded system one may also want to add --without-x if the
embedded system does not use X11 and the -rawfb mechanism must be
used.


### Q-119: Does x11vnc support Mac OS X Aqua/Quartz displays natively (i.e. no X11 involved)?

Yes, since Nov/2006 in the development tree (x11vnc-0.8.4 tarball)
there is support for native Mac OS X Aqua/Quartz displays using the
-rawfb mechanism described above. The mouse and keyboard input is
achieved via Mac OS X API's.

So you can use x11vnc as an alternative to OSXvnc (aka Vine Server),
or Apple Remote Desktop (ARD). Perhaps there is some x11vnc feature
you'd like to use on Mac OS X, etc. For a number of activities (e.g.
window drags) it seems to be faster than OSXvnc.

Notes:

X11:  x11vnc will also work (as it has for years) with a X11 server
(XDarwin) running on Mac OS X (people often install this software to
display remote X11 apps on their Mac OS X system, or use some old
favorites locally such as xterm.) However in this case x11vnc will
only work reasonably in single window -id windowid mode (and the
window may need to have mouse focus.)

If you do not have the DISPLAY env. variable set, x11vnc will assume
native Aqua/Quartz on Mac OS X, however if DISPLAY is set it will
assume an X11 connection. Use "-rawfb console" to force the native
display (or unset DISPLAY.)

Update: Leopard sets DISPLAY by default in all sessions. Since it
starts with the string "/tmp/" x11vnc will use that to know if it
should ignore it. Use "-display :0.0" to force it.

Building:  If you don't have the X11 build and runtime packages
installed you will need to build it like this:
(cd to the e.g. x11vnc-0.9, source directory)

```
./configure --without-x
make
```

Win2VNC/x2vnc:  One handy use is to use the -nofb mode to redirect
mouse and keyboard input to a nearby Mac (i.e. one to the side of your
desk) via x2vnc or Win2VNC. See this FAQ for more info.

Options:  Here are the Mac OS X specific x11vnc options:

```
-macnodim              For the native Mac OS X server, disable dimming.
-macnosleep            For the native Mac OS X server, disable display sleep.
-macnosaver            For the native Mac OS X server, disable screensaver.
-macnowait             For the native Mac OS X server, do not wait for the
                        user to switch back to his display.
-macwheel n            For the native Mac OS X server, set the mouse wheel
                        speed to n (default 5.)
-macnoswap             For the native Mac OS X server, do not swap mouse
                        buttons 2 and 3.
-macnoresize           For the native Mac OS X server, do not resize or reset
                        the framebuffer even if it is detected that the screen
                        resolution or depth has changed.
-maciconanim n         For the native Mac OS X server, set n to the number
                        of milliseconds that the window iconify/deiconify
                        animation takes.  In -ncache mode this value will be
                        used to skip the animation if possible. (default 400)
-macmenu               For the native Mac OS X server, in -ncache client-side
                        caching mode, try to cache pull down menus (not perfect
                        because they have animated fades, etc.)
```

PasteBoard/Clipboard:   There is a bug that the Clipboard (called
PasteBoard on Mac it appears) exchange will not take place unless
x11vnc was started from inside the Aqua display (e.g. started inside a
Terminal app window.) Otherwise it cannot connect to the PasteBoard
server. So Clipboard exchange won't work for our standard "ssh in"
startup scheme.

Hopefully this deficiency can be removed, but until then for Clipboard
exchange to work you will need to start x11vnc inside the desktop
session (i.e. either start it running before you leave, or start up a
2nd x11vnc inside from a 1st one started outside, or use the apple
script below)

Here also is a osascript trick that seems to work (it opens the
Terminal app and instructs it to start x11vnc):

```
#!/bin/sh
#
# start_x11vnc: start x11vnc in a Terminal window
# (this will allow Clipboard/Pasteboard exchange to work)

tmp=/tmp/start_x11vnc.$$

cat > $tmp <<END

tell application "Terminal"
        activate
        do script with command "$HOME/x11vnc -rfbauth .vnc/passwd -ssl SAVE"
end tell

END

osascript $tmp
rm -f $tmp
```

where you should customize the x11vnc command line to your needs and
the full path to the binary. Save it in a file e.g. "start_x11vnc" and
then after you SSH in just type "./start_x11vnc" (or have ssh run the
command for you.) Then once you are connected via VNC, iconify the
Terminal windows (you can't delete them since that will kill x11vnc.)

   Update Aug/2010: A user reports the following useful information:

```
This is not a problem on Mac OS X 10.6.x (Snow Leopard) when connecting
via ssh to start x11vnc.  And, on Mac OS X 10.5.x (Leopard), the problem
can be permanently eliminated by doing this:


sudo /usr/libexec/PlistBuddy -c 'delete :LimitLoadToSessionType' \
   -c 'add :LimitLoadToSessionType string Background' \
   /System/Library/LaunchAgents/com.apple.pboard.plist
# ignore any 'Delete: Entry, ":LimitLoadToSessionType", Does Not Exist' message

and then restarting (yes, you must restart not just log off).  But
ONLY do that for Mac OS X 10.5.x and NOT for 10.6.x (which doesn't
need it anyway).
```

We recently got access to a MacOSX 10.6.4 (Snow Leopard) macbook and
have confirmed that the above is correct.


### Q-120: Can x11vnc be used as a VNC reflector/repeater to improve performance for the case of a large number of simultaneous VNC viewers (e.g. classroom broadcasting or a large demo)?

Yes, as of Feb/2007 there is the "-reflect host:N" option to connect
to the VNC server "host:N" (either another x11vnc or any other VNC
server) and re-export it. VNC viewers then connect to the x11vnc(s)
running -reflect.

The -reflect option is the same as: "-rawfb vnc:host:N". See the
-rawfb description under "VNC HOST" for more details.

You can replace "host:N" with "listen" or "listen:port" for reverse
connections.

One can set up a number of such reflectors/repeaters to spread the
resource usage around, e.g.:

```
C -------<-------|
C -------<-------|
C -------<-------|---- R -----|
C -------<-------|            |
C -------<-------|            |
                              |
C -------<-------|            |
C -------<-------|            |
C -------<-------|---- R -----|
C -------<-------|            |
C -------<-------|            |
                              |====== S
C -------<-------|            |
C -------<-------|            |
C -------<-------|---- R -----|
C -------<-------|            |
C -------<-------|            |
                              |
C -------<-------|            |
C -------<-------|            |
C -------<-------|---- R -----|
C -------<-------|
C -------<-------|
```

Where "S" is the original VNC Server, "C" denote VNC viewer clients,
and "R" denotes an x11vnc running -reflect to "S".

Ideally, a client "C" will be fairly close network-wise to its "R". It
is fine to run the "R" on the same machine as one of its "C's". A nice
setup for a large, (e.g. 64-128) viewer classroom broadcast case would
be to run R's on areas isolated by network switches, e.g. one R per
switch.

In an extreme case (e.g. 1000 viewers) one might actually need a 2nd
layer of R's in the tree. If you try something like that let us know!

There are many resource savings in doing something like the above. The
first obvious one is network bandwidth savings. Another is less CPU
load on "S" since it handles many fewer simultaneous connections.
Also, if there are a few clients C on very slow links, their presence
does not slow down every other client, just the clients on their "R".
One way a slow client affects things is if there are some large
framebuffer writes (e.g. jpeg image region) then the repeater may
block waiting for that large write to finish before going onto the
next client (however, if the write is small enough, the kernel will
buffer it and the server can go on to service the next client.)

The x11vnc -reflect implementation uses the libvncclient library in
the LibVNCServer project to handle the connection to "S". It is not
currently very efficient since it simply does its normal framebuffer
polling scheme on the libvncclient framebuffer (which it then
re-exports via VNC to its clients C.) However, CopyRect and
CursorShape encodings are preserved in the reflection and that helps.
Dragging windows with the mouse can be a problem (especially if S is
not doing wireframing somehow, consider -nodragging if the problem is
severe) For a really fast reflector/repeater it would have to be
implemented from scratch with performance in mind. See these other
projects:

* http://sourceforge.net/projects/vnc-reflector/,
* http://www.tightvnc.com/projector/                (closed source?),

Automation via Reverse Connections:   Instead of having the R's
connect directly to S and then the C's connect directly to the R they
should use, some convenience can be achieved by using reverse
connections (the x11vnc ""-connect host1,host2,..." option.) Suppose
all the clients "C" are started up in Listen mode:

```
client1>  vncviewer -listen
client2>  vncviewer -listen
client3>  vncviewer -listen
...
client64> vncviewer -listen
```

(e.g. client1> is the cmdline prompt on machine client1 ... etc) and
all the repeaters R are started like this:

```
repeater1> x11vnc -reflect listen -connect client1,client2,...client8
repeater2> x11vnc -reflect listen -connect client9,client10,...client16
...
repeater8> x11vnc -reflect listen -connect client57,client58,...client64
```

and finally the main server is started to kick the whole thing into
motion:

```
vncserver> x11vnc -display :0 -connect repeater1,repeater2,...repeater8
```

(or instruct a non-x11vnc VNC server to reverse connect to the
repeaters.) For a classroom broadcasting setup one might have the
first two sets of commands start automatically at bootup or when
someone logs in, and then start everything up with the S server. One
may even be able to script the forward connection bootstrap case, let
us know what you did. A really nice thing would be some sort of
auto-discovery of your repeater, etc...

### Q-121: Can x11vnc be used during a Linux, Solaris, etc. system installation so the installation can be done remotely?

This can be done, but it doesn't always work because it depends on how
the OS does its install. We have to "sneak in" somehow. Note that some
OS's have a remote install (ssh etc.) built in and so you might want
to use that instead.

Usually the OS install will have to be a network-install in order to
have networking up during the install. Otherwise, you may have a
(slim) chance to configure the networking manually (ifconfig(8) and
route(8).)

To avoid library dependencies problems in the typical minimal (e.g.
busybox) installation OS it is a good idea to build a statically
linked x11vnc binary. A way that often works is to do a normal build
and then paste the final x11vnc link line into a shell script. Then
change the "gcc" to "gcc -static" and run the shell script. You may
need to disable features (e.g. "--without-xfixes") if there is not a
static library for the feature available. You may also need to add
extra link options (e.g. "-lXrender") to complete library dependencies
manually.

Let's call the binary x11vnc.static. Place it on a webserver
somewhere. It may be possible to retrieve it via scp(1) too.

During the install you need to get a shell to retreive x11vnc.static
and run it.

If the Solaris install is an older X-based one, there will be a menu
for you to get a terminal window. From that window you might be able
to retrieve x11vnc.static via wget, scp, or ftp. Remember to do "chmod
755 ./x11vnc.static" and then find the -auth file as in this FAQ.

If it is a Linux install that uses an X server (e.g. SuSE and probably
Fedora), then you can often get a shell by pressing Ctrl-Alt-F2 or
similar. Then get the x11vnc binary via something like this:

```
cd /tmp
wget http://192.168.0.22/x11vnc.static
chmod 755 ./x11vnc.static
```

Find the name of the auth file as in this FAQ. (maybe run "ps wwaux |
grep auth".) Then run it like this:
./x11vnc.static -forever -nopw -display :0 -auth /tmp/wherever/the/authfile

then press Alt-F7 to go back to the X install. You should now be able
to connect via a vnc viewer and continue the install. Watch out for
the display being :1, etc.

If there is a firewall blocking incoming connections during the
install, use the "-connect hostname" option option for a reverse
connection to the hostname running the VNC viewer in listen mode.

Debian based installs are either console-text or console-framebuffer
based. These are install (or expert) and installgui (or expertgui)
boot lines, respectively. For the console-text based installs you
probably need to add a boot cmd line option like vga=0x314 (which is
800x600x16) to get the console-text to use the linux framebuffer
device properly.

For a Debian console-text based install after the network is
configured press Ctrl-Alt-F2 to get a shell. Retrieve the binary via
wget as above and chmod 755 it. Then run it something like this:
sleep 10; ./x11vnc.static -forever -nopw -rawfb console

then before the sleep is over press Alt-F1 to get back to the install
virtual console. You should be able to connect via a VNC viewer and
continue with the install.

For a recent (2009) Debian install we booted with "expert vga=0x301"
and "expert vga=0x311" to get console text based installs at 640x480x8
and 640x480x16, respectively (replace "expert" with "install" if you
like.) Otherwise it was giving a 16 color 640x480x4 (4 bit per pixel)
display which x11vnc could not handle.

For Debian console-framebuffer GUI based installs (installgui or
expertgui) we have not be able to enter keystrokes or mouse motions.
This may be resolved if the install had the Linux kernel module
uinput, but it doesn't; one can wget uinput.ko and then run insmod on
it, but the module must match the installation kernel. So, failing
that, you can only do the GUI view-only, which can be handy to watch a
long network install from your desk instead of in front of the machine
being installed. For these, after the network is configured press
Ctrl-Alt-F2 to get a shell. Retrieve the binary via wget as above and
chmod 755 it. Then run it something like this:
sleep 10; ./x11vnc.static -forever -nopw -rawfb console

then before the sleep is over press Alt-F5 to get back to the GUI
install console. You should be able to connect via a VNC viewer and
watch the install.
[Misc: Clipboard, File Transfer/Sharing, Printing, Sound, Beeps,
Thanks, etc.]

### Q-122: Does the Clipboard/Selection get transferred between the vncviewer and the X display?

As of Jan/2004 x11vnc supports the "CutText" part of the RFB (aka VNC)
protocol. When text is selected/copied in the X session that x11vnc is
polling it will be sent to connected VNC viewers. And when CutText is
received from a VNC viewer then x11vnc will set the X11 selections
PRIMARY, CLIPBOARD, and CUTBUFFER0 to it. x11vnc is able to hold the
PRIMARY and CLIPBOARD selections (Xvnc does not seem to do this.)

The X11 selections can be confusing, especially to those coming from
Windows or MacOSX where there is just a single 'Clipboard'. The X11
CLIPBOARD selection is a lot like that of Windows and MacOSX, e.g.
highlighted text is sent to the clipboard when the user activates
"Edit -> Copy" or presses "Control+C" (and pasting it via "Edit ->
Paste" or "Control+V".) The X11 PRIMARY selection has been described
as 'for power users' or 'an Easter Egg'. As soon as text is
highlighted it is set to the PRIMARY selection and so it is
immediately ready for pasting, usually via the Middle Mouse Button or
"Shift+Insert". See this jwz link for more information.

x11vnc's default behavior is to watch both CLIPBOARD and PRIMARY and
whenever one of them changes, it sends the new text to connected
viewers. Note that since the RFB protocol only has a single "CutText"
then both selections are "merged" to some degree (and this can lead to
confusing results.) One user was confused why x11vnc was "forgetting"
his CLIPBOARD selection and the reason was he also changed PRIMARY
some time after he copied text to the clipboard. Usually an app will
set PRIMARY as soon as any text is highlighted so it easy to see how
CLIPBOARD was forgotten. Use the -noprimary described below as a
workaround. Similarly, by default when x11vnc receives CutText it sets
both CLIPBOARD and PRIMARY to it (this is probably less confusing, but
could possibly lead to some failure modes as well.)

You may not like these defaults. Here are ways to change the behavior:

* If you don't want the Clipboard/Selection exchanged at all use the
  -nosel option.
* If you want changes in PRIMARY to be ignored use the -noprimary
  option.
* If you want changes in CLIPBOARD to be ignored use the
  -noclipboard option.
* If you don't want x11vnc to set PRIMARY to the "CutText" received
  from viewers use the -nosetprimary option.
* If you don't want x11vnc to set CLIPBOARD to the "CutText"
  received from viewers use the -nosetclipboard option.

You can also fine-tune it a bit with the -seldir dir option and also
-input.

You may need to watch out for desktop utilities such as KDE's
"Klipper" that do odd things with the selection, clipboard, and
cutbuffers.


### Q-123: Can I use x11vnc to record a Shock Wave Flash (or other format) video of my desktop, e.g. to record a tutorial or demo?

Yes, it is possible with a number of tools that record VNC and
transform it to swf format or others. One such popular tool is
pyvnc2swf. There are a number of tutorials (broken link?) on how to do
this. Another option is to use the vnc2mpg that comes in the
LibVNCServer package.
An important thing to remember when doing this is that tuning
parameters should be applied to x11vnc to speed up its polling for
this sort of application, e.g. "-wait 10 -defer 10".

### Q-124: Can I transfer files back and forth with x11vnc?

As of Oct/2005 and May/2006 x11vnc enables, respectively, the TightVNC
and UltraVNC file transfer implementations that were added to
libvncserver. This currently works with TightVNC and UltraVNC viewers
(and Windows viewers only support filetransfer it appears... but they
do work to some degree under Wine on Linux.)

The SSVNC Unix VNC viewer supports UltraVNC file transfer by use of a
Java helper program.

TightVNC file transfer is off by default, if you want to enable it use
the -tightfilexfer option.

UltraVNC file transfer is off by default, to enable it use something
like "-rfbversion 3.6 -permitfiletransfer"
options (UltraVNC incorrectly uses the RFB protocol version to
determine if its features are available, so x11vnc has to pretend to
be version 3.6.) As of Sep/2006 "-ultrafilexfer" is an alias for these
two options. Note that running as RFB version 3.6 may confuse other
VNC Viewers.

Sadly you cannot do both -tightfilexfer and -ultrafilexfer at the same
time because the latter requires setting the version to 3.6 and
tightvnc will not do filetransfer when it sees that version number.

Also, because of the way the LibVNCServer TightVNC file transfer is
implemented, you cannot do Tightvnc file transfer in -unixpw mode.
However, UltraVNC file transfer does work in -unixpw (but if a client
tries it do a filetransfer during the login process it will be
disconnected.)

IMPORTANT: please understand if -ultrafilexfer or -tightfilexfer is
specified and you run x11vnc as root for, say, inetd or display
manager (gdm, kdm, ...) access and you do not have it switch users via
the -users option, then VNC Viewers that connect are able to do
filetransfer reads and writes as *root*.

The UltraVNC and TightVNC settings can be toggled on and off inside
the gui or by -R remote control. However for TightVNC the changed
setting only applies for NEW clients, current clients retain their
TightVNC file transfer ability. For UltraVNC it works better, however
if an UltraVNC client has initiated a file transfer dialog it will
remain in effect until the dialog is closed. If you want to switch
between UltraVNC and TightVNC file transfer in the gui or by remote
control you will probably be foiled by the "-rfbversion 3.6" issue.


### Q-125: Which UltraVNC extensions are supported?

Some of them are supported. To get UltraVNC Viewers to attempt to use
these extensions you will need to supply this option to x11vnc:
-rfbversion 3.6

Or use -ultrafilexfer which is an alias for the above option and
"-permitfiletransfer". UltraVNC evidently treats any other RFB version
number as non-UltraVNC.

Here are a list of the UltraVNC extensions supported by x11vnc:
    * ServerInput: "Toggle Remote Input and Remote Blank Monitor"
    * FileTransfer: "Open File Transfer..."
    * SingleWindow: "Select Single Window..."
    * TextChat: "Open Chat..."
    * 1/n Server Scaling

The SSVNC Unix VNC viewer supports these UltraVNC extensions.

To disable SingleWindow and ServerInput use -noultraext (the others
are managed by LibVNCServer.) See this option too: -noserverdpms.

Also, the UltraVNC repeater proxy is supported for use with reverse
connections: "-connect repeater://host:port+ID:NNNN". Use it for both
plaintext and SSL connections. This mode can send any string before
switching to the VNC protocol, and so could be used with other
proxy/gateway tools. Also, a perl repeater implemention is here:
ultravnc_repeater.pl


### Q-126: Can x11vnc emulate UltraVNC's Single Click helpdesk mode for Unix? I.e. something very simple for a naive user to initiate a reverse vnc connection from their Unix desktop to a helpdesk operator's VNC Viewer.

Yes, UltraVNC's Single Click (SC) mode can be emulated fairly well on
Unix.

We use the term "helpdesk" below, but it could be any sort of remote
assistance you want to set up, e.g. something for Unix-using friends
or family to use. This includes Mac OS X.

Assume you create a helpdesk directory "hd" on your website:
http://www.mysite.com/hd (any website that you can upload files to
should work, although remember the user will be running the programs
you place there.)

In that "hd" subdirectory copy an x11vnc binary to be run on the Unix
user's machine (e.g. Linux, etc) and also create a file named "vnc"
containing the following:

```
#!/bin/sh

webhost="http://www.mysite.com/hd"  # Your helpdesk dir URL.

vnchost="ip.someplace.net"          # Your host running 'vncviewer -listen'
                                    # It could also be your IP number. If it is
                                    # a router/firewall, you will need to
                                    # configure it to redirect port 5500 to your
                                    # workstation running 'vncviewer -listen'

dir=/tmp/vnc_helpdesk.$$            # Make a temporary working dir.
mkdir $dir || exit 1
cd $dir || exit 1

trap "cd /tmp; rm -rf $dir" 0 2 15  # Cleans up on exit.

wget $webhost/x11vnc                # Fetch x11vnc binary.  If multi-
chmod 755 ./x11vnc                  # platform, use $webhost/`uname`/x11vnc
                                    # or similar.

./x11vnc -connect_or_exit $vnchost -rfbport 0 -nopw
```

with the hostnames / IP addresses customized to your case.

On the helpdesk VNC viewer machine (ip.someplace.net in this example)
you have the helpdesk operator running VNC viewer in listen mode:
vncviewer -listen

or if on Windows, etc. somehow have the VNC viewer be in "listen"
mode.

Then, when the naive user needs assistance you instruct him to open up
a terminal window on his Unix desktop and paste the following into the
shell:

```
wget -qO - http://www.mysite.com/hd/vnc | sh -
```

and then press Enter. You could have this instruction on a web page or
in an email you send him, etc. This requires that the wget is
installed on the user's Unix machine (he might only have curl or lynx,
see below for more info.)


So I guess this is about 3-4 clicks (start a terminal and paste) and
pressing "Enter" instead of "single click"...

See this page for some variations on this method, e.g. how to add a
password, SSL Certificates, etc.


If you don't have a website (there are many free ones) or don't want
to use one you will have to email him all of the ingredients (x11vnc
binary and a launcher script) and tell him how to run it. This could
be easy or challenging depending on the skill of the naive unix
user...

A bit of obscurity security could be put in with a -passwd, -rfbauth
options, etc. (note that x11vnc will require a password even for
reverse connections.) More info here.


Firewalls: If the helpdesk (you) with the vncviewer is behind a
NAT/Firewall/Router the router will have to be configured to redirect
a port (i.e. 5500 or maybe different one if you like) to the vncviewer
machine. If the vncviewer machine also has its own host-level
firewall, you will have to open up the port there as well.

NAT-2-NAT: There is currently no way to go "NAT-2-NAT", i.e. both User
and Helpdesk workstations behind NAT'ing Firewall/Routers without
configuring a router to do a port redirection (i.e. on your side, the
HelpDesk.) To avoid modifying either firewall/router, one would need
some public (IP address reachable on the internet) redirection/proxy
service. Perhaps such a thing exists. http://sc.uvnc.com provides this
service for their UltraVNC Single Click users.

Update: It may be possible to do "NAT-2-NAT" with a UDP tunnel such as
http://samy.pl/pwnat/. All that is required is that both NAT firewalls
allow in UDP packets from an IP address to which a UDP packet has
recently been sent to. If you try it out let us know how it went.


Very Naive Users:

If it is beyond the user how to open a terminal window and paste in a
command (you have my condolences...) you would have to somehow setup
his Web browser to download the "vnc" file (or a script containing the
above wget line) and prompt the user if he wants to run it. This may
be tricky to set up (which is probably a good thing to not have the
web browser readily run arbitrary programs downloaded from the
internet...)

One command-line free way, tested with KDE, is to name the file vnc.sh
and then instruct the user to right-click on the link and do "Save
Link As" to his Desktop. It will appear as an icon, probably one that
looks like a terminal or a command line prompt. He next should
right-click on the icon and select "Properties" and go to the
"Permissions" tab. Then in that dialog select the checkbox "Is
executable". He should then be able to click on the icon to launch it.
Another option is to right-click on the icon and select "Open With ->
Other ..." and for the name of the application type in "/bin/sh".
Unfortunately in both cases the command output is lost and so errors
cannot be debugged as easily. A similar thing appears to work in GNOME
if under "Properties -> Permissions" they click on "Execute" checkbox
for "Owner". Then when they click on the icon, they will get a dialog
where they can select "Run in Terminal". In general for such cases, if
it is feasible, it might be easier to ssh to his machine and set
things up yourself...


SSL Encrypted Helpdesk Connections:

As of Apr/2007 x11vnc supports reverse connections in SSL and so we
can do this. On the Helpdesk side (Viewer) you will need STUNNEL or
better use the Enhanced TightVNC Viewer (SSVNC) package we provide
that automates all of the SSL for you.

To do this create a file named "vncs" in the website "hd" directory
containing the following:

```
#!/bin/sh

webhost="http://www.mysite.com/hd"  # Your helpdesk dir URL.

vnchost="ip.someplace.net"          # Your host running 'vncviewer -listen'
                                    # It could also be your IP number. If it is
                                    # a router/firewall, you will need to
                                    # configure it to redirect port 5500 to your
                                    # workstation running 'vncviewer -listen'

dir=/tmp/vnc_helpdesk.$$            # Make a temporary working dir.
mkdir $dir || exit 1
cd $dir || exit 1

trap "cd /tmp; rm -rf $dir" 0 2 15  # Cleans up on exit.

wget $webhost/x11vnc                # Fetch x11vnc binary.  If multi-
chmod 755 ./x11vnc                  # platform, use $webhost/`uname`/x11vnc
                                    # or similar.

./x11vnc -connect_or_exit $vnchost -rfbport 0 -nopw -ssl    # Note -ssl option.
```

with the hostnames or IP addresses customized to your case.

The only change from the "vnc" above is the addition of the -ssl
option to x11vnc. This will create a temporary SSL cert: openssl(1)
will need to be installed on the user's end. A fixed SSL cert file
could be used to avoid this (and provide some authentication; more
info here.)

The naive user will be doing this:

```
wget -qO - http://www.mysite.com/hd/vncs | sh -
```

(or perhaps even use https:// if available.)

But before that, the helpdesk operator needs to have "vncviewer
-listen" running as before, however he needs an SSL tunnel at his end.
The easiest way to do this is use Enhanced TightVNC Viewer (SSVNC).
Start it, and select Options -> 'Reverse VNC Connection (-listen)'.
Then UN-select 'Verify All Certs' (this can be enabled later if you
want; you'll need the x11vnc SSL certificate), and click 'Listen'.

If you don't want to use SSVNC for the viewer, but rather set up
STUNNEL manually instead, make a file "stunnel.cfg" containing:

```
foreground = yes
pid =

[vnc]
accept = 5500
connect = localhost:5501
```

and run:

```
stunnel ./stunnel.cfg
```

and then start the "vncviewer -listen 1" (i.e. 1 to correspond to the
5501 port.) Note that this assumes the stunnel install created a
Server SSL cert+key, usually /etc/stunnel/stunnel.pem (not all distros
will do this.) Also, that file is by default only readable by root, so
stunnel needs to be run as root. If your system does not have a key
installed or you do not want to run stunnel as root (or change the
permissions on the file), you can use x11vnc to create one for you for
example:

```
x11vnc -sslGenCert server self:mystunnel
```

answer the prompts with whatever you want; you can take the default
for all of them if you like. The openssl(1) package must be installed.
See this link and this one too for more info on SSL certs. This
creates $HOME/.vnc/certs/server-self:mystunnel.pem, then you would
change the "stunnel.cfg" to look something like:

```
foreground = yes
pid =
cert = /home/myusername/.vnc/certs/server-self:mystunnel.pem

[vnc]
accept = 5500
connect = localhost:5501
```

In any event, with stunnel having been setup, the naive user is
instructed to paste in and run:
wget -qO - http://www.mysite.com/hd/vncs | sh -

to pick up the vncs script this time.

Of course if a man-in-the-middle can alter what the user downloads
then all bets are off!.

More SSL variations and info about certificates can be found here.


OpenSSL libssl.so.0.9.7 problems:

If you build your own stunnel or x11vnc for deployment, you may want
to statically link libssl.a and libcrypto.a into it because Linux
distros are currently a bit of a mess regarding which version of
libssl is installed.

You will find the details here.


### Q-127: Can I (temporarily) mount my local (viewer-side) Windows/Samba File share on the machine where x11vnc is running?

You will have to use an external network redirection for this.
Filesystem mounting is not part of the VNC protocol.

We show a simple Samba example here.

First you will need a tunnel to redirect the SMB requests from the
remote machine to the one you sitting at. We use an ssh tunnel:
sitting-here> ssh -C -R 1139:localhost:139 far-away.east

Or one could combine this with the VNC tunnel at the same time, e.g.:

```
sitting-here> ssh -C -R 1139:localhost:139 -t -L 5900:localhost:5900 far-away.east 'x11vnc -localhost -display :0'
```

Port 139 is the Windows Service port. For Windows systems instead of
Samba, you may need to use the actual IP address of the Window machine
instead of "localhost" in the -R option (since the Windows service
does not listen on localhost by default.)

Note that we use 1139 instead of 139 on the remote side because 139
would require root permission to listen on (and you may have a samba
server running on it already.)

The ssh -C is to enable compression, which might speed up the data
transfers.

Depending on the remote system side configuration, it may or may not
be possible to mount the SMB share as a non-root user. Try it first as
a non-root user and if that fails you will have to become root.

We will assume the user name is "fred" and we will try to mount the
viewer-side Windows SMB share "//haystack/pub" in
/home/fred/smb-haystack-pub.

```
far-away> mkdir -p /home/fred/smb-haystack-pub
far-away> smbmount //haystack/pub /home/fred/smb-haystack-pub -o username=fred,ip=127.0.0.1,port=1139
```

(The 2nd command may need to be run as root.) Then run "df" or "ls -l
/home/fred/smb-haystack-pub" to see if it is mounted properly. Consult
the smbmount(8) and related documentation (it may require some
fiddling to get write permissions correct, etc.) To unmount:

```
far-away> smbumount /home/fred/smb-haystack-pub
```

At some point we hope to fold some automation for SMB ssh redir setup
into the Enhanced TightVNC Viewer (SSVNC) package we provide (as of
Sep 2006 it is there for testing.)


### Q-128: Can I redirect CUPS print jobs from the remote desktop where x11vnc is running to a printer on my local (viewer-side) machine?

You will have to use an external network redirection for this.
Printing is not part of the VNC protocol.

We show a simple Unix to Unix CUPS example here. Non-CUPS port
redirections (e.g. LPD) should also be possible, but may be a bit more
tricky. If you are viewing on Windows SMB and don't have a local cups
server it may be trickier still (see below.)

First you will need a tunnel to redirect the print requests from the
remote machine to the one you sitting at. We use an ssh tunnel:
sitting-here> ssh -C -R 6631:localhost:631 far-away.east

Or one could combine this with the VNC tunnel at the same time, e.g.:

```
sitting-here> ssh -C -R 6631:localhost:631 -t -L 5900:localhost:5900 far-away.east 'x11vnc -localhost -display :0'
```

Port 631 is the default CUPS port. The above assumes you have a Cups
server running on your viewer machine (localhost:631), if not, use
something like my-cups-srv:631 (the viewer-side Cups server) in the -R
instead.

Note that we use 6631 instead of 631 on the remote side because 631
would require root permission to listen on (and you likely have a cups
server running on it already.)

Now the tricky part: to get applications to notice your cups
server/printer on localhost:6631.

If you have administrative privilege (i.e. root password) on the
x11vnc side where the desktop is running, it should be easy to add the
printer through some configuration utility (e.g. in KDE: Utilities ->
Printing -> Printing Manager, and then supply admin password, and then
Add Printer/Class, and then fill in the inquisitive wizard. Most
important is the "Remote IPP server" panel where you put in localhost
for Host and 6631 for Port.) The main setting you want to convey is
the host is localhost and the port is non-standard (e.g. 6631.) Some
configuration utilities will take an Internet Printing Protocol (IPP)
URI, e.g. http://localhost:6631/printers/,
ipp://localhost:6631/printers/printer-name,
ipp://localhost:6631/ipp/printer-name, etc. Check your CUPS
documentation and admin interfaces to find what the syntax is and what
the "printer name" is.

If you do not have root or print admin privileges, but are running a
recent (version 1.2 or greater) of the Cups client software, then an
easy way to temporarily switch Cups servers is to create the directory
and file: $HOME/.cups/client.conf on the remote side with a line like:

```
ServerName localhost:6631
```

When not using x11vnc for remote access you can comment the above line
out with a '#' (or rename the client.conf file), to have normal cups
operation.

Unfortunately, running applications may need to be restarted to notice
the new printers (libcups does not track changes in client.conf.)
Depending on circumstances, a running application may actually notice
the new printers without restarting (e.g. no print dialog has taken
place yet, or there are no CUPS printers configured on the remote
side.)

Cups client software that is older (1.1) does not support appending
the port number, and for newer ones there is a bug preventing it from
always working (fixed in 1.2.3.) Kludges like these at the command
line will work:

```
far-away> env CUPS_SERVER=localhost IPP_PORT=6631 lpstat -p -d
far-away> env CUPS_SERVER=localhost IPP_PORT=6631 lpr -P myprinter file.ps
far-away> env CUPS_SERVER=localhost IPP_PORT=6631 firefox
```

but are somewhat awkward since you have to retroactively set the env.
var IPP_PORT. Its value cannot be broadcast to already running apps
(like the $HOME/.cups/client.conf trick sometimes does.) A common
workaround for an already running app is to somehow get it to "Print
To File", e.g. file.ps and then use something like the lpr example
above. Also, the option "-h host:port" works with CUPS lp(1) and
lpr(1).

You can also print to Windows shares printers in principle. You may do
this with the smbspool(8) command, or configure the remote CUPS via
lpadmin(8), etc, to use a printer URI something like
smb://machine:port/printer (this may have some name resolution
problems WRT localhost.) Also, as with SMB mounting, the port redir
(-R) to the Windows machine must use the actual IP address instead of
"localhost".

At some point we hope to fold some automation for CUPS ssh redir setup
into the Enhanced TightVNC Viewer (SSVNC) package we provide (as of
Sep 2006 it is there for testing.)


### Q-129: How can I hear the sound (audio) from the remote applications on the desktop I am viewing via x11vnc?

You will have to use an external network audio mechanism for this.
Audio is not part of the VNC protocol.

We show a simple Unix to Unix esd example here (artsd should be
possible too, we have also verified the esd Windows port works for the
method described below.)

First you will need a tunnel to redirect the audio from the remote
machine to the one you sitting at. We use an ssh tunnel:
sitting-here> ssh -C -R 16001:localhost:16001 far-away.east

Or one could combine this with the VNC tunnel at the same time, e.g.:

```
sitting-here> ssh -C -R 16001:localhost:16001 -t -L 5900:localhost:5900 far-away.east 'x11vnc -localhost -display :0'
```

Port 16001 is the default ESD uses. So when an application on the
remote desktop makes a sound it will connect to this tunnel and be
redirected to port 16001 on the local machine (sitting-here in this
example.) The -C option is an attempt to compress the audio a little
bit.

So we next need a local (sitting-here) esd daemon running that will
receive those requests and play them on the local sound device:

```
sitting-here> esd -promiscuous -port 16001 -tcp -bind 127.0.0.1
```

See the esd(1) man page for the meaning of the options (the above are
not very secure.) (This method also works with the EsounD windows port
esd.exe)

To test this sound tunnel, we use the esdplay program to play a simple
.wav file:

```
far-away> esdplay -s localhost:16001 im_so_happy.wav
```

If you hear the sound (Captain Kirk in this example), that means you
are in great shape.

To run individual audio applications you can use the esddsp(1)
command:

```
far-away> esddsp -s localhost:16001 xmms
```

Then you could try playing some sounds inside xmms. You could also set
the environment variable ESPEAKER=localhost:16001 to not need to
supply the -s option all the time. (for reasons not clear, sometimes
esddsp can figure it out on its own.) All the script esddsp does is to
set ESPEAKER and LD_PRELOAD for you so that when the application opens
the sound device (usually /dev/dsp) its interactions with the device
will be intercepted and sent to the esd daemon running on sitting-here
(that in turn writes them to the real, local /dev/dsp.)

Redirecting All sound:

It does not seem to be possible to switch all of the sound of the
remote machine from its sound device to the above esd+ssh tunnel
without some preparation. But it can be done reasonably well if you
prepare (i.e. restart) the desktop with this in mind.

Here is one way to redirect all sound. The idea is we run the entire
desktop with sound directed to localhost:16001. When we are sitting at
far-away.east we run "esd -promiscuous -port 16001 -tcp -bind
127.0.0.1" on far-away.east (to be able to hear the sound.) However,
when we are sitting at sitting-here.west we kill that esd process and
run that same esd command on sitting-here.west and start up the above
ssh tunnel. This is a little awkward, but with some scripts one would
probably kill and restart the esd processes automatically when x11vnc
is used.

So next we have to run the whole desktop pointing toward our esd. Here
is a simple way to test. Log in to the machine via the "FailSafe"
desktop. Then in the lone terminal type something like:

```
esddsp -s localhost:16001 gnome-session
```

or:

```
esddsp -s localhost:16001 startkde
```

where the last part is whatever command starts your desktop (even
fvwm2.) This causes the environment variables ESPEAKER and LD_PRELOAD
to be set appropriately and every application (processes with the
desktop as an ancestor) will use them. If this scheme works well you
can make it less klunky by adding the command to your ~/.xsession,
etc. file that starts your default desktop. Or you may be able to
configure your desktop to use localhost:16001, or whatever is needed,
via a gui configuration panel. Some Notes:

* Not all audio applications are compatible with the esd and artsd
  mechanisms, but many are.
* The audio is not compressed so you probably need a broadband or
  faster connection. Listening to music may not be very pleasant...
  (Although we found streaming music from across the US over cable
  modem worked OK, but took 200 KB/sec, to use less bandwidth
  consider something like "ssh far-away.east 'cat favorite.mp3' |
  mpg123 -b 4000 -")
* Linux does not seem to have the concept of LD_PRELOAD_64 so if you
  run on a mixed 64- and 32-bit ABI system (e.g. AMD x86_64) some of
  the applications will fail to run because LD_PRELOAD will point to
  libraries of the wrong wordsize.
* At some point we hope to fold some automation for esd or artsd ssh
  redir setup into the Enhanced TightVNC Viewer (SSVNC) package we
  provide (as of Sep/2006 it is there for testing.)


### Q-130: Why don't I hear the "Beeps" in my X session (e.g. when typing tput bel in an xterm)?

As of Dec/2003 "Beep" XBell events are tracked by default. The X
server must support the XKEYBOARD extension (this is not on by default
in Solaris, see Xserver(1) for how to turn it on via +kb), and so you
won't hear them if the extension is not present.

If you don't want to hear the beeps use the -nobell option. If you
want to hear the audio from the remote applications, consider trying a
redirector such as esd.


### Q-131: Does x11vnc work with IPv6?

Update: as of Apr/2010 in the 0.9.10 x11vnc development tarball, there
is now built-in support for IPv6 (128 bit internet addresses.) See the
-6 and -connect options for details.

The remainder of this FAQ entry shows how to do with this with pre
0.9.10 x11vnc using IPv6 helper tools.

---

Using an external IPv6 helper:
A way to do this is via a separate helper program such as inetd (or
for encrypted connections: ssh or stunnel.) For example, you configure
x11vnc to be run from inetd or xinetd and instruct it to listen on an
IPv6 address. For xinetd the setting "flags = IPv6" will be needed.
For inetd.conf, an example is:

```
5900 stream tcp6 nowait root /usr/sbin/tcpd /usr/local/bin/x11vnc_wrapper.sh
```

We also provide a transitional tool in "x11vnc/misc/inet6to4" that
acts as a relay for any IPv4 application to allow connections over
IPv6. For example:

```
inet6to4 5900 localhost:5900
```

where x11vnc is listening on IPv4 port 5900.

Also note that not all VNC Viewers are IPv6 enabled, so a redirector
may also be needed for them. The tool "inet6to4 -r ..." can do this as
well. SSVNC (see below) supports IPv6 without need for the helper.

```
# ./inet6to4 -help

inet6to4:  Act as an ipv6-to-ipv4 relay for tcp applications that
           do not support ipv6.

Usage:     inet6to4
           inet6to4 -r

Examples:  inet6to4 5900 localhost:5900
           inet6to4 8080 web1:80
           inet6to4 -r 5900 fe80::217:f2ff:fee6:6f5a%eth0:5900

The -r option reverses the direction of translation (e.g. for ipv4
clients that need to connect to ipv6 servers.)  Reversing is the default
if this script is named 'inet4to6' (e.g. by a symlink.)

Use Ctrl-C to stop this program.

You can also set env. vars INET6TO4_LOOP=1 or INET6TO4_LOOP=BG
to have an outer loop restarting this program (BG means do that
in the background), and INET6TO4_LOGFILE for a log file.
Also set INET6TO4_VERBOSE to verbosity level and INET6TO4_WAITTIME
and INET6TO4_PIDFILE (see below.)
```

The "INET6TO4_LOOP=BG" and "INET6TO4_LOGFILE=..." env. variables make
the tool run reliably as a daemon for very long periods. Read the top
part of the script for more information.

---

Encrypted Tunnels with IPv6 Support:
For SSH tunnelled encrypted VNC connections, one can of course use the
IPv6 support in ssh(1).

For SSL encrypted VNC connections, one possibility is to use the IPv6
support in stunnel(1). This includes the built-in support via the
-stunnel option. For example:

```
x11vnc -stunnel SAVE -env STUNNEL_LISTEN=:: -env STUNNEL_DEBUG=1 ...
```

---

SSH IPv6 Tricks:
It is interesting to note that ssh(1) can do basically the same thing
as inet6to4 above by:

```
ssh -g -L 5900:localhost:5901 localhost "printf 'Press Enter to Exit: '; read x"
```

(where we have x11vnc running via "-rfbport 5901" in this case.)

Note that one can also make a home-brew SOCKS5 ipv4-to-ipv6 gateway
proxy using ssh like this:

```
ssh -D '*:1080' localhost "printf 'Press Enter to Exit: '; read x"
```

then specify a proxy like socks://hostname:1080 where hostname is the
machine running the above ssh command (add -v to ssh for connection
logging info.)

---

IPv6 SSVNC Viewer:
Our SSVNC VNC Viewer is basically a wrapper for ssh(1) and stunnel(1),
and so it already has good IPv6 support because these two commands do.
On Unix, MacOSX, and Windows nearly all of the the remaining parts of
SSVNC (e.g. the built-in proxying and un-encrypted connections) have
been modified to support IPv6 in SSVNC 1.0.26.

Contributions:

### Q-132: Thanks for your program or for your help! Can I make a donation?

Please do (any amount is appreciated; very few have donated) and thank
you for your support! Click on the PayPal button below for more info.

[x-click-but04.gif]-Submit

### Q-133: I am experiencing extreme framebuffer update lags and am using an Intel graphics card. What to do?

Depending on your graphics card model and x.org driver used, several
approaches might work out for you:

Some users [have reported](https://github.com/LibVNC/x11vnc/issues/58)
that when using the 'intel' x.org driver, switching  the driver's
option named "AccelMethod" to "UXA" helped.

Others [reported](https://github.com/LibVNC/x11vnc/issues/100) that
using the 'modesetting' x.org driver caused the lags and switching to
the 'intel' x.org driver helped.

Some [reported](https://github.com/LibVNC/x11vnc/issues/102) that
lags only occured with xfwm as window manager and with x11vnc started
with monitors off, no matter what the x.org driver. In their case, one
of disabling vsync, using 'modesetting' with "AccelMethod" "none" or
disabling compositing all helped.
