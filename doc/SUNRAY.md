# Sun Ray Notes

You can run x11vnc on your (connected or disconnected) SunRay session
(Please remember to use settings like -wait 200, -sb 15, and not
running a screensaver animation (blank instead) to avoid being a
resource hog! x11vnc does induce a lot of memory I/O from polling the
X server. It also helps to have a solid background color, e.g.
-solid).

News: Sun Ray Remote Control Toolkit: See the nice set of tools in the
Sun Ray Remote Control Toolkit that launch x11vnc automatically for
you for certain usage modes.

You have to know the name of the machine your SunRay session X server
is running on (so you can ssh into it and start x11vnc). You also need
to know the X11 DISPLAY number for the session: on a SunRay it could
be a large number, e.g. :137, since there are many people with X
sessions (Xsun processes) on the same machine. If you don't know it,
you can get it by running who(1) in a shell on the SunRay server and
looking for the dtlocal entry with your username (and if you don't
even know which server machine has your session, you could login to
all possible ones looking at the who output for your username...).

I put some code in my ~/.dtprofile script that stores $DISPLAY
(including the hostname) in a ~/.sunray_current file at session
startup (and deletes it when the X session ends) to make it easy to
get at the hostname and X11 display number info for my current X
sessions when I ssh in and am about to start x11vnc.

SunRay Gotcha #1:   Note that even though your SunRay X11 DISPLAY is
something like :137, x11vnc still tries for port 5900 as its listening
port if it can get it, in which case the VNC display (i.e. the
information you supply to the VNC viewer) is something like
sunray-server:0   (note the :0 corresponding to port 5900, it is not
:137). If it cannot get 5900, it tries for 5901, and so on. You can
also try to force the port (and thereby the VNC display) using the
-rfbport NNNN option.

Especially on a busy Sun Ray server it is often difficult to find free
ports for both VNC and the HTTP Java applet server to listen on. This
script, vnc_findports may be of use for doing this automatically. It
suggests x11vnc command line options based on netstat output that
lists the occupied ports. It is even more difficult to start
vncserver/Xvnc on a busy Sun Ray because then 3 ports (HTTP, VNC, and
X11), all separated by 100 are needed! This script, findvncports may
be helpful as well. Both scripts start at VNC display :10 and work
their way up.

   SunRay Gotcha #2:   If you get an error like:

```
shmget(tile) failed.
shmget: No space left on device
```

when starting up x11vnc that most likely means all the shared memory
(shm) slots are filled up on your machine. The Solaris default is only
100, and that can get filled up in a week or so on a SunRay server
with lots of users. If the shm slot is orphaned (e.g. creator process
dies) the slot is not reclaimed. You can view the shm slots with the
"ipcs -mA" command. If there are about 100 then you've probably hit
this problem. They can be cleaned out (by the owner or by root) using
the ipcrm command. I wrote a script shm_clear that finds the orphans
and lists or removes them. Longer term, have your SunRay sysadmin add
something like this to /etc/system:

```
set shmsys:shminfo_shmmax = 0x2000000
set shmsys:shminfo_shmmni = 0x1000
```

SunRay Gotcha #3:   Some SunRay installations have implemented
suspending certain applications when a SunRay session is in a
disconnected state (e.g. Java Badge pulled out, utdetach, etc). This
is a good thing because it limits hoggy or runaway apps from wasting
the shared CPU resource. Think how much CPU and memory I/O is wasted
by a bunch of Firefox windows running worthless Flash animations while
your session is disconnected!

So some sites have implemented scripts to suspend (e.g. kill -STOP)
certain apps when your badge is removed from the SunRay terminal. When
you reattach, it kill -CONT them. This causes problems for viewing the
detached SunRay session via x11vnc: those suspended apps will not
respond (their windows will be blank or otherwise inactive).

What to do? Well, since you are going to be using the application you
might as well unfreeze it rather than starting up a 2nd instance. Here
is one way to do it using the kill -CONT mechanism:
kill -CONT `ps -ealf | grep ' T ' | grep $LOGNAME | awk '{print $4}'`

If you want to be a good citizen and re-freeze them before you exit
x11vnc this script could be of use:

```
#!/bin/sh
#
# kill -STOP/-CONT script for x11vnc (or other) SunRay usage ("freezes"
# certain apps from hogging resources when disconnected).
#
# Put here a pattern that matches the apps that are frozen:
#
appmatch="java_vm|jre|netscape-bin|firefox-bin|realplay|acroread|mozilla-bin"

if [ "X$1" = "Xfreeze" ]; then
        pkill -STOP -U $LOGNAME "$appmatch"
elif [ "X$1" = "Xthaw" ]; then
        pkill -CONT -U $LOGNAME "$appmatch"

elif [ "$RFB_MODE" = "afteraccept" -a "$RFB_STATE" = "NORMAL" ]; then
        # a valid x11vnc login.
        if [ "$RFB_CLIENT_COUNT" = "1" ]; then
                # only one client present.
                pkill -CONT -U $LOGNAME "$appmatch"
        fi
elif [ "$RFB_MODE" = "gone" -a "$RFB_STATE" = "NORMAL" ]; then
        # a valid x11vnc login.
        if [ "$RFB_CLIENT_COUNT" = "0" ]; then
                # last client present has just left.
                pkill -STOP -U $LOGNAME "$appmatch"
        fi
fi
exit 0
```

If you called the script "goodcitizen" you could type "goodcitizen
thaw" to unfreeze them, and then "goodcitizen freeze" to refreeze
them. One could also use these x11vnc options "-afteraccept
goodcitizen -gone goodcitizen" to do it automatically.

SunRay Gotcha #4:   Recent versions of the Sun Ray Server Software
SRSS (seems to be version 3.0 or 3.1) have a "misfeature" that when
the session is disconnected (i.e. badge/smartcard out) the screen
locker (xscreensaver) will freeze the X server just when the "Enter
Password" dialog box appears. So you cannot unlock the screen remotely
via x11vnc!

Update: please see Bob Doolittle's detailed description of the this
issue at the bottom of this section.

Here "freeze" means "stop other X clients from inserting keyboard and
mouse input and from viewing the current contents of the screen". Or
something like that; the upshot is x11vnc can't do its normal thing.

There are several workarounds for this.

1. The easiest one by far is to put these lines in your
$HOME/.dtprofile file:

    ```
    SUN_SUNRAY_UTXLOCK_PREF="/usr/openwin/bin/xlock -mode blank"
    export SUN_SUNRAY_UTXLOCK_PREF
    ```

   One might argue that xlock isn't particularly "pretty". (Just IMHO,
   but if something like this not being pretty actually gets in the way
   of your work I think some introspection may be in order. :-)

2. The problem has been traced to the pam_sunray.so PAM module.
Evidently xscreensaver invokes this pam module and it communicates
with utsessiond who in turn instructs the Xsun server to not process
any synthetic mouse/keyboard input or to update the screen
framebuffer. It is not clear if this is by design (security?) or
something else.

    In any event, the problem can be avoided, somewhat drastically, by
    commenting out the corresponding line in /etc/pam.conf:

    ```
    #xscreensaver auth sufficient /opt/SUNWut/lib/pam_sunray.so syncondisplay
    ```

    Leave the other xscreensaver pam authentication lines unchanged. The
    dtsession-SunRay line may also need to be commented out to avoid the
    problem for CDE sessions. N.B. it is possible the application of a
    SSRS patch, etc, may re-enable that /etc/pam.conf line. It may be
    difficult to convince a sysadmin to make this change.

3. A more forceful way is to kill the xscreensaver process from a
shell prompt whenever you connect via x11vnc and the screen is in a
locked state:

    ```
    pkill -U $LOGNAME '^xscreensaver$'
    ```

   And then after you are in be sure to restart it by typing something
   like:

    ```
    xscreensaver &
    ```

   You may want to avoid restarting it until you are about to disconnect
   your VNC viewer (since if it locks the screen while you are working
   you'll be stuck again).

   3') The above idea can be done a bit more cleanly by having x11vnc do
   it. Suppose we called the following script xss_killer:

    ```
    #!/bin/sh
    #
    # xss_killer: kill xscreensaver after a valid x11vnc client logs in.
    #             Restart xscreensaver and lock it when the last client
    #             disconnects.

    PATH=/usr/openwin/bin:/usr/bin:$PATH
    export PATH

    if [ "$RFB_MODE" = "afteraccept" -a "$RFB_STATE" = "NORMAL" ]; then
            # a valid x11vnc login.
            if [ "$RFB_CLIENT_COUNT" = "1" ]; then
                    # only one client present.
                    pkill -U $LOGNAME '^xscreensaver$'
                    pkill -KILL -U $LOGNAME -f xscreensaver/hacks
            fi
    elif [ "$RFB_MODE" = "gone" -a "$RFB_STATE" = "NORMAL" ]; then
            # a valid x11vnc login.
            if [ "$RFB_CLIENT_COUNT" = "0" ]; then
                    # last client present has just left.
                    xscreensaver -nosplash &
                    sleep 1
                    xscreensaver-command -lock &
            fi
    fi
    ```

    Then we would run x11vnc with these options: "-afteraccept xss_killer
    -gone xss_killer". The -afteraccept option (introduced in version 0.8)
    is used to run a command after a vncviewer has successfully logged in
    (note that this is a VNC login, not a Unix login, so you may not want
    to do this if you are really paranoid...)

   Note if you use the above script and also plan to Ctrl-C (SIGINT)
   x11vnc you have to run the xscreensaver in a new process group to
   avoid killing it as well. One way to do this is via this kludge:

    ```
    perl -e 'setpgrp(0,0); exec "xscreensaver -nosplash &"'
    ```

   in the above script.

4. There appears to be a bug in pam_sunray.so in that it doesn't seem
to honor the convention that, say, DISPLAY=unix:3 means to use Unix
sockets to connect to display 3 on the local machine (this is a bit
faster than TCP sockets). Rather, it thinks the display is a non-local
one to a machine named "unix" (that usually does not resolve to an IP
address).

    Amusingly, this can be used to bypass the pam_sunray.so blocking of
    Xsun that prevents one from unlocking the screen remotely via x11vnc.
    One could put something like this in $HOME/.dtprofile to kill any
    existing xscreensavers and then start up a fresh xscreensaver using
    DISPLAY=unix:N

    ```
    # stop/kill any running xscreensavers (probably not running yet, but to be sure)
    xscreensaver-command -exit
    pkill -U $LOGNAME '^xscreensaver$'
    env DISPLAY=`echo $DISPLAY | sed -e 's/^.*:/unix:/'` xscreensaver &
    ```

Important: Note that all of the above workarounds side-step the
pam_sunray.so PAM module in one way or another. You'll need to see if
that is appropriate for your site's SunRay / smartcard usage. Also,
these hacks may break other things and so you may want to test various
scenarios carefully. E.g. check corner cases like XDMCP/dtremote,
NSCM, etc.

Update May 2008: Here is a useful description of this issue from Bob
Doolittle who is a developer for Sun Ray at Sun. I don't have the time
to digest and distill it and then adjust the above methods to provide
a clearer description, so I just include below the description he sent
me with the hope that it will help some users:

> In SRSS 4.0 and earlier, the purpose of pam_sunray.so in the "auth"
> PAM stack of screensavers is to enable NSCM (and, although this is
> much less commonly used, "SC", which is configured when 3rd-party
> software is installed to allow smartcards to be used as part of the
> authentication process) to work. It should have no effect with
> smartcards. Currently, however, it does block the PAM stack for all
> sessions, which causes xscreensaver, when it locks a disconnected
> session, to not process any mouse or keyboard events as you
> describe (unless xscreensaver does an X server grab, however, other
> applications should still be able to draw in the session although
> xscreensaver may be playing tricks like putting a black window on
> top of everything). In both of the NSCM and SC models,
> authentication occurs in a separate session before SRSS will
> reconnect to the user session, in which case pam_sunray.so causes
> xscreensaver to just unlock the screen without prompting the user
> to enter their password again. To do this, pam_sunray.so has to
> block until the session becomes reconnected, so it can query SRSS
> at that time to determine whether the user has already
> authenticated or not. In SRSS 4.0 and earlier releases,
> pam_sunray.so could have been optimized to not block smartcard
> sessions, although since the session is disconnected this typically
> isn't important (except in the x11vnc case, as you've observed).
>
> In SRSS 4.1, however, for increased security the out-of-session
> authentication model has been extended to *all* session types, so
> pam_sunray.so will be required in all cases unless users are
> willing to authenticate twice upon hotdesking (e.g. when their card
> is inserted). In future, we may do away with pam_sunray.so, and in
> fact with any traditional screen locker in the user session, since
> SRSS itself will be providing better security than a screen locker
> running entirely within the user's X session is capable of
> providing.
>
> Your trick of setting DISPLAY to unix:DPY will effectively disable
> pam_sunray.so (I'm not sure I'd call that a bug - you're going out
> of your way to do something that wouldn't occur in the normal
> course of events, and really provides no useful value other than to
> tickle this behavior in pam_sunray.so). This will mean that, in
> SRSS 4.0 and earlier releases, users will be prompted for their
> passwords twice when reconnecting to their sessions for NSCM and SC
> session types. In 4.1, disabling pam_sunray.so in this way will
> cause this double-authentication to occur for *all* sessions,
> including simple smartcard sessions. Users may be willing to pay
> that price in order to be able to use x11vnc in disconnected
> sessions. I like this hack, personally. It's a little less
> convenient than some of the other approaches you describe, but it's
> lighter-weight and more secure than most of the other approaches,
> and provides the value of being able to use x11vnc in locked
> sessions.
>
> Here are some other minor notes: - I wouldn't recommend storing
> your display in your .dtprofile, unless you're willing to live with
> a single session at a time. Personally, I often find myself using
> several sessions, in several FoGs, for short periods of time so
> this would certainly break. IMO it's pretty easy to use $DISPLAY to
> do what you want on the fly, as needed, so I don't think the price
> of breaking multiple-session functionality would be worth the
> convenience, to me at least. Here's some ksh/bash syntax to extract
> the hostname and display number on the fly which you may find
> useful:
>
> ```
> HOSTNAME=${DISPLAY%:*}
> FULLDPY=${DISPLAY#*:}
> DPYNUM=${FULLDPY%.*}
> ```
>
> A final note may give you some insight into other clever hacks in
> this area: - Check out utaction. It's a very handy little utility
> that can be run as a daemon in the user session which will invoke a
> specified command upon session connects and/or disconnects.
> Personally, I start one up in my .dtprofile as follows:
>
> ```
> utaction -c $HOME/.srconnectrc -d $HOME/.srdisconnectrc &
> ```
>
> This then allows me to construct a .srconnectrc script containing
> useful commands I'd like to have run every time I insert my
> smartcard, and a .srdisconnectrc script of commands to be run every
> time I remove my smartcard (or, connect/disconnect to my session
> via NSCM or SC). This can be used for things like notifying a chat
> client of away status, as well as some of the hacks you've
> described on your page such as freeze/unfreeze, or perhaps to
> terminate an xscreensaver and start up a new one with the unix:DPY
> $DISPLAY specification as you describe (although it probably makes
> most sense to do this at login time, as opposed to every connect or
> disconnect event).
