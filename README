[![Build Status](https://travis-ci.org/LibVNC/x11vnc.svg?branch=master)](https://travis-ci.org/LibVNC/x11vnc)

**❗ The project currently is unmaintained and is looking for a new maintainer, please see
https://github.com/LibVNC/x11vnc/issues/186 ❗**

This is x11vnc with its development continued by LibVNC and the GitHub community.
While 0.9.13 was the last release by the original author Karl Runge, 0.9.14 was
the first community-based release here on GitHub.

This repo represents the x11vnc codebase that once resided in the LibVNCServer repo.
It was split off LibVNCServer and moved here with
https://github.com/LibVNC/libvncserver/commit/498d222976975f53dea885cfe43ef0f805abd412.

Copyright (C) 2002-2010 Karl J. Runge <runge@karlrunge.com>
All rights reserved.


# x11vnc: a VNC server for real X displays

x11vnc allows one to view remotely and interact with real X displays
(i.e. a display corresponding to a physical monitor, keyboard, and
mouse) with any VNC viewer. In this way it plays the role for Unix/X11
that WinVNC plays for Windows.

It has built-in SSL/TLS encryption and 2048 bit RSA authentication,
including VeNCrypt support; UNIX account and password login support;
server-side scaling; single port HTTPS/HTTP+VNC; Zeroconf service
advertising; and TightVNC and UltraVNC file-transfer. It has also been
extended to work with non-X devices: natively on Mac OS X Aqua/Quartz,
webcams and TV tuner capture devices, and embedded Linux systems such
as Qtopia Core. Full IPv6 support is provided. More features are
described here.

It also provides an encrypted Terminal Services mode (-create, -svc,
or -xdmsvc options) based on Unix usernames and Unix passwords where
the user does not need to memorize his VNC display/port number.
Normally a virtual X session (Xvfb) is created for each user, but it
also works with X sessions on physical hardware. See the tsvnc
terminal services mode of the SSVNC viewer for one way to take
advantage of this mode.

I wrote x11vnc back in 2002 because x0rfbserver was basically
impossible to build on Solaris and had poor performance. The primary
x0rfbserver build problems centered around esoteric C++ toolkits.
x11vnc is written in plain C and needs only standard libraries and so
should work on nearly all Unixes, even very old ones. I also created
enhancements to improve the interactive response, added many features,
and etc.

This page including the FAQ contains much information [*]; solutions
to many problems; and interesting applications, but nevertheless
please feel free to contact me if you have problems or questions (and
if I save you time or expense by giving you some of my time, please
consider a PayPal Donation.) Do check the FAQ and this page first; I
realize the pages are massive, but you can often use your browser's
find-in-page search action using a keyword to find the answer to your
problem or question.

SSVNC:  An x11vnc side-project provides an Enhanced TightVNC Viewer
package (SSVNC) for Unix, Windows, and Mac OS X with automatic SSL
and/or SSH tunnelling support, SSL Certificate creation, Saved
connection profiles, Zeroconf, VeNCrypt, and built-in Proxy support.
Added features for the TightVNC Unix viewer: NewFBSize, ZRLE encoding,
Viewer-side Scaling, cursor alphablending, low color modes, and
enhanced popup menu; UltraVNC extensions support for: File Transfer,
Text Chat, Single Window, Server Input, and 1/n Scaling extensions,
and UltraVNC DSM encryption. The SSVNC bundle could be placed on, say,
a USB memory stick for SSL/SSH VNC viewing from nearly any networked
computer.


# Announcements

Important: If you created any permanent SSL certificates (e.g. via
"x11vnc -ssl SAVE ...") on a Debian or Ubuntu system from Sept. 2006
through May 2008, then those keys are likely extremely weak and can be
easily cracked. The certificate files should be deleted and recreated
on a non-Debian system or an updated one. See
http://www.debian.org/security/2008/dsa-1571 for details. The same
applies to SSH keys (not used by x11vnc directly, but many people use
SSH tunnels for VNC access.)

# News

See [NEWS](NEWS)

# Frequently Asked Questions (FAQs)

See [FAQ.md](doc/FAQ.md)


# Background

VNC (Virtual Network Computing) is a very useful network graphics
protocol (applications running on one computer but displaying their
windows on another) in the spirit of X, however, unlike X, the
viewing-end is very simple and maintains no state. It is a remote
framebuffer (RFB) protocol.

Some VNC links:

* http://www.realvnc.com
* http://www.tightvnc.com
* http://www.ultravnc.com/
* http://www.testplant.com/products/vine_server/OS_X

For Unix, the traditional VNC implementation includes a "virtual" X11
server Xvnc (usually launched via the vncserver command) that is not
associated with a physical display, but provides a "fake" one X11
clients (xterm, firefox, etc.) can attach to. A remote user then
connects to Xvnc via the VNC client vncviewer from anywhere on the
network to view and interact with the whole virtual X11 desktop.

The VNC protocol is in most cases better suited for remote connections
with low bandwidth and high latency than is the X11 protocol because
it involves far fewer "roundtrips" (an exception is the cached pixmap
data on the viewing-end provided by X.) Also, with no state maintained
the viewing-end can crash, be rebooted, or relocated and the
applications and desktop continue running. Not so with X11.

So the standard Xvnc/vncserver program is very useful, I use it for
things like:

* Desktop conferencing with other users (e.g. code reviews.)
* Long running apps/tasks I want to be able to view from many places
  (e.g. from home and work.)
* Motif, GNOME, and similar applications that would yield very poor
  performance over a high latency link.

However, sometimes one wants to connect to a real X11 display (i.e.
one attached to a physical monitor, keyboard, and mouse: a Workstation
or a SunRay session) from far away. Maybe you want to close down an
application cleanly rather than using kill, or want to work a bit in
an already running application, or would like to help a distant
colleague solve a problem with their desktop, or would just like to
work out on the deck for a while. This is where x11vnc is useful.

# How to use x11vnc

In this basic example let's assume the remote machine with the X
display you wish to view is "far-away.east:0" and the workstation you
are presently working at is "sitting-here.west".

Step 0. Download x11vnc (see below) and have it available to run on
far-away.east (on some linux distros it is as easy as "apt-get install
x11vnc", "emerge x11vnc", etc.) Similarly, have a VNC viewer (e.g.
vncviewer) ready to run on sitting-here.west. We recommend TightVNC
Viewers (see also our SSVNC viewer.)

Step 1. By some means log in to far-away.east and get a command shell
running there. You can use ssh, or even rlogin, telnet, or any other
method to do this. We do this because the x11vnc process needs to be
run on the same machine the X server process is running on (otherwise
things would be extremely slow.)

Step 2. In that far-away.east shell (with command prompt "far-away>"
in this example) run x11vnc directed at the far-away.east X session
display:

```
far-away> x11vnc -display :0
```

You could have also set the environment variable DISPLAY=:0 instead of
using "-display :0". This step attaches x11vnc to the far-away.east:0
X display (i.e. no viewer clients yet.)

Common Gotcha: To get X11 permissions right, you may also need to set
the XAUTHORITY environment variable (or use the -auth option) to point
to the correct MIT-MAGIC-COOKIE file (e.g. /home/joe/.Xauthority.) If
x11vnc does not have the authority to connect to the display it exits
immediately. More on how to fix this below.

If you suspect an X11 permissions problem do this simple test: while
sitting at the physical X display open a terminal window
(gnome-terminal, xterm, etc.) You should be able to run x11vnc
successfully in that terminal without any need for command line
options. If that works OK then you know X11 permissions are the only
thing preventing it from working when you try to start x11vnc via a
remote shell. Then fix this with the tips below.

Note as of Feb/2007 you can also try the -find option instead of
"-display ..." and see if that finds your display and Xauthority. Note
as of Dec/2009 the -findauth and "-auth guess" options may be helpful
as well.
(End of Common Gotcha)

When x11vnc starts up there will then be much chatter printed out (use
"-q" to quiet it), until it finally says something like:

```
.
.
13/05/2004 14:59:54 Autoprobing selected port 5900
13/05/2004 14:59:54 screen setup finished.
13/05/2004 14:59:54
13/05/2004 14:59:54 The VNC desktop is far-away:0
PORT=5900
```

which means all is OK, and we are ready for the final step.

Step 3. At the place where you are sitting (sitting-here.west in this
example) you now want to run a VNC viewer program. There are VNC
viewers for Unix, Windows, MacOS, Java-enabled web browsers, and even
for PDA's like the Palm Pilot and Cell Phones! You can use any of them
to connect to x11vnc (see the above VNC links under "Background:" on
how to obtain a viewer for your platform or see this FAQ. For Solaris,
vncviewer is available in the Companion CD package SFWvnc.)

In this example we'll use the Unix vncviewer program on sitting-here
by typing the following command in a second terminal window:

```
sitting-here> vncviewer far-away.east:0
```

That should pop up a viewer window on sitting-here.west showing and
allowing interaction with the far-away.east:0  X11 desktop. Pretty
nifty! When finished, exit the viewer: the remote x11vnc process will
shutdown automatically (or you can use the -forever option to have it
wait for additional viewer connections.)

Common Gotcha: Nowadays there will likely be a host-level firewall on
the x11vnc side that is blocking remote access to the VNC port (e.g.
5900.) You will either have to open up that port (or a range of ports)
in your firewall administration tool, or try the SSH tunnelling method
below (even still the firewall must allow in the SSH port, 22.)

Shortcut: Of course if you left x11vnc running on far-away.east:0 in a
terminal window with the -forever option or as a service, you'd only
have to do Step 3 as you moved around. Be sure to use a VNC Password
or other measures if you do that.

Super Shortcut: Here is a potentially very easy way to get all of it
working.

* Have x11vnc (0.9.3 or later) available to run on the remote host
  (i.e. in $PATH.)
* Download and unpack a SSVNC bundle (1.0.19 or later, e.g.
  ssvnc_no_windows-1.0.28.tar.gz) on the Viewer-side machine.
* Start the SSVNC Terminal Services mode GUI: ./ssvnc/bin/tsvnc
* Enter your remote username@hostname (e.g. fred@far-away.east) in
  the "VNC Terminal Server" entry.
* Click "Connect".

That will do an SSH to username@hostname and start up x11vnc and then
connect a VNC Viewer through the SSH encrypted tunnel.

There are a number of things assumed here, first that you are able to
SSH into the remote host; i.e. that you have a Unix account there and
the SSH server is running. On Unix and MacOS X it is assumed that the
ssh client command is available on the local machine (on Windows a
plink binary is included in the SSVNC bundle.) Finally, it is assumed
that you are already logged into an X session on the remote machine,
e.g. your workstation (otherwise, a virtual X server, e.g. Xvfb, will
be started for you.)

In some cases the remote SSH server will not run commands with the
same $PATH that you normally have in your shell there. In this case
click on Options -> Advanced -> X11VNC Options, and type in the
location of the x11vnc binary under "Full Path". (End of Super
Shortcut)

Desktop Sharing: The above more or less assumed nobody was sitting at
the workstation display "far-away.east:0". This is often the case: a
user wants to access her workstation remotely. Another usage pattern
has the user sitting at "far-away.east:0" and invites one or more
other people to view and interact with his desktop. Perhaps the user
gives a demo or presentation this way (using the telephone for vocal
communication.) A "Remote Help Desk" mode would be similar: a
technician connects remotely to the user's desktop to interactively
solve a problem the user is having.

For these cases it should be obvious how it is done. The above steps
will work, but more easily the user sitting at far-away.east:0 simply
starts up x11vnc from a terminal window, after which the guests would
start their VNC viewers. For this usage mode the "-connect
host1,host2" option may be of use to automatically connect to the
vncviewers in "-listen" mode on the list of hosts.

## Tunnelling x11vnc via SSH

The above example had no security or privacy at all. When logging into
remote machines (certainly when going over the internet) it is best to
use ssh, or use a VPN (for a VPN, Virtual Private Network, the above
example should be pretty safe.)

For x11vnc one can tunnel the VNC protocol through an encrypted ssh
channel. It would look something like running the following commands:

```
sitting-here> ssh -t -L 5900:localhost:5900 far-away.east 'x11vnc -localhost -display :0'
```

(you will likely have to provide passwords/passphrases to login from
sitting-here into your far-away.east Unix account; we assume you have
a login account on far-away.east and it is running the SSH server)

And then in another terminal window on sitting-here run the command:

```
sitting-here> vncviewer -encodings "copyrect tight zrle hextile" localhost:0
```

Note: The -encodings option is very important: vncviewer will often
default to "raw" encoding if it thinks the connection is to the local
machine, and so vncviewer gets tricked this way by the ssh
redirection. "raw" encoding will be extremely slow over a networked
link, so you need to force the issue with -encodings "copyrect tight
...". Nowadays, not all viewers use the -encodings option, try
"-PreferredEncoding=ZRLE" (although the newer viewers seem to
autodetect well when to use raw or not.)

Note that "x11vnc -localhost ..." limits incoming vncviewer
connections to only those from the same machine. This is very natural
for ssh tunnelling (the redirection appears to come from the same
machine.) Use of a VNC password is also strongly recommended.

Note also the -t we used above (force allocate pseudoterminal), it
actually seems to improve interactive typing response via VNC!

You may want to add the -C option to ssh to enable compression. The
VNC compression is not perfect, and so this may help a bit. However,
over a fast LAN you probably don't want to enable SSH compression
because it can slow things down. Try both and see which is faster.

If your username is different on the remote machine use something
like: "fred@far-away.east" in the above ssh command line.

Some VNC viewers will do the ssh tunnelling for you automatically, the
TightVNC Unix vncviewer does this when the "-via far-away.east" option
is supplied to it (this requires x11vnc to be already running on
far-away.east or having it started by inetd(8).) See the 3rd script
example below for more info.

SSVNC:  You may also want to look at the Enhanced TightVNC Viewer
(ssvnc) bundles because they contain scripts and GUIs to automatically
set up SSH tunnels (e.g. the GUI, "ssvnc", does it automatically and
so does this command: "ssvnc_cmd -ssh user@far-away.east:0") and can
even start up x11vnc as well.

The Terminal Services mode of SSVNC is perhaps the easiest way to use
x11vnc. You just need to have x11vnc available in $PATH on the remote
side (and can SSH to the host), and then on the viewer-side you type
something like:

```
tsvnc fred@far-away.east
```

everything else is done automatically for you. Normally this will
start a virtual Terminal Services X session (RAM-only), but if you
already have a real X session up on the physical hardware it will find
that one for you.

Gateways:  If the machine you SSH into is not the same machine with
the X display you wish to view (e.g. your company provides incoming
SSH access to a gateway machine), then you need to change the above
to, e.g.: "-L 5900:OtherHost:5900":

```
sitting-here> ssh -t -L 5900:OtherHost:5900 gateway.east
```

Where gateway.east is the internet hostname (or IP) of the gateway
machine (SSH server.) 'OtherHost' might be, e.g., freds-pc or
192.168.2.33 (it is OK for these to be private hostnames or private IP
addresses, the host in -L is relative to the remote server side.)

Once logged in, you'll need to do a second login (ssh, rsh, etc.) to
the workstation machine 'OtherHost' and then start up x11vnc on it (if
it isn't already running.) (The "-connect gateway:59xx" option may be
another alternative here with the viewer already in -listen mode.) For
an automatic way to use a gateway and have all the network traffic
encrypted (including inside the firewall) see Chaining SSH's.

These gateway access modes also can be done automatically for you via
the "Proxy/Gateway" setting in SSVNC (including the Chaining SSH's
case, "Double Proxy".)

## Chaining SSH's

Note that for use of a ssh gateway and -L redirection
to an internal host (e.g. "-L 5900:otherhost:5900") the VNC traffic
inside the firewall is not encrypted and you have to manually log into
otherhost to start x11vnc. Kyle Amon shows a method where you chain
two ssh's together that encrypts all network traffic and also
automatically starts up x11vnc on the internal workstation:

```
#!/bin/sh
#
gateway="example.com"   # or "user@example.com"
host="labyrinth"        # or "user@hostname"
user="kyle"

# Need to sleep long enough for all of the passwords and x11vnc to start up.
# The </dev/null below makes the vncviewer prompt for passwd via popup window.
#
(sleep 10; vncviewer -encodings "copyrect tight zrle zlib hextile" \
    localhost:0 </dev/null >/dev/null) &

# Chain the vnc connection thru 2 ssh's, and connect x11vnc to user's display:
#
exec /usr/bin/ssh -t -L 5900:localhost:5900 $gateway \
     /usr/bin/ssh -t -L 5900:localhost:5900 $host \
     sudo /usr/bin/x11vnc -localhost -auth /home/$user/.Xauthority \
         -rfbauth .vnc/passwd -display :0
```

Also note the use of sudo(1) to switch to root so that the different
user's .Xauthority file can be accessed. See the visudo(8) manpage for
details on how to set this up (remove the sudo if you do not want to
do this). One can also chain together ssh's for reverse connections
with vncviewers using the -listen option. For this case -R would
replace the -L (and 5500 the 5900, see the #2 example script above).
If the gateway machine's sshd is configured with GatewayPorts=no (the
default) then the double chaining of "ssh -R ..." will be required for
reverse connections to work.

## Firewalls/Routers

A lot of people have inexpensive devices for home or office that act
as a Firewall and Router to the machines inside on a private LAN. One
can usually configure the Firewall/Router from inside the LAN via a
web browser.

Often having a Firewall/Router sitting between the vncviewer and
x11vnc will make it impossible for the viewer to connect to x11vnc.

One thing that can be done is to redirect a port on the
Firewall/Router to, say, the SSH port (22) on an inside machine (how
to do this depends on your particular Firewall/Router, often the
router config URL is http://192.168.100.1 See www.portforward.com for
more info.) This way you reach these computers from anywhere on the
Internet and use x11vnc to view X sessions running on them.

Suppose you configured the Firewall/Router to redirect these ports to
two internal machines:

```
Port 12300 -> 192.168.1.3, Port 22 (SSH)
Port 12301 -> 192.168.1.4, Port 22 (SSH)

(where 192.168.1.3 is "jills-pc" and 192.168.1.4 is "freds-pc".) Then the ssh's would look something like:
sitting-here> ssh -t -p 12300 -L 5900:localhost:5900 jill@far-away.east 'x11v nc -localhost -display :0'
sitting-here> ssh -t -p 12301 -L 5900:localhost:5900 fred@far-away.east 'x11v nc -localhost -display :0'
```

Where far-away.east means the hostname (or IP) that the
Router/Firewall is using (for home setups this is usually the IP
gotten from your ISP via DHCP, the site http://www.whatismyip.com/ is
a convenient way to determine what it is.)

It is a good idea to add some obscurity to accessing your system via
SSH by using some high random port (e.g. 12300 in the above example.)
If you can't remember it, or are otherwise not worried about port
scanners detecting the presence of your SSH server and there is just
one internal PC involved you could map 22:

```
Port 22 -> 192.168.1.3, Port 22 (SSH)
```

Again, this SSH gateway access can be done automatically for you via
the "Proxy/Gateway" setting in SSVNC. And under the "Remote SSH
Command" setting you can enter the x11vnc -localhost -display :0.

Host-Level-Firewalls: even with the hardware Firewall/Router problem
solved via a port redirection, most PC systems have their own Host
level "firewalls" enabled to protect users from themselves. I.e. the
system itself blocks all incoming connections. So you will need to see
what is needed to configure it to allow in the port (e.g. 22) that you
desire. E.g. Yast, Firestarter, iptables(1), etc..

VNC Ports and Firewalls: The above discussion was for configuring the
Firewall/Router to let in port 22 (SSH), but the same thing can be
done for the default VNC port 5900:

```
Port 5900 -> 192.168.1.3, Port 5900 (VNC)
Port 5901 -> 192.168.1.4, Port 5900 (VNC)
```

(where 192.168.1.3 is "jills-pc" and 192.168.1.4 is "freds-pc".) This
could be used for normal, unencrypted connections and also for SSL
encrypted ones.

The VNC displays to enter in the VNC viewer would be, say,
"far-away.east:0" to reach jills-pc and "far-away.east:1" to reach
freds-pc. We assume above that x11vnc is using port 5900 (and any
Host-Level-firewalls on jills-pc has been configured to let that port
in.) Use the "-rfbport" option to tell which port x11vnc should listen
on.

For a home system one likely does not have a hostname and would have
to use the IP address, say, "24.56.78.93:0". E.g.:

```
vncviewer 24.56.78.93:0
```

You may want to choose a more obscure port on the router side, e.g.
5944, to avoid a lot of port scans finding your VNC server. For 5944
you would tell the viewer to use:

```
vncviewer 24.56.78.93:44
```

The IP address would need to be communicated to the person running the
VNC Viewer. The site http://www.whatismyip.com/ can help here.

Scripts to automate ssh tunneling: As discussed below, there may be
some problems with port 5900 being available. If that happens, the
above port and display numbers may change a bit (e.g. -> 5901 and :1).
However, if you "know" port 5900 will be free on the local and remote
machines, you can easily automate the above two steps by using the
x11vnc option -bg (forks into background after connection to the
display is set up) or using the -f option of ssh. Some example scripts
are shown below. Feel free to try the ssh -C to enable its compression
and see if that speeds things up noticeably.

1. A simple example script, assuming no problems with port 5900 being
taken on the local or remote sides, looks like:

    ```
    #!/bin/sh
    # usage: x11vnc_ssh <host>:<xdisplay>
    #  e.g.: x11vnc_ssh snoopy.peanuts.com:0
    #  (user@host:N also works)

    host=`echo $1 | awk -F: '{print $1}'`
    disp=`echo $1 | awk -F: '{print $2}'`
    if [ "x$disp" = "x" ]; then disp=0; fi

    cmd="x11vnc -display :$disp -localhost -rfbauth .vnc/passwd"
    enc="copyrect tight zrle hextile zlib corre rre raw"

    ssh -f -t -L 5900:localhost:5900 $host "$cmd"

    for i in 1 2 3
    do
        sleep 2
        if vncviewer -encodings "$enc" :0; then break; fi
    done
    ```

    See also rx11vnc.pl below.

2. Another method is to start the VNC viewer in listen mode
"vncviewer -listen" and have x11vnc initiate a reverse connection
using the -connect option:

    ```
    #!/bin/sh
    # usage: x11vnc_ssh <host>:<xdisplay>
    #  e.g.: x11vnc_ssh snoopy.peanuts.com:0
    #  (user@host:N also works)

    host=`echo $1 | awk -F: '{print $1}'`
    disp=`echo $1 | awk -F: '{print $2}'`
    if [ "x$disp" = "x" ]; then disp=0; fi

    cmd="x11vnc -display :$disp -localhost -connect localhost"   # <== note new option
    enc="copyrect tight zrle hextile zlib corre rre raw"

    vncviewer -encodings "$enc" -listen &
    pid=$!
    ssh -t -R 5500:localhost:5500 $host "$cmd"
    kill $pid
    ```

    Note the use of the ssh option "-R" instead of "-L" to set up a remote
    port redirection.

3. A third way is specific to the TightVNC vncviewer special option
-via for gateways. The only tricky part is we need to start up x11vnc
and give it some time (5 seconds in this example) to start listening
for connections (so we cannot use the TightVNC default setting for
VNC_VIA_CMD):

    ```
    #!/bin/sh
    # usage: x11vnc_ssh <host>:<xdisplay>
    #  e.g.: x11vnc_ssh snoopy.peanuts.com:0

    host=`echo $1 | awk -F: '{print $1}'`
    disp=`echo $1 | awk -F: '{print $2}'`
    if [ "x$disp" = "x" ]; then disp=0; fi

    VNC_VIA_CMD="ssh -f -t -L %L:%H:%R %G x11vnc -localhost -rfbport 5900 -display
    :$disp; sleep 5"
    export VNC_VIA_CMD

    vncviewer -via $host localhost:0      # must be TightVNC vncviewer.
    ```

    Of course if you already have the x11vnc running waiting for
    connections (or have it started out of inetd(8)), you can simply use
    the TightVNC "vncviewer -via gateway host:port" in its default mode to
    provide secure ssh tunnelling.

VNC password file: Also note in the #1. example script that the option
"-rfbauth .vnc/passwd" provides additional protection by requiring a
VNC password for every VNC viewer that connects. The vncpasswd or
storepasswd programs, or the x11vnc -storepasswd option can be used to
create the password file. x11vnc also has the slightly less secure
-passwdfile and "-passwd XXXXX" options to specify passwords.

Very Important: It is up to YOU to tell x11vnc to use password
protection (-rfbauth or -passwdfile), it will NOT do it for you
automatically or force you to (use -usepw if you want to be forced
to.) The same goes for encrypting the channel between the viewer and
x11vnc: it is up to you to use ssh, stunnel, -ssl mode, a VPN, etc.
(use the Enhanced TightVNC Viewer (SSVNC) GUI if you want to be forced
to use SSL or SSH.) For additional safety, also look into the -allow
and -localhost options and building x11vnc with tcp_wrappers support
to limit host access.


## Tunnelling x11vnc via SSL/TLS

One can also encrypt the VNC traffic using an SSL/TLS tunnel such as
stunnel.mirt.net (also stunnel.org) or using the built-in (Mar/2006)
-ssl openssl mode. A SSL-enabled Java applet VNC Viewer is also
provided in the x11vnc package (and https can be used to download it.)

Although not as ubiquitous as ssh, SSL tunnelling still provides a
useful alternative. See this FAQ on -ssl and -stunnel modes for
details and examples.

The Enhanced TightVNC Viewer (SSVNC) bundles contain some convenient
utilities to automatically set up an SSL tunnel from the viewer-side
(i.e. to connect to "x11vnc -ssl ...".) And many other enhancements
too.

# Downloading x11vnc

x11vnc is a contributed program to the LibVNCServer project at
SourceForge.net. I use libvncserver for all of the VNC aspects; I
couldn't have done without it. The full source code may be found and
downloaded (either file-release tarball or GIT tree) from the above
link. As of Sep 2010, the x11vnc-0.9.12.tar.gz source package is
released (recommended download). The x11vnc 0.9.12 release notes.

The x11vnc package is the subset of the libvncserver package needed to
build the x11vnc program. Also, you can get a copy of my latest,
bleeding edge x11vnc-0.9.13-dev.tar.gz tarball to build the most up to
date one.

Precompiled Binaries/Packages:  See the FAQ below for information
about where you might obtain a precompiled x11vnc binary from 3rd
parties and some ones I create.

VNC Viewers:  To obtain VNC viewers for the viewing side (Windows, Mac
OS, or Unix) try these links:

* http://www.tightvnc.com/download.html
* http://www.realvnc.com/download-free.html
* http://sourceforge.net/projects/cotvnc/
* http://www.ultravnc.com/
* Our Enhanced TightVNC Viewer (SSVNC)[ssvnc.gif]

More tools: Here is a ssh/rsh wrapper script rx11vnc that attempts to
automatically do the above Steps 1-3 for you (provided you have
ssh/rsh login permission on the machine x11vnc is to be run on.) The
above example would be: "rx11vnc far-away.east:0" typed into a shell
on sitting-here.west. Also included is an experimental script
rx11vnc.pl that attempts to tunnel the vnc traffic through an ssh port
redirection (and does not assume port 5900 is free.) Have a look at
them to see what they do and customize as needed:

* rx11vnc wrapper script
* rx11vnc.pl wrapper script to tunnel traffic thru ssh


# Building x11vnc

Make sure you have all the needed build/compile/development packages
installed. On a Debian-based distro you can simply do

```
sudo apt-get build-dep x11vnc
```

to install most of them.

To generate the build system, do a

```
autoreconf -fiv
```

After that, it's the usual

```
./configure
make
```

You might want to take a look at the dfferent configure options
provided by `configure --help` to build without X11 or adapt the
build to your needs in other ways.

## Building on Solaris, FreeBSD, etc

Depending on your version of
Solaris or other Unix OS the jpeg and/or zlib libraries may be in
non-standard places (e.g. /usr/local, /usr/sfw, /opt/sfw, etc.)

Note: If configure cannot find these two libraries then TightVNC and
ZRLE encoding support will be disabled, and you don't want that!!! The
TightVNC encoding gives very good compression and performance, it even
makes a noticeable difference over a fast LAN.

Shortcuts: On Solaris 10 you can pick up almost everything just by
insuring that your PATH has /usr/sfw/bin (for gcc) and /usr/ccs/bin
(for other build tools), e.g.:

```
env PATH=/usr/sfw/bin:/usr/ccs/bin:$PATH sh -c './configure; make'
```

(The only thing this misses is /usr/X11/lib/libXrandr.so.2, which is
for the little used -xrandr option, see the script below to pick it up
as well.)


libjpeg is included in Solaris 9 and later (/usr/sfw/include and
/usr/sfw/lib), and zlib in Solaris 8 and later (/usr/include and
/usr/lib.) So on Solaris 9 you can pick up everything with something
like this:

```
env PATH=/usr/local/bin:/usr/ccs/bin:$PATH sh -c './configure --with-jpeg=/usr/sfw; make'
```

assuming your gcc is in /usr/local/bin and x11vnc 0.7.1 or later.
These are getting pretty long, see those assignments split up in the
build script below.

If your system does not have these libraries at all you can get the
source for the libraries to build them: libjpeg is available at
ftp://ftp.uu.net/graphics/jpeg/ and zlib at http://www.gzip.org/zlib/.
See also http://www.sunfreeware.com/ for Solaris binary packages of
these libraries as well as for gcc. Normally they will install into
/usr/local but you can install them anywhere with the
--prefix=/path/to/anywhere, etc.

Here is a build script that indicates one way to pass the library
locations information to the libvncserver configuration via the
CPPFLAGS and LDFLAGS environment variables.

```
#!/bin/sh

# Build script for Solaris, etc, with gcc, libjpeg and libz in
# non-standard locations.

# set to get your gcc, etc:
#
PATH=/path/to/gcc/bin:/usr/ccs/bin:/usr/sfw/bin:$PATH

JPEG=/path/to/jpeg      # set to maybe "/usr/local", "/usr/sfw", or "/opt/sfw"
ZLIB=/path/to/zlib      # set to maybe "/usr/local", "/usr/sfw", or "/opt/sfw"

# Below we assume headers in $JPEG/include and $ZLIB/include and the
# shared libraries are in $JPEG/lib and $ZLIB/lib.  If your situation
# is different change the locations in the two lines below.
#
CPPFLAGS="-I $JPEG/include -I $ZLIB/include"
LDFLAGS="-L$JPEG/lib -R $JPEG/lib -L$ZLIB/lib -R $ZLIB/lib"

# These two lines may not be needed on more recent Solaris releases:
#
CPPFLAGS="$CPPFLAGS -I /usr/openwin/include"
LDFLAGS="$LDFLAGS -L/usr/openwin/lib -R /usr/openwin/lib"

# These are for libXrandr.so on Solaris 10:
#
CPPFLAGS="$CPPFLAGS -I /usr/X11/include"
LDFLAGS="$LDFLAGS -L/usr/X11/lib -R /usr/X11/lib"

# Everything needs to built with _REENTRANT for thread safe errno:
#
CPPFLAGS="$CPPFLAGS -D_REENTRANT"

export PATH CPPFLAGS LDFLAGS

./configure
make

ls -l ./x11vnc/x11vnc
```

Then do make install or copy the x11vnc binary to your desired
destination.

BTW, To run a shell script, just cut-and-paste the above into a file,
say "myscript", then modify the "/path/to/..." items to correspond to
your system/environment, and then type: "sh myscript" to run it.

Note that on Solaris make is /usr/ccs/bin/make, so that is why the
above puts /usr/ccs/bin in PATH. Other important build utilities are
there too: ld, ar, etc. Also, it is probably a bad idea to have
/usr/ucb in your PATH while building.

Starting with the 0.7.1 x11vnc release the "configure --with-jpeg=DIR
--with-zlib=DIR" options are handy if you want to avoid making a
script.

If you need to link OpenSSL libssl.a on Solaris see this method.

If you need to build on Solaris 2.5.1 or earlier or other older Unix
OS's, see this workaround FAQ.

## Building on FreeBSD, OpenBSD, ...

The jpeg libraries seem to be in
/usr/local or /usr/pkg on these OS's. You won't need the openwin stuff
in the above script (but you may need /usr/X11R6/....) Also starting
with the 0.7.1 x11vnc release, this usually works:

```
./configure --with-jpeg=/usr/local
make
```

## Building on HP-UX

For jpeg and zlib you will need to do the same
sort of thing as described above for Solaris. You set CPPFLAGS and
LDFLAGS to find them (see below for an example.) You do not need to do
any of the above /usr/openwin stuff. Also, HP-UX does not seem to
support -R, so get rid of the -R items in LDFLAGS. Because of this, at
runtime you may need to set LD_LIBRARY_PATH or SHLIB_PATH to indicate
the directory paths so the libraries can be found. It is a good idea
to have static archives, e.g. libz.a and libjpeg.a for the nonstandard
libraries so that they get bolted into the x11vnc binary (and so won't
get "lost".)

Here is what we recently did to build x11vnc 0.7.2 on HP-UX 11.11

```
./configure --with-jpeg=$HOME/hpux/jpeg --with-zlib=$HOME/hpux/zlib
make
```

Where we had static archives (libjpeg.a, libz.a) only and header files
in the $HOME/hpux/... directories as discussed for the build script.

On HP-UX 11.23 and 11.31 we have had problems compiling with gcc.
"/usr/include/rpc/auth.h:87: error: field 'syncaddr' has incomplete
type". As a workaround for x11vnc 0.9.4 and later set your CPPFLAGS to
include:

```
CPPFLAGS="-DIGNORE_GETSPNAM"
export CPPFLAGS
```

This disables a very rare usage mode for -unixpw_nis by not trying
getspnam(3).

Using HP-UX's C compiler on 11.23 and 11.31 we have some severe
compiler errors that have not been worked around yet. If you need to
do this, contact me and I will give you a drastic recipe that will
produce a working binary.

## Building on AIX

AIX: one user had to add the "X11.adt" package to
AIX 4.3.3 and 5.2 to get build header files like XShm.h, etc. You may
also want to make sure that /usr/lpp/X11/include, etc is being picked
up by the configure and make.

For a recent build on AIX 5.3 we needed to add these CFLAGS to be able
to build with gcc:
env CFLAGS='-maix64 -Xlinker -bbigtoc' ./configure ...

we also built our own libjpeg and libz using -maix64.

BTW, one way to run an Xvfb-like virtual X server for testing on AIX
is something like "/usr/bin/X11/X -force -vfb -ac :1".

## Building on Mac OS X

There is now native Mac OS X support for
x11vnc by using the raw framebuffer feature. This mode does not use or
need X11 at all. To build you may need to disable X11:

```
./configure --without-x ...
make
```

However, if your system has the Mac OS X build package for X11 apps
you will not need to supply the "--without-x" option (in this case the
resulting x11vnc would be able to export both the native Mac OS X
display and windows displayed in the XDarwin X server.) Be sure to
include the ./configure option to find libjpeg on your system.

## OpenSSL

Starting with version 0.8.3 x11vnc can now be built with
SSL/TLS support. For this to be enabled the libssl.so library needs to
be available at build time. So you may need to have additional
CPPFLAGS and LDFLAGS items if your libssl.so is in a non-standard
place. As of x11vnc 0.9.4 there is also the --with-ssl=DIR configure
option.

Note that from OpenSSL 1.1.0 on SSLv2 support has been dropped and
SSLv3 deactivated at build time per default. This means that unless
explicitly enabled, OpenSSL builds only support TLS (any version).
Since there is a reason for dropping SSLv3 (heard of POODLE?), most
distributions do not enable it for their OpenSSL binary. In summary
this means compiling x11vnc against OpenSSL 1.1.0 or newer is no
problem, but using encryption will require a viewer with TLS support.

On Solaris using static archives libssl.a and libcrypto.a instead of
.so shared libraries (e.g. from www.sunfreeware.com), we found we
needed to also set LDFLAGS as follows to get the configure to work:

```
env LDFLAGS='-lsocket -ldl' ./configure --with-ssl=/path/to/openssl ...
make
```

## Misc. Build problems

We collect here rare build problems some users
have reported and the corresponding workarounds. See also the FAQ's on
building.

---

ENV parameter: One user had a problem where the build script below was
failing because his work environment had the ENV variable set to a
script that was resetting his PATH so that gcc could no longer be
found. Make sure you do not have any ENV or BASH_ENV in your
environment doing things like that. Typing "unset ENV", etc. before
configuring and building should clear it.

---

Bash xpg: One user had his bash shell compiled with
--enable-xpg-echo-default that causes some strange behavior with
things like echo "\\1 ..." the configure script executes. In
particular instead of getting "\1" the non-printable character "^A" is
produced, and causes failures at compile time like:

```
../rfb/rfbconfig.h:9:22: warning: extra tokens at end of #ifndef directive
```

The workaround is to configure like this:

```
env CONFIG_SHELL=/bin/sh /bin/sh ./configure
```

i.e. avoid using the bash with the misbehavior. A bug has been filed
against autoconf to guard against this.

---

AIX: one user had to add the "X11.adt" package to AIX to get build
header files like XShm.h, etc.

---

Ubuntu Feisty Fawn 7.04: In May/2007 one user said he needed to add
these packages to compile x11vnc on that Linux distro and version:

```
apt-get install build-essential make bin86 libjpeg62-dev libssl-dev libxtst-dev
```

Note that Ubuntu is based on Debian, so perhaps this is the list
needed on Debian (testing?) as well. To build in Avahi (mDNS service
advertising) support it would appear that libavahi-client-dev is
needed as well.

---

Exceedingly slow compilation: x11vnc has a couple of files which
contain very large "case statements" (over 100 cases) that on some
platforms can take a very long time to compile (in extreme cases over
an hour). However on 32bit Linux with intel/amd processor and gcc
these files usually take less than 10 seconds to compile. For 64bit
systems using gcc the problem appears to be much worse.

The two files with the large number of cases, remote.c and x11vnc.c,
have no real need to be optimized (the code is used only very
infrequently). So it is fine to supply "-O0" (disables optimization)
to CFLAGS when compiling them. However, it is tricky with
autoconf/automake to do this (especially since both the compiler and
make versions have a big effect).

So if the compile times are getting too long for you for these two
files you will need to manually change some things. First, run
configure and when it has finished, edit the generated file
x11vnc/Makefile and put these lines at the very top:

```
x11vnc-x11vnc.o :  CFLAGS += -O0
x11vnc-remote.o :  CFLAGS += -O0
```

Those lines assume gnu make (gmake) is being used. If you are using
another make, say Solaris make, insert these instead:

```
x11vnc-x11vnc.o := CFLAGS += -O0
x11vnc-remote.o := CFLAGS += -O0
```

You could write a build shell script that modified the Makefile this
way before running make.

The "-O0" (note it is "capital Oh" followed by "zero") assumes the gcc
compiler. If you are using a different compiler you will need to find
the command line option to disable optimization, or otherwise have the
lines set CFLAGS to the empty string.

---

Broken Thread Local Storage on SuSE 9.2: Starting with x11vnc 0.9.8
the bundled libvncserver uses the __thread keyword to make some of the
encodings (i.e. tight) thread safe (multiple VNC clients can be using
tight at the same time in x11vnc -threads mode.) Evidently on the old
SuSE 9.2 system the compiler does not support the thread local storage
properly. Here is an example build failure:

```
tight.c:1126: error: unrecognizable insn:
(insn:HI 11 10 13 0 (nil) (set (reg/f:SI 59)
        (const:SI (plus:SI (symbol_ref:SI ("%lpalette"))
                (const_int 2048 [0x800])))) -1 (nil)
    (expr_list:REG_EQUAL (const:SI (plus:SI (symbol_ref:SI ("%lpalette"))
                (const_int 2048 [0x800])))
        (nil)))
tight.c:1126: internal compiler error: in extract_insn, at recog.c:2175
Please submit a full bug report,
with preprocessed source if appropriate.
See URL:http://www.suse.de/feedback for instructions.
```

The workaround is to disable thread local storage at configure time
like this:

```
env CPPFLAGS="-DTLS=''" ./configure
```
and then build it.


# Some Notes

## Both a client and a server

It is sometimes confusing to people that
x11vnc is both a client and a server at the same time. It is an X
client because it connects to the running X server to do the screen
polls. Think of it as a rather efficient "screenshot" program running
continuously. It is a server in the sense that it is a VNC server that
VNC viewers on the network can connect to and view the screen
framebuffer it manages.

When trying to debug problems, remember to think of both roles. E.g.
"how is x11vnc connecting to the X server?", "how is the vncviewer
connecting to x11vnc?", "what permits/restricts the connection?". Both
links may have reachability, permission, and other issues.

## Network performance

Whether you are using Xvnc or x11vnc it is
always a good idea to have a solid background color instead of a
pretty background image. Each and every re-exposure of the background
must be resent over the network: better to have that background be a
solid color that compresses very well compared to a photo image. (This
is one place where the X protocol has an advantage over the VNC
protocol.) I suggest using xsetroot, dtstyle or similar utility to set
a solid background while using x11vnc. You can turn the pretty
background image back on when you are using the display directly.
Update: As of Feb/2005 x11vnc has the -solid [color] option that works
on recent GNOME, KDE, and CDE and also on classic X (background image
is on the root window.) Update: As of Oct/2007 x11vnc has the -ncache
option that does a reasonable job caching the background (and other)
pixmap data on the viewer side.

I also find the TightVNC encoding gives the best response for my usage
(Unix <-> Unix over cable modem.) One needs a tightvnc-aware vncviewer
to take advantage of this encoding.

## TCP port issues

Notice the lines

```
18/07/2003 14:36:31 Autoprobing selected port 5900
PORT=5900
```

in the output. 5900 is the default VNC listening port (just like 6000
is X11's default listening port.) Had port 5900 been taken by some
other application, x11vnc would have next tried 5901. That would mean
the viewer command above should be changed to vncviewer
far-away.east:1. You can force the port with the "-rfbport NNNN"
option where NNNN is the desired port number. If that port is already
taken, x11vnc will exit immediately. The "-N" option will try to match
the VNC display number to the X display.   (also see the "SunRay
Gotcha" note below)

## Options

x11vnc has (far too) many features that may be activated
via its command line options. Useful options are, e.g., -scale to do
server-side scaling, and -rfbauth passwd-file to use VNC password
protection (the vncpasswd or storepasswd programs, or the x11vnc
-storepasswd option can be used to create the password file.)

## Algorithm

How does x11vnc do it? Rather brute-forcedly: it
continuously polls the X11 framebuffer for changes using
XShmGetImage(). When changes are discovered, it instructs libvncserver
which rectangular regions of the framebuffer have changed, and
libvncserver compresses the changes and sends them off to any
connected VNC viewers. A number of applications do similar things,
such as x0rfbserver, krfb, x0vncserver, vino. x11vnc uses a 32 x 32
pixel tile model (the desktop is decomposed into roughly 1000 such
tiles), where changed tiles are found by pseudo-randomly polling 1
pixel tall horizontal scanlines separated vertically by 32 pixels.
This is a surprisingly effective algorithm for finding changed
regions. For keyboard and mouse user input the XTEST extension is used
to pass the input events to the X server. To detect XBell "beeps" the
XKEYBOARD extension is used. If available, the XFIXES extension is
used to retrieve the current mouse cursor shape. Also, if available
the X DAMAGE extension is used to receive hints from the X server
where modified regions on the screen are. This greatly reduces the
system load when not much is changing on the screen and also improves
how quickly the screen is updated.

## Barbershop mirrors effect

What if x11vnc is started up, and
vncviewer is then started up on the same machine and displayed on the
same display x11vnc is polling? One might "accidentally" do this when
first testing out the programs. You get an interesting
recursive/feedback effect where vncviewer images keep popping up each
one contained in the previous one and slightly shifted a bit by the
window manager decorations. There will be an even more interesting
effect if -scale is used. Also, if the XKEYBOARD is supported and the
XBell "beeps" once, you get an infinite loop of beeps going off.
Although all of this is mildly exciting it is not much use: you will
normally run and display the viewer on a different machine!


# Limitations

* Due to the polling nature, some activities (opaque window moves,
  scrolling), can be pretty choppy/ragged and others (exposures of
  large areas) slow. Experiment with interacting a bit differently
  than you normally do to minimize the effects (e.g. do fullpage
  paging rather than line-by-line scrolling, and move windows in a
  single, quick motion.) Recent work has provided the
  -scrollcopyrect and -wireframe speedups using the CopyRect VNC
  encoding and other things, but they only speed up some activities,
  not all.
* A rate limiting factor for x11vnc performance is that graphics
  hardware is optimized for writing, not reading (x11vnc reads the
  video framebuffer for the screen image data.) The difference can
  be a factor of 10 to 1000, and so it usually takes about 0.5-1 sec
  to read in the whole video hardware framebuffer (e.g. 5MB for
  1280x1024 at depth 24 with a read rate of 5-10MB/sec.) So whenever
  activity changes most of the screen (e.g. moving or iconifying a
  large window) there is a delay of 0.5-1 sec while x11vnc reads the
  changed regions in.
  A slow framebuffer read rate will often be the performance
  bottleneck on a fast LAN (whereas on slower links the reduced
  network bandwidth becomes the bottleneck.)
  Note: A quick way to get a 2X speedup of this for x11vnc is to
  switch your X server from depth 24 (32bpp) to depth 16 (16bpp.)
  You get a 4X speedup going to 8bpp, but the lack of color cells is
  usually unacceptable.
  To get a sense of the read and write speeds of your video card,
  you can run benchmarks like: "x11perf -getimage500",  "x11perf
  -putimage500",  "x11perf -shmput500" and for XFree86 displays with
  direct graphics access the "dga" command (press "b" to run the
  benchmark and then after a few seconds press "q" to quit.) Even
  this "dd if=/dev/fb0 of=/dev/null" often gives a good estimate.
  x11vnc also prints out its estimate:

  ```
  28/02/2009 11:11:07 Autoprobing TCP port
  28/02/2009 11:11:07 Autoprobing selected port 5900
  28/02/2009 11:11:08 fb read rate: 10 MB/sec
  28/02/2009 11:11:08 screen setup finished.
  ```

  We have seen a few cases where the hardware fb read speed is
  greater than 65 MB/sec: on high end graphics workstations from SGI
  and Sun, and also from a Linux user using nvidia proprietary
  drivers for his nvidia video card. Update 2008: thankfully, these
  sped up drivers are becoming more common on Linux and *BSD systems
  and that makes x11vnc run somewhat more quickly. Sometimes they
  have a read rate of over 400 MB/sec.
  On XFree86/Xorg it is actually possible to increase the
  framebuffer read speed considerably (10-100 times) by using the
  Shadow Framebuffer (a copy of the framebuffer is kept in main
  memory and this can be read much more quickly.) To do this one
  puts the line Option "ShadowFB" "true" in the Device section of
  the /etc/X11/XF86Config or /etc/X11/xorg.conf file. Note that this
  disables 2D acceleration at the physical display and so that might
  be unacceptable if one plays games, etc. on the machine's local
  display. Nevertheless this could be handy in some circumstances,
  e.g. if the slower speed while sitting at the physical display was
  acceptable (this seems to be true for most video cards these
  days.) Unfortunately it does not seem shadowfb can be turned on
  and off dynamically...
  Another amusing thing one can do is use Xvfb as the X server, e.g.
  "xinit $HOME/.xinitrc -- /usr/X11R6/bin/Xvfb :1 -screen 0
  1024x768x16" x11vnc can poll Xvfb efficiently via main memory.
  It's not exactly clear why one would want to do this instead of
  using vncserver/Xvnc, (perhaps to take advantage of an x11vnc
  feature, such as framebuffer scaling or built-in SSL encryption),
  but we mention it because it may be of use for special purpose
  applications. You may need to use the "-cc 4" option to force Xvfb
  to use a TrueColor visual instead of DirectColor. See also the
  description of the -create option that does all of this
  automatically for you (be sure to install the Xvfb package, e.g.
  apt-get install xvfb.)
  Also, a faster and more accurate way is to use the "dummy"
  Xorg/XFree86 device driver (or our Xdummy wrapper script.) See
  this FAQ for details.
* Somewhat surprisingly, the X11 mouse (cursor) shape is write-only
  and cannot be queried from the X server. So traditionally in
  x11vnc the cursor shape stays fixed at an arrow. (see the "-cursor
  X" and "-cursor some" options, however, for a partial hack for the
  root window, etc.) However, on Solaris using the SUN_OVL overlay
  extension, x11vnc can show the correct mouse cursor when the
  -overlay option is also supplied. A similar thing is done on IRIX
  as well when -overlay is supplied.
  More generally, as of Dec/2004 x11vnc supports the new XFIXES
  extension (in Xorg and Solaris 10) to query the X server for the
  exact cursor shape, this works pretty well except that cursors
  with transparency (alpha channel) need to approximated to solid
  RGB values (some cursors look worse than others.)
* Audio from applications is of course not redirected (separate
  redirectors do exist, e.g. esd, see the FAQ on this below.) The
  XBell() "beeps" will work if the X server supports the XKEYBOARD
  extension. (Note that on Solaris XKEYBOARD is disabled by default.
  Passing +kb to Xsun enables it.)
* The scroll detection algorithm for the -scrollcopyrect option can
  give choppy or bunched up transient output and occasionally
  painting errors.
* Using -threads can expose some bugs/crashes in libvncserver.
