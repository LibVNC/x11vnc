# Enhanced TightVNC Viewer (SSVNC: SSL/SSH VNC viewer)

The Enhanced TightVNC Viewer, SSVNC, adds encryption security to VNC
connections.

The package provides a GUI for Windows, Mac OS X, and Unix that
automatically starts up an STUNNEL SSL tunnel for SSL or ssh/plink for
SSH connections to any VNC server, such as x11vnc, and then launches
the VNC Viewer to use the encrypted tunnel.

The x11vnc server has built-in SSL support, however SSVNC can make SSL
encrypted VNC connections to any VNC Server if they are running an SSL
tunnel, such as STUNNEL or socat, at their end. SSVNC's SSH tunnel
will work to any VNC Server host running sshd that you can log into.

The Enhanced TightVNC Viewer package started as a project to add some
patches to the long neglected Unix TightVNC Viewer. However, now the
front-end GUI, encryption, and wrapper scripts features possibly
outweigh the Unix TightVNC Viewer improvements (see the lists below to
compare).

The SSVNC Unix vncviewer can also be run without the SSVNC encryption
GUI as an enhanced replacement for the xvncviewer, xtightvncviewer,
etc., viewers.

In addition to normal SSL, SSVNC also supports the VeNCrypt SSL/TLS
and Vino/ANONTLS encryption extensions to VNC on Unix, Mac OS X, and
Windows. Via the provided SSVNC VeNCrypt bridge, VeNCrypt and ANONTLS
encryption also works with any third party VNC Viewer (e.g. RealVNC,
TightVNC, UltraVNC, etc...) you select via 'Change VNC Viewer'.

The short name for this project is "ssvnc" for SSL/SSH VNC Viewer.
This is the name of the command to start it.

There is a simplified SSH-Only mode (sshvnc). And an even more
simplified Terminal-Services mode (tsvnc) for use with x11vnc on the
remote side.

The tool has many additional features; see the descriptions below.

It is a self-contained bundle, you could carry it around on, say, a
USB memory stick / flash drive for secure VNC viewing from almost any
machine, Unix, Mac OS X, and Windows (and if you create a directory
named "Home" in the toplevel ssvnc directory on the drive your VNC
profiles and certs will be kept there as well). For Unix, there is
also a conventional source tarball to build and install in the normal
way and not use a pre-built bundle.

---

## Announcements

Important: If you created any SSL certificates with SSVNC (or anything
else) on a Debian or Ubuntu system from Sept. 2006 through May 2008,
then those keys are likely extremely weak and can be easily cracked.
The certificate files should be deleted and recreated on a non-Debian
system or an updated one. See
http://www.debian.org/security/2008/dsa-1571 for details. The same
applies to SSH keys.

Please read this information on using SSVNC on workstations with
Untrusted Local Users.

---

## Feature List

Wrapper scripts and a tcl/tk GUI were written to create these features
for Unix, Mac OS X, and Windows:

* SSL support for connections using the bundled stunnel program.
* Automatic SSH connections from the GUI (system ssh is used on Unix
  and MacOS X; bundled plink is used on Windows)
* Ability to Save and Load VNC profiles for different hosts.
* You can also use your own VNC Viewer, e.g. UltraVNC or RealVNC,
  with the SSVNC encryption GUI front-end if you prefer.
* Create or Import SSL Certificates and Private Keys.
* Reverse (viewer listening) VNC connections via SSL and SSH.
* VeNCrypt SSL/TLS VNC encryption support (used by VeNCrypt, QEMU,
  ggi, libvirt/virt-manager/xen, vinagre/gvncviewer/gtk-vnc)
* ANONTLS SSL/TLS VNC encryption support (used by Vino)
* VeNCrypt and ANONTLS are also enabled for any 3rd party VNC Viewer
  (e.g. RealVNC, TightVNC, UltraVNC ...) on Unix, MacOSX, and
  Windows via the provided SSVNC VeNCrypt Viewer Bridge tool (use
  'Change VNC Viewer' to select the one you want.)
* Support for Web Proxies, SOCKS Proxies, and the UltraVNC repeater
  proxy (e.g. repeater://host:port+ID:1234). Multiple proxies may be
  chained together (3 max).
* Support for SSH Gateway connections and non-standard SSH ports.
* Automatic Service tunnelling via SSH for CUPS and SMB Printing,
  ESD/ARTSD Audio, and SMB (Windows/Samba) filesystem mounting.
* Sets up any additional SSH port redirections that you want.
* Zeroconf (aka Bonjour) is used on Unix and Mac OS X to find VNC
  servers on your local network if the avahi-browse or dns-sd
  program is available and in your PATH.
* Port Knocking for "closed port" SSH/SSL connections. In addition
  to a simple fixed port sequence and one-time-pad implementation, a
  hook is also provided to run any port knocking client before
  connecting.
* Support for native MacOS X usage with bundled Chicken of the VNC
  viewer (the Unix X11 viewer is also provided for MacOS X, and is
  better IMHO. It is now the default on MacOS X.)
* Dynamic VNC Server Port determination and redirection (using ssh's
  builtin SOCKS proxy, ssh -D) for servers like x11vnc that print
  out PORT= at startup.
* Unix Username and Password entry for use with "x11vnc -unixpw"
  type login dialogs.
* Simplified mode launched by command "sshvnc" that is SSH Only.
* Simplified mode launched by command "tsvnc" that provides a VNC
  "Terminal Services" mode (uses x11vnc on the remote side).
* IPv6 support for all connection modes on Unix, MacOSX, and
  Windows.

Patches to TightVNC 1.3.9 vnc_unixsrc tree were created for Unix
TightVNC Viewer improvements (these only apply to the Unix VNC viewer,
including MacOSX XQuartz):

* rfbNewFBSize VNC support (dynamic screen resizing)
* Client-side Scaling of the Desktop in the viewer.
* ZRLE VNC encoding support (RealVNC's encoding)
* Support for the ZYWRLE encoding, a wavelet based extension to ZRLE
  to improve compression of motion video and photo regions.
* TurboVNC support (VirtualGL's modified TightVNC encoding; requires
  TurboJPEG library)
* Pipelined Updates of the framebuffer as in TurboVNC (asks for the
  next update before the current one has finished downloading; this
  gives some speedup on high latency connections.)
* Cursor alphablending with x11vnc at 32bpp (-alpha option)
* Option "-unixpw ..." for use with "x11vnc -unixpw" type login
  dialogs.
* Support for UltraVNC extensions: 1/n Server side scaling, Text
  Chat, Single Window, Disable Server-side Input. Both UltraVNC and
  x11vnc servers support these extensions.
* UltraVNC File Transfer via an auxiliary Java helper program (java
  must be in $PATH). Note that the x11vnc server also supports
  UltraVNC file transfer.
* Connection support for the UltraVNC repeater proxy (-repeater
  option).
* Support for UltraVNC Single Click operation. (both unencrypted: SC
  I, and SSL encrypted: SC III)
* Support for UltraVNC DSM Encryption Plugin symmetric encryption
  mode. (ARC4, AESV2, MSRC4, and SecureVNC)
* Support for UltraVNC MS-Logon authentication (NOTE: the UltraVNC
  MS-Logon key exchange implementation is very weak; an eavesdropper
  on the network can recover your Windows password easily in a few
  seconds; you need to use an additional encrypted tunnel with
  MS-Logon.)
* Support for symmetric encryption (including blowfish and 3des
  ciphers) to Non-UltraVNC Servers. Any server using the same
  encryption method will work, e.g.:  x11vnc -enc blowfish:./my.key
* Instead of hostname:display one can also supply "exec=command
  args..." to connect the viewer to the stdio of an external command
  (e.g. stunnel or socat) rather than using a TCP/IP socket. Unix
  domain sockets, e.g. /path/to/unix/socket, and a previously opened
  file descriptor fd=0, work too.
* Local Port Protections for STUNNEL and SSH: avoid having for long
  periods of time a listening port on the the local (VNC viewer)
  side that redirects to the remote side.
* Reverse (viewer listening) VNC connections can show a Popup dialog
  asking whether to accept the connection or not (-acceptpopup.) The
  extra info provided by UltraVNC Single Click reverse connections
  is also supported (-acceptpopupsc)
* Extremely low color modes: 64 and 8 colors in 8bpp
  (-use64/-bgr222, -use8/-bgr111)
* Medium color mode: 16bpp mode on a 32bpp Viewer display
  (-16bpp/-bgr565)
* For use with x11vnc's client-side caching -ncache method use the
  cropping option -ycrop n. This will "hide" the large pixel buffer
  cache below the actual display. Set to the actual height or use -1
  for autodetection (also, tall screens, H > 2*W, are autodetected
  by default).
* Escape Keys: specify a set of modifier keys so that when they are
  all pressed down you can invoke Popup menu actions via keystrokes.
  I.e., a set of 'Hot Keys'. One can also pan (move) the desktop
  inside the viewport via Arrow keys or a mouse drag.
* Scrollbar width setting: -sbwidth n, the default is very thin, 2
  pixels, for less distracting -ycrop usage.
* Selection text sending and receiving can be fine-tuned with the
  -sendclipboard, -sendalways, and -recvtext options.
* TightVNC compression and quality levels are automatically set
  based on observed network latency (n.b. not bandwidth.)
* Improvements to the Popup menu, all of these can now be changed
  dynamically via the menu: ViewOnly, Toggle Bell, CursorShape
  updates, X11 Cursor, Cursor Alphablending, Toggle Tight/ZRLE,
  Toggle JPEG, FullColor/16bpp/8bpp (256/64/8 colors), Greyscale for
  low color modes, Scaling the Viewer resolution, Escape Keys,
  Pipeline Updates, and others, including UltraVNC extensions.
* Maintains its own BackingStore if the X server does not.
* The default for localhost:0 connections is not raw encoding since
  same-machine connections are pretty rare. Default assumes you are
  using a SSL or SSH tunnel. Use -rawlocal to revert.
* XGrabServer support for fullscreen mode, for old window managers
  (-grab/-graball option).
* Fix for Popup menu positioning for old window managers (-popupfix
  option).
* The VNC Viewer ssvncviewer supports IPv6 natively (no helpers
  needed.)

The list of 3rd party software bundled in the archive files:

* TightVNC Viewer  (windows, unix, macosx)
* Chicken of the VNC Viewer  (macosx)
* Stunnel  (windows, unix, macosx)
* Putty/Plink/Pageant  (windows)
* OpenSSL  (windows)
* esound  (windows)

These are all self-contained in the bundle directory: they will not be
installed on your system. Just un-zip or un-tar the file you
downloaded and run the frontend ssvnc straight from its directory.
Alternatively, on Unix you can use the conventional source tarball.

---

## Here is the Quick Start info from the README for how to setup and use SSVNC:

```
Quick Start:
-----------

Unix and Mac OS X:

    Inside a Terminal do something like the following.

    Unpack the archive:

        % gzip -dc ssvnc-1.0.29.tar.gz | tar xvf -

    Run the GUI:

        % ./ssvnc/Unix/ssvnc               (for Unix)

        % ./ssvnc/MacOSX/ssvnc             (for Mac OS X)

    The smaller file "ssvnc_no_windows-1.0.29.tar.gz"
    could have been used as well.

    On MacOSX you could also click on the SSVNC app icon in the Finder.

    On MacOSX if you don't like the Chicken of the VNC (e.g. no local
    cursors, no screen size rescaling, and no password prompting), and you
    have the XDarwin X server installed, you can set DISPLAY before starting
    ssvnc (or type DISPLAY=... in Host:Disp and hit Return).  Then our
    enhanced TightVNC viewer will be used instead of COTVNC.
    Update: there is now a 'Use X11 vncviewer on MacOSX' under Options ...


    If you want a SSH-only tool (without the distractions of SSL) run
    the command:

                sshvnc

    instead of "ssvnc".  Or click "SSH-Only Mode" under Options.
    Control-h will toggle between the two modes.


    If you want a simple VNC Terminal Services only mode (requires x11vnc
    on the remote server) run the command:

                tsvnc

    instead of "ssvnc".  Or click "Terminal Services" under Options.
    Control-t will toggle between the two modes.

    "tsvnc profile-name" and "tsvnc user@hostname" work too.


Unix/MacOSX Install:

    There is no standard install for the bundles, but you can make
    symlinks like so:

        cd /a/directory/in/PATH
        ln -s /path/to/ssvnc/bin/{s,t}* .

    Or put /path/to/ssvnc/bin, /path/to/ssvnc/Unix, or /path/to/ssvnc/MacOSX
    in your PATH.

    For the conventional source tarball it will compile and install, e.g.:

       gzip -dc ssvnc-1.0.29.src.tar.gz | tar xvf -
       cd ssvnc-1.0.29
       make config
       make all
       make PREFIX=/my/install/dir install

    then have /my/install/dir/bin in your PATH.



Windows:

    Unzip, using WinZip or a similar utility, the zip file:

        ssvnc-1.0.29.zip

    Run the GUI, e.g.:

        Start -> Run -> Browse

    and then navigate to

        .../ssvnc/Windows/ssvnc.exe

    select Open, and then OK to launch it.

    The smaller file "ssvnc_windows_only-1.0.29.zip"
    could have been used as well.

    You can make a Windows shortcut to this program if you want to.

    See the Windows/README.txt for more info.


    If you want a SSH-only tool (without the distractions of SSL) run
    the command:

                sshvnc.bat

    Or click "SSH-Only Mode" under Options.


    If you want a simple VNC Terminal Services only mode (requires x11vnc
    on the remote server) run the command:

                tsvnc.bat

    Or click "Terminal Services" under Options.  Control-t will toggle
    between the two modes.  "tsvnc profile-name" and "tsvnc user@hostname"
    work too.
```

---

You can read all of the SSVNC GUI's Online Help Text here.

---

The bundle unpacks a directory/folder named: ssvnc. It contains these
programs to launch the GUI:

```
Windows/ssvnc.exe        for Windows
MacOSX/ssvnc             for Mac OS X
Unix/ssvnc               for Unix
```

(the Mac OS X and Unix launchers are simply links to the bin
directory). See the README for more information.

The SSH-Only mode launcher program has name sshvnc. The Terminal
Services mode launcher program (assumes x11vnc 0.8.4 or later and Xvfb
installed on the server machine) has name tsvnc.

The Viewer SSL support is done via a wrapper script (bin/ssvnc_cmd
that calls bin/util/ss_vncviewer) that starts up the STUNNEL tunnel
first and then starts the TightVNC viewer pointed at that tunnel. The
bin/ssvnc program is a GUI front-end to that script. See this FAQ for
more details on SSL tunnelling. In SSH connection mode, the wrappers
start up SSH appropriately.

## Memory Stick Usage

If you create a directory named "Home" in that
toplevel ssvnc directory then that will be used as the base for
storing VNC profiles and certificates. Also, for convenience, if you
first run the command with "." as an argument (e.g. "ssvnc .") it will
automatically create the "Home" directory for you. This is handy if
you want to place SSVNC on a USB flash drive that you carry around for
mobile use and you want the profiles you create to stay with the drive
(otherwise you'd have to browse to the drive directory each time you
load or save).

One user on Windows created a BAT file to launch SSVNC and needed to
do this to get the Home directory correct:

```
cd \ssvnc\Windows
start \ssvnc\Windows\ssvnc.exe
```

(an optional profile name can be supplied to the ssvnc.exe line)

WARNING: if you use ssvnc from an "Internet Cafe", i.e. some untrusted
computer, please be aware that someone may have set up that machine to
be capturing your keystrokes, etc.


## SSH-Only version

The command "sshvnc" can be run instead of "ssvnc"
to get an SSH-only version of the tool:

[sshvnc.gif]

These also work: "sshvnc myprofile" and "sshvnc user@hostname". To
switch from the regular SSVNC mode, click "SSH-Only Mode" under
Options. This mode is less distracting if you never plan to use SSL,
manage certificates, etc.


Terminal Services Only: The command "tsvnc" can be run instead of
"ssvnc" to get a "Terminal Services" only version of the tool:

[tsvnc.gif]

These also work: "tsvnc myprofile" and "tsvnc user@hostname". To
switch from the regular SSVNC mode, click "Terminal Services" under
Options.

This mode requires x11vnc (0.9.3 or later) installed on the remote
machine to find, create, and manage the user sessions. SSH is used to
create the encrypted and authenticated tunnel. The Xvfb (virtual
framebuffer X server) program must also be installed on the remote
system. However tsvnc will also connect to a real X session (i.e. on
the physical hardware) if you are already logged into the X session;
this is a useful access mode and does not require Xvfb on the remote
system.

This mode should be very easy for beginner users to understand and
use. On the remote end you only need to have x11vnc and Xvfb available
in $PATH, and on the local end you just run something like:
tsvnc myname@myhost.com

(or start up the tsvnc GUI first and then enter myname@myhost.com and
press "Connect").

Normally the Terminal Services sessions created are virtual (RAM-only)
ones (e.g. Xvfb, Xdummy, or Xvnc), however a nice feature is if you
have a regular X session (i.e displaying on the physical hardware) on
the remote machine that you are ALREADY logged into, then the x11vnc
run from tsvnc will find it for you as well.

Also, there is setting "X Login" under Advanced Options that allows
you to attach to a real X server with no one logged in yet (i.e.
XDM/GDM/KDM Login Greeter screen) as long as you have sudo(1)
permission on the remote machine.

Nice features to soon to be added to the tsvnc mode are easy CUPS
printing (working fairly well) and Sound redirection (needs much work)
of the Terminal Services Desktop session. It is easier in tsvnc mode
because the entire desktop session can be started with the correct
environment. ssvnc tries to handle the general case of an already
started desktop and that is more difficult.


## Proxies

Web proxies, SOCKS proxies, and the UltraVNC repeater proxy
are supported to allow the SSVNC connection to go through the proxy to
the otherwise unreachable VNC Server. SSH gateway machines can be used
in the same way. Read more about SSVNC proxy support here.


## Dynamic VNC Server Port determination

If you are running SSVNC on
Unix and are using SSH to start the remote VNC server and the VNC
server prints out the line "PORT=NNNN" to indicate which dynamic port
it is using (x11vnc does this), then if you prefix the SSH command
with "PORT=" SSVNC will watch for the PORT=NNNN line and uses ssh's
built in SOCKS proxy (ssh -D ...) to connect to the dynamic VNC server
port through the SSH tunnel. For example:

```
VNC Host:Display     user@somehost.com
Remote SSH Command:  PORT= x11vnc -find
```

or "PORT= x11vnc -display :0 -localhost", etc. Or use "P= x11vnc ..."

There is also code to detect the display of the regular Unix
vncserver(1). It extracts the display (and hence port) from the lines
"New 'X' desktop is hostname:4" and also "VNC server is already
running as :4". So you can use something like:

```
        PORT= vncserver; sleep 15
or:     PORT= vncserver :4; sleep 15
```

the latter is preferred because when you reconnect with it will find
the already running one. The former one will keep creating new X
sessions if called repeatedly.

If you use PORT= on Windows, a large random port is selected instead
and the -rfbport option is passed to x11vnc (it does not work with
vncserver).


## Patches for Unix Tightvnc viewer

The rfbNewFBSize support allows the enhanced TightVNC Unix viewer to
resize when the server does (e.g. "x11vnc -R scale=3/4" remote control
command).

The cursor alphablending is described here.

The RealVNC ZRLE encoding is supported, in addition to some low colors
modes (16bpp and 8bpp at 256, 64, and even 8 colors, for use on very
slow connections). Greyscales are also enabled for the low color
modes.

The Popup menu (F8) is enhanced with the ability to change many things
on the fly. F9 is added as a shortcut to toggle FullScreen mode.

## Client Side Caching

The x11vnc client-side caching is handled nicely
by this viewer. The very large pixel cache below the actual display in
this caching method is distracting. Our Unix VNC viewer will
automatically try to autodetect the actual display height if the
framebuffer is very tall (more than twice as high as it is wide). One
can also set the height to the known value via -ycrop n, or use -ycrop
-1 to force autodection. In fullscreen mode one is not possible to
scroll down to the pixel cache region. In non-fullscreen mode the
window manager frame is "shrink-wrapped" around the actual screen
display. You can still scroll down to the pixel cache region. The
scrollbars are set to be very thin (2 pixels) to be less distracting.
Use the -sbwidth n to make them wider.

Probably nobody is interested in the grabserver patch for old window
managers when the viewer is in fullscreen mode... This and some other
unfixed bugs have been fixed in our patches (fullscreen toggle works
with KDE, -x11cursor has been fixed, and the dot cursor has been made
smaller).

From the -help output:

```
SSVNC Viewer (based on TightVNC viewer version 1.3.9)

Usage: vncviewer [<OPTIONS>] [<HOST>][:<DISPLAY#>]
       vncviewer [<OPTIONS>] [<HOST>][::<PORT#>]
       vncviewer [<OPTIONS>] exec=[CMD ARGS...]
       vncviewer [<OPTIONS>] fd=n
       vncviewer [<OPTIONS>] /path/to/unix/socket
       vncviewer [<OPTIONS>] -listen [<DISPLAY#>]
       vncviewer -help

<OPTIONS> are standard Xt options, or:
        -via <GATEWAY>
        -shared (set by default)
        -noshared
        -viewonly
        -fullscreen
        -noraiseonbeep
        -passwd <PASSWD-FILENAME> (standard VNC authentication)
        -user <USERNAME> (Unix login authentication)
        -encodings <ENCODING-LIST> (e.g. "tight,copyrect")
        -bgr233
        -owncmap
        -truecolour
        -depth <DEPTH>
        -compresslevel <COMPRESS-VALUE> (0..9: 0-fast, 9-best)
        -quality <JPEG-QUALITY-VALUE> (0..9: 0-low, 9-high)
        -nojpeg
        -nocursorshape
        -x11cursor
        -autopass

Option names may be abbreviated, e.g. -bgr instead of -bgr233.
See the manual page for more information.


Enhanced TightVNC viewer (SSVNC) options:

   URL http://www.karlrunge.com/x11vnc/ssvnc.html

   Note: ZRLE and ZYWRLE encodings are now supported.

   Note: F9 is shortcut to Toggle FullScreen mode.

   Note: In -listen mode set the env var. SSVNC_MULTIPLE_LISTEN=1
         to allow more than one incoming VNC server at a time.
         This is the same as -multilisten described below.  Set
         SSVNC_MULTIPLE_LISTEN=MAX:n to allow no more than "n"
         simultaneous reverse connections.

   Note: If the host:port is specified as "exec=command args..."
         then instead of making a TCP/IP socket connection to the
         remote VNC server, "command args..." is executed and the
         viewer is attached to its stdio.  This enables tunnelling
         established via an external command, e.g. an stunnel(8)
         that does not involve a listening socket.  This mode does
         not work for -listen reverse connections.

         If the host:port is specified as "fd=n" then it is assumed
         n is an already opened file descriptor to the socket. (i.e
         the parent did fork+exec)

         If the host:port contains a '/' it is interpreted as a
         unix-domain socket (AF_LOCAL insead of AF_INET)

        -multilisten  As in -listen (reverse connection listening) except
                    allow more than one incoming VNC server to be connected
                    at a time.  The default for -listen of only one at a
                    time tries to play it safe by not allowing anyone on
                    the network to put (many) desktops on your screen over
                    a long window of time. Use -multilisten for no limit.

        -acceptpopup  In -listen (reverse connection listening) mode when
                    a reverse VNC connection comes in show a popup asking
                    whether to Accept or Reject the connection.  The IP
                    address of the connecting host is shown.  Same as
                    setting the env. var. SSVNC_ACCEPT_POPUP=1.

        -acceptpopupsc  As in -acceptpopup except assume UltraVNC Single
                    Click (SC) server.  Retrieve User and ComputerName
                    info from UltraVNC Server and display in the Popup.

        -use64      In -bgr233 mode, use 64 colors instead of 256.
        -bgr222     Same as -use64.

        -use8       In -bgr233 mode, use 8 colors instead of 256.
        -bgr111     Same as -use8.

        -16bpp      If the vnc viewer X display is depth 24 at 32bpp
                    request a 16bpp format from the VNC server to cut
                    network traffic by up to 2X, then tranlate the
                    pixels to 32bpp locally.
        -bgr565     Same as -16bpp.

        -grey       Use a grey scale for the 16- and 8-bpp modes.

        -alpha      Use alphablending transparency for local cursors
                    requires: x11vnc server, both client and server
                    must be 32bpp and same endianness.

        -scale str  Scale the desktop locally.  The string "str" can
                    a floating point ratio, e.g. "0.9", or a fraction,
                    e.g. "3/4", or WxH, e.g. 1280x1024.  Use "fit"
                    to fit in the current screen size.  Use "auto" to
                    fit in the window size.  "str" can also be set by
                    the env. var. SSVNC_SCALE.

                    If you observe mouse trail painting errors, enable
                    X11 Cursor mode (either via Popup or -x11cursor.)

                    Note that scaling is done in software and so can be
                    slow and requires more memory.  Some speedup Tips:

                        ZRLE is faster than Tight in this mode.  When
                        scaling is first detected, the encoding will
                        be automatically switched to ZRLE.  Use the
                        Popup menu if you want to go back to Tight.
                        Set SSVNC_PRESERVE_ENCODING=1 to disable this.

                        Use a solid background on the remote side.
                        (e.g. manually or via x11vnc -solid ...)

                        If the remote server is x11vnc, try client
                        side caching: x11vnc -ncache 10 ...

        -ycrop n    Only show the top n rows of the framebuffer.  For
                    use with x11vnc -ncache client caching option
                    to help "hide" the pixel cache region.
                    Use a negative value (e.g. -1) for autodetection.
                    Autodetection will always take place if the remote
                    fb height is more than 2 times the width.

        -sbwidth n  Scrollbar width for x11vnc -ncache mode (-ycrop),
                    default is very narrow: 2 pixels, it is narrow to
                    avoid distraction in -ycrop mode.

        -nobell     Disable bell.

        -rawlocal   Prefer raw encoding for localhost, default is
                    no, i.e. assumes you have a SSH tunnel instead.

        -notty      Try to avoid using the terminal for interactive
                    responses: use windows for messages and prompting
                    instead.  Messages will also be printed to terminal.

        -sendclipboard  Send the X CLIPBOARD selection (i.e. Ctrl+C,
                        Ctrl+V) instead of the X PRIMARY selection (mouse
                        select and middle button paste.)

        -sendalways     Whenever the mouse enters the VNC viewer main
                        window, send the selection to the VNC server even if
                        it has not changed.  This is like the Xt resource
                        translation SelectionToVNC(always)

        -recvtext str   When cut text is received from the VNC server,
                        ssvncviewer will set both the X PRIMARY and the
                        X CLIPBOARD local selections.  To control which
                        is set, specify 'str' as 'primary', 'clipboard',
                        or 'both' (the default.)

        -graball    Grab the entire X server when in fullscreen mode,
                    needed by some old window managers like fvwm2.

        -popupfix   Warp the popup back to the pointer position,
                    needed by some old window managers like fvwm2.
        -sendclipboard  Send the X CLIPBOARD selection (i.e. Ctrl+C,
                        Ctrl+V) instead of the X PRIMARY selection (mouse
                        select and middle button paste.)

        -sendalways     Whenever the mouse enters the VNC viewer main
                        window, send the selection to the VNC server even if
                        it has not changed.  This is like the Xt resource
                        translation SelectionToVNC(always)

        -recvtext str   When cut text is received from the VNC server,
                        ssvncviewer will set both the X PRIMARY and the
                        X CLIPBOARD local selections.  To control which
                        is set, specify 'str' as 'primary', 'clipboard',
                        or 'both' (the default.)

        -graball    Grab the entire X server when in fullscreen mode,
                    needed by some old window managers like fvwm2.

        -popupfix   Warp the popup back to the pointer position,
                    needed by some old window managers like fvwm2.

        -grabkbd    Grab the X keyboard when in fullscreen mode,
                    needed by some window managers. Same as -grabkeyboard.
                    -grabkbd is the default, use -nograbkbd to disable.

        -bs, -nobs  Whether or not to use X server Backingstore for the
                    main viewer window.  The default is to not, mainly
                    because most Linux, etc, systems X servers disable
                    *all* Backingstore by default.  To re-enable it put

                        Option "Backingstore"

                    in the Device section of /etc/X11/xorg.conf.
                    In -bs mode with no X server backingstore, whenever an
                    area of the screen is re-exposed it must go out to the
                    VNC server to retrieve the pixels. This is too slow.

                    In -nobs mode, memory is allocated by the viewer to
                    provide its own backing of the main viewer window. This
                    actually makes some activities faster (changes in large
                    regions) but can appear to "flash" too much.

        -noshm      Disable use of MIT shared memory extension (not recommended)

        -termchat   Do the UltraVNC chat in the terminal vncviewer is in
                    instead of in an independent window.

        -unixpw str Useful for logging into x11vnc in -unixpw mode. "str" is a
                    string that allows many ways to enter the Unix Username
                    and Unix Password.  These characters: username, newline,
                    password, newline are sent to the VNC server after any VNC
                    authentication has taken place.  Under x11vnc they are
                    used for the -unixpw login.  Other VNC servers could do
                    something similar.

                    You can also indicate "str" via the environment
                    variable SSVNC_UNIXPW.

                    Note that the Escape key is actually sent first to tell
                    x11vnc to not echo the Unix Username back to the VNC
                    viewer. Set SSVNC_UNIXPW_NOESC=1 to override this.

                    If str is ".", then you are prompted at the command line
                    for the username and password in the normal way.  If str is
                    "-" the stdin is read via getpass(3) for username@password.
                    Otherwise if str is a file, it is opened and the first line
                    read is taken as the Unix username and the 2nd as the
                    password. If str prefixed by "rm:" the file is removed
                    after reading. Otherwise, if str has a "@" character,
                    it is taken as username@password. Otherwise, the program
                    exits with an error. Got all that?

     -repeater str  This is for use with UltraVNC repeater proxy described
                    here: http://www.uvnc.com/addons/repeater.html.  The "str"
                    is the ID string to be sent to the repeater.  E.g. ID:1234
                    It can also be the hostname and port or display of the VNC
                    server, e.g. 12.34.56.78:0 or snoopy.com:1.  Note that when
                    using -repeater, the host:dpy on the cmdline is the repeater
                    server, NOT the VNC server.  The repeater will connect you.

                    Example: vncviewer ... -repeater ID:3333 repeat.host:5900
                    Example: vncviewer ... -repeater vhost:0 repeat.host:5900

                    Use, e.g., '-repeater SCIII=ID:3210' if the repeater is a
                    Single Click III (SSL) repeater (repeater_SSL.exe) and you
                    are passing the SSL part of the connection through stunnel,
                    socat, etc. This way the magic UltraVNC string 'testB'
                    needed to work with the repeater is sent to it.

     -rfbversion str Set the advertised RFB version.  E.g.: -rfbversion 3.6
                    For some servers, e.g. UltraVNC this needs to be done.

     -ultradsm      UltraVNC has symmetric private key encryption DSM plugins:
                    http://www.uvnc.com/features/encryption.html. It is assumed
                    you are using a unix program (e.g. our ultravnc_dsm_helper)
                    to encrypt and decrypt the UltraVNC DSM stream. IN ADDITION
                    TO THAT supply -ultradsm to tell THIS viewer to modify the
                    RFB data sent so as to work with the UltraVNC Server. For
                    some reason, each RFB msg type must be sent twice under DSM.

     -mslogon user  Use Windows MS Logon to an UltraVNC server.  Supply the
                    username or "1" to be prompted.  The default is to
                    autodetect the UltraVNC MS Logon server and prompt for
                    the username and password.

                    IMPORTANT NOTE: The UltraVNC MS-Logon Diffie-Hellman
                    exchange is very weak and can be brute forced to recover
                    your username and password in a few seconds of CPU time.
                    To be safe, be sure to use an additional encrypted tunnel
                    (e.g. SSL or SSH) for the entire VNC session.

     -chatonly      Try to be a client that only does UltraVNC text chat. This
                    mode is used by x11vnc to present a chat window on the
                    physical X11 console (i.e. chat with the person at the
                    display).

     -env VAR=VALUE To save writing a shell script to set environment variables,
                    specify as many as you need on the command line.  For
                    example, -env SSVNC_MULTIPLE_LISTEN=MAX:5 -env EDITOR=vi

     -noipv6        Disable all IPv6 sockets.  Same as VNCVIEWER_NO_IPV6=1.

     -noipv4        Disable all IPv4 sockets.  Same as VNCVIEWER_NO_IPV4=1.

     -printres      Print out the Ssvnc X resources (appdefaults) and then exit
                    You can save them to a file and customize them (e.g. the
                    keybindings and Popup menu)  Then point to the file via
                    XENVIRONMENT or XAPPLRESDIR.

     -pipeline      Like TurboVNC, request the next framebuffer update as soon
                    as possible instead of waiting until the end of the current
                    framebuffer update coming in.  Helps 'pipeline' the updates.
                    This is currently the default, use -nopipeline to disable.

     -appshare      Enable features for use with x11vnc's -appshare mode where
                    instead of sharing the full desktop only the application's
                    windows are shared.  Viewer multilisten mode is used to
                    create the multiple windows: -multilisten is implied.
                    See 'x11vnc -appshare -help' more information on the mode.

                    Features enabled in the viewer under -appshare are:
                    Minimum extra text in the title, auto -ycrop is disabled,
                    x11vnc -remote_prefix X11VNC_APPSHARE_CMD: message channel,
                    x11vnc initial window position hints.  See also Escape Keys
                    below for additional key and mouse bindings.

     -escape str    This sets the 'Escape Keys' modifier sequence and enables
                    escape keys mode.  When the modifier keys escape sequence
                    is held down, the next keystroke is interpreted locally
                    to perform a special action instead of being sent to the
                    remote VNC server.

                    Use '-escape default' for the default modifier sequence.
                    (Unix: Alt_L,Super_L and MacOSX: Control_L,Meta_L)

    Here are the 'Escape Keys: Help+Set' instructions from the Popup Menu:

    Escape Keys:  Enter a comma separated list of modifier keys to be the
    'escape sequence'.  When these keys are held down, the next keystroke is
    interpreted locally to invoke a special action instead of being sent to
    the remote VNC server.  In other words, a set of 'Hot Keys'.

    To enable or disable this, click on 'Escape Keys: Toggle' in the Popup.

    Here is the list of hot-key mappings to special actions:

       r: refresh desktop  b: toggle bell   c: toggle full-color
       f: file transfer    x: x11cursor     z: toggle Tight/ZRLE
       l: full screen      g: graball       e: escape keys dialog
       s: scale dialog     +: scale up (=)  -: scale down (_)
       t: text chat                         a: alphablend cursor
       V: toggle viewonly  Q: quit viewer   1 2 3 4 5 6: UltraVNC scale 1/n

       Arrow keys:         pan the viewport about 10% for each keypress.
       PageUp / PageDown:  pan the viewport by a screenful vertically.
       Home   / End:       pan the viewport by a screenful horizontally.
       KeyPad Arrow keys:  pan the viewport by 1 pixel for each keypress.
       Dragging the Mouse with Button1 pressed also pans the viewport.
       Clicking Mouse Button3 brings up the Popup Menu.

    The above mappings are *always* active in ViewOnly mode, unless you set the
    Escape Keys value to 'never'.

    If the Escape Keys value below is set to 'default' then a default list of
    of modifier keys is used.  For Unix it is: Alt_L,Super_L and for MacOSX it
    is Control_L,Meta_L.  Note: the Super_L key usually has a Windows(TM) Flag
    on it.  Also note the _L and _R mean the key is on the LEFT or RIGHT side
    of the keyboard.

    On Unix   the default is Alt and Windows keys on Left side of keyboard.
    On MacOSX the default is Control and Command keys on Left side of keyboard.

    Example: Press and hold the Alt and Windows keys on the LEFT side of the
    keyboard and then press 'c' to toggle the full-color state.  Or press 't'
    to toggle the ultravnc Text Chat window, etc.

    To use something besides the default, supply a comma separated list (or a
    single one) from: Shift_L Shift_R Control_L Control_R Alt_L Alt_R Meta_L
    Meta_R Super_L Super_R Hyper_L Hyper_R or Mode_switch.


   New Popup actions:

        ViewOnly:                ~ -viewonly
        Disable Bell:            ~ -nobell
        Cursor Shape:            ~ -nocursorshape
        X11 Cursor:              ~ -x11cursor
        Cursor Alphablend:       ~ -alpha
        Toggle Tight/Hextile:    ~ -encodings hextile...
        Toggle Tight/ZRLE:       ~ -encodings zrle...
        Toggle ZRLE/ZYWRLE:      ~ -encodings zywrle...
        Quality Level            ~ -quality (both Tight and ZYWRLE)
        Compress Level           ~ -compresslevel
        Disable JPEG:            ~ -nojpeg  (Tight)
        Pipeline Updates         ~ -pipeline

        Full Color                 as many colors as local screen allows.
        Grey scale (16 & 8-bpp)  ~ -grey, for low colors 16/8bpp modes only.
        16 bit color (BGR565)    ~ -16bpp / -bgr565
        8  bit color (BGR233)    ~ -bgr233
        256 colors               ~ -bgr233 default # of colors.
         64 colors               ~ -bgr222 / -use64
          8 colors               ~ -bgr111 / -use8
        Scale Viewer             ~ -scale
        Escape Keys: Toggle      ~ -escape
        Escape Keys: Help+Set    ~ -escape
        Set Y Crop (y-max)       ~ -ycrop
        Set Scrollbar Width      ~ -sbwidth
        XGrabServer              ~ -graball

        UltraVNC Extensions:

          Set 1/n Server Scale     Ultravnc ext. Scale desktop by 1/n.
          Text Chat                Ultravnc ext. Do Text Chat.
          File Transfer            Ultravnc ext. File xfer via Java helper.
          Single Window            Ultravnc ext. Grab and view a single window.
                                   (select then click on the window you want).
          Disable Remote Input     Ultravnc ext. Try to prevent input and
                                   viewing of monitor at physical display.

        Note: the Ultravnc extensions only apply to servers that support
              them.  x11vnc/libvncserver supports some of them.

        Send Clipboard not Primary  ~ -sendclipboard
        Send Selection Every time   ~ -sendalways
```

Nearly all of these can be changed dynamically in the Popup menu
(press F8 for it):

[viewer_menu.gif] [unixviewer.jpg]

---

## Windows

For Windows, SSL Viewer support is provided by a GUI Windows/ssvnc.exe
that prompts for the VNC display and then starts up STUNNEL followed
by the Stock TightVNC Windows Viewer. Both are bundled in the package
for your convenience. The GUI has other useful features. When the
connection is finished, you will be asked if you want to terminate the
STUNNEL program. For SSH connections from Windows the GUI will use
PLINK instead of STUNNEL.

## Unix and Mac OS X

Run the GUI (ssvnc, see above) and let me know how it goes.

---

Hopefully this tool will make it convenient for people to help test
and use the built-in SSL support in x11vnc. Extra testing of this
feature is much appreciated!! Thanks.

Please Help Test the newly added features:

* Automatic Service tunnelling via SSH for CUPS and SMB Printing
* ESD/ARTSD Audio
* SMB (Windows/Samba) filesystem mounting

These allow you to print from the remote (VNC Server) machine to local
printers, listen to sounds (with some limitations) from the remote VNC
Server machine, and to mount your local Windows or Samba shares on the
remote VNC Server machine. Basically these new features try to
automate the tricks described here:

* http://www.karlrunge.com/x11vnc/faq.html#faq-smb-shares
* http://www.karlrunge.com/x11vnc/faq.html#faq-cups
* http://www.karlrunge.com/x11vnc/faq.html#faq-sound

---

## Downloading

Downloads for this project are hosted at Sourceforge.net.

Choose the archive file bundle that best suits you (e.g. no source
code, windows only, unix only, zip, tar etc).

A quick guide:

* On some flavor of Unix, e.g. Linux or Solaris? Use "ssvnc_unix_only" (or "ssvnc_no_windows" to recompile).
* On Mac OS X? Use "ssvnc_no_windows".
* On Windows? Use "ssvnc_windows_only".

```
ssvnc_windows_only-1.0.28.zip      Windows Binaries Only.  No source included (6.2MB)
ssvnc_no_windows-1.0.28.tar.gz     Unix and Mac OS X Only. No Windows binaries.  Source included. (10.1MB)
ssvnc_unix_only-1.0.28.tar.gz      Unix Binaries Only.     No source included. (7.2MB)
ssvnc_unix_minimal-1.0.28.tar.gz   Unix Minimal.  You must supply your own vncviewer and stunnel. (0.2MB)

ssvnc-1.0.28.tar.gz                All Unix, Mac OS X, and Windows binaries and source TGZ. (16.1MB)
ssvnc-1.0.28.zip                   All Unix, Mac OS X, and Windows binaries and source ZIP. (16.4MB)
ssvnc_all-1.0.28.zip               All Unix, Mac OS X, and Windows binaries and source AND full archives in the zip dir. (19.2MB)
```

Here is a conventional source tarball:

```
ssvnc-1.0.28.src.tar.gz            Conventional Source for SSVNC GUI and Unix VNCviewer  (0.5MB)
```

it will be of use to those who do not want the SSVNC
"one-size-fits-all" bundles. For example, package/distro maintainers
will find this more familiar and useful to them (i.e. they run: "make
config; make all; make install"). Note that it does not include the
stunnel source, and so has a dependency that the system stunnel is
installed.

Read the README.src file for more information on using the
conventional source tarball.


Note: even with the Unix bundles, e.g. "ssvnc_no_windows" or
"ssvnc_all", you may need to run the "./build.unix" script in the top
directory to recompile for your operating system.

Here are the corresponding 1.0.29 development bundles (Please help
test them):

```
ssvnc_windows_only-1.0.29.zip
ssvnc_no_windows-1.0.29.tar.gz
ssvnc_unix_only-1.0.29.tar.gz
ssvnc_unix_minimal-1.0.29.tar.gz

ssvnc-1.0.29.tar.gz
ssvnc-1.0.29.zip
ssvnc_all-1.0.29.zip

ssvnc-1.0.29.src.tar.gz            Conventional Source for SSVNC GUI and Unix VNCviewer  (0.5MB)
```

For any Unix system, a self-extracting and running file for the
"ssvnc_unix_minimal" package is here: ssvnc. Save it as filename
"ssvnc", type "chmod 755 ./ssvnc", and then launch the GUI via typing
"./ssvnc". Note that this "ssvnc_unix_minimal" mode requires you
install the "stunnel" and "vncviewer" programs externally (for
example, install your distros' versions, e.g. on debian: "apt-get
install stunnel4 xtightvncviewer".) It will work, but many of the
SSVNC features will be missing.

## Previous releases

* Release 1.0.18 at Sourceforge.net
* Release 1.0.19 at Sourceforge.net
* Release 1.0.20 at Sourceforge.net
* Release 1.0.21 at Sourceforge.net
* Release 1.0.22 at Sourceforge.net
* Release 1.0.23 at Sourceforge.net
* Release 1.0.24 at Sourceforge.net
* Release 1.0.25 at Sourceforge.net
* Release 1.0.26 at Sourceforge.net
* Release 1.0.27 at Sourceforge.net
* Release 1.0.28 at Sourceforge.net


Please help test the UltraVNC File Transfer support in the native Unix
VNC viewer! Let us know how it went.

Current Unix binaries in the archives:

```
Linux.i686
Linux.x86_64
Linux.ppc64    X (removed)
Linux.alpha    X (removed)
SunOS.sun4u
SunOS.sun4m
SunOS.i86pc
Darwin.Power.Macintosh
Darwin.i386
HP-UX.9000     X (removed)
FreeBSD.i386   X (removed)
NetBSD.i386    X (removed)
OpenBSD.i386   X (removed)
```

(some of these are out of date, marked with 'X' above, because I no
longer have access to machines running those OS's. Use the
"build.unix" script to recompile on your system).

Note: some of the above binaries depend on libssl.so.0.9.7, whereas
some recent distros only provide libssl.so.0.9.8 by default (for
compatibility reasons they should install both by default but not all
do). So you may need to instruct your distro to install the 0.9.7
library (it is fine to have both runtimes installed simultaneously
since the libraries have different names). Update: I now try to
statically link libssl.a for all of the binaries in the archive.

You can also run the included build.unix script to try to
automatically build the binaries if your OS is not in the above list
or the included binary does not run properly on your system. Let me
know how that goes.

---

## IMPORTANT

there may be restrictions for you to download, use, or
redistribute the above because of cryptographic software they contain
or for other reasons. Please check out your situation and information
at the following and related sites:

```
http://stunnel.mirt.net
http://www.stunnel.org
http://www.openssl.org
http://www.chiark.greenend.org.uk/~sgtatham/putty/
http://www.tightvnc.com
http://www.realvnc.com
http://sourceforge.net/projects/cotvnc/
```
