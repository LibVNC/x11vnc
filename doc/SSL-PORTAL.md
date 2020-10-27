# Using Apache as an SSL Gateway to multiple x11vnc servers inside a firewall

Background:

The typical way to allow access to x11vnc (or any other VNC server)
running on multiple workstations inside a firewall is via SSH. The
user somewhere out on the Internet logs in to the SSH gateway machine
and uses port forwarding (e.g. ssh -t -L 5900:myworkstation:5900
user@gateway) to set up the encrypted channel that VNC is then
tunneled through. Next he starts up the VNC viewer on the machine
where he is sitting directed to the local tunnel port (e.g.
localhost:0).

The SSH scheme is nice because it is a widely used and well tested
login technique for users connecting to machines inside their company
or home firewall. For VNC access it is a bit awkward, however, because
SSH needs to be installed on the Viewer machine and the user usually
has to rig up his own port redirection plumbing (however, see our
other tool).

Also, some users have restrictive work environments where SSH and
similar applications are prohibited (i.e. only outgoing connections to
standard WWW ports from a browser are allowed, perhaps mediated by a
proxy server). These users have successfully used the method described
here for remote access.

With the SSL support in x11vnc and the SSL enabled Java VNC viewer
applet, a convenient and secure alternative exists that uses the
Apache webserver as a gateway. The idea is that the company or home
internet connection is already running apache as a web server (either
SSL or non-SSL) and we add to it the ability to act as a gateway for
SSL VNC connections. The only thing needed on the Viewer side is a
Java enabled Web Browser: the user simply enters a URL that starts the
entire VNC connection process. No VNC or SSH specific software needs
to be installed on the viewer side machine.

The stunnel VNC viewer stunnel wrapper script provided (ss_vncviewer)
can also take advantage of the method described here with its -proxy
option.

---

Simpler Solutions: This apache SSL VNC portal solution may be too much
for you. It is mainly intended for automatically redirecting to
MULTIPLE workstations inside the firewall. If you only have one or two
inside machines that you want to access, the method described here is
overly complicated! See below for some simpler (and still non-SSH)
encrypted setups.

Also see the recent (Mar/2010) desktop.cgi x11vnc desktop web login
CGI script that achieves much of what the method describes here
(especially if its 'port redirection' feature is enabled.)

---

There are numerous ways to achieve this with Apache. We present one of
the simplest ones here.

Important: these sorts of schemes allow incoming connections from
anywhere on the Internet to fixed ports on machines inside the
firewall. Care must be taken to implement and test thoroughly. If one
is paranoid one can (and should) add extra layers of protection. (e.g.
extra passwords, packet filtering, SSL certificate verification, etc).

Also, it is easy to miss the point that unless precautions are taken
to verify SSL Certificates, then the VNC Viewer is vulnerable to
man-in-the-middle attacks (but not to the more common passive sniffing
attacks). Note that there are hacker tools like dsniff/webmitm and
cain that implement SSL Man-In-The-Middle attacks. They rely on the
client not bothering to check the cert.

---

The Holy Grail: a single https port (443)

Before we discuss the self-contained apache examples here, we want to
mention that many x11vnc users who read this page and implement the
apache SSL VNC portal ask for something that (so far) seems difficult
or impossible to do entirely inside apache:

* A single port, 443 (the default https:// port), is open to the
  Internet
* It is HTTPS/SSL encrypted
* It handles both VNC traffic and Java VNC Applet downloads.
* And the server can also serve normal HTTPS webpages, CGI, etc.

It is the last item that makes it tricky (otherwise the method
described on this page will work). If you are interested in such a
solution and are willing to run a separate helper program
(connect_switch) look here. Also, see this apache patch.

---

Example:

The scheme described here sets up apache on the firewall/gateway as a
regular Web proxy into the intranet and allows connections to a single
fixed port on a limited set of machines.

The configuration described in this section does not use the mod_ssl
apache module (the optional configuration described in the section
"Downloading the Java applet to the browser via HTTPS" does take
advantage of mod_ssl)

In this example suppose the gateway machine running apache is named
"www.gateway.east" (e.g. it may also provide normal web service). We
also choose the Internet-facing port for this VNC service to be port
563. One could choose any port, including the default HTTP port 80.

Detail: We choose 563 because it is the rarely used SNEWS port that is
often allowed by Web proxies for the CONNECT method. The idea is the
user may be coming out of another firewall using a proxy (not the one
we describe here, that is, the case when two proxies are involved,
e.g. one at work and another Apache (described here) at home
redirecting into our firewall; the "double proxy" or "double firewall"
problem). Using port 563 simplifies things because CONNECT's to it are
usually allowed by default.

We also assume all of the x11vnc servers on the internal machines are
all listening on port 5915 ("-rfbport 5915") instead of the default
5900. This is to limit any unintended proxy redirections to a lesser
used port, and also to stay out of the way of normal VNC servers on
the same machines. One could obviously implement a scheme that handles
different ports, but we just discuss this simple setup here.

So we basically assume x11vnc has been started this way on all of the
workstations to be granted VNC access:

```
x11vnc -ssl SAVE -http -display :0 -forever -rfbauth ~/.vnc/passwd -rfbport 5915
```

i.e. we force SSL VNC connections, port 5915, serve the Java VNC
viewer applet, and require a VNC password (another option would be
-unixpw). The above command could also be run out of inetd(8). It can
also be used to autodetect the user's display and Xauthority data.

These sections are added to the httpd.conf apache configuration file
on www.gateway.east:

```
# In the global section you need to enable these modules.
# Note that the ORDER MATTERS! mod_rewrite must be before mod_proxy
# (so that we can check the allowed host list via rewrite)
#
LoadModule rewrite_module modules/mod_rewrite.so
LoadModule proxy_module modules/mod_proxy.so
LoadModule proxy_connect_module modules/mod_proxy_connect.so
LoadModule proxy_ftp_module modules/mod_proxy_ftp.so
LoadModule proxy_http_module modules/mod_proxy_http.so
<IfDefine SSL>
LoadModule ssl_module modules/mod_ssl.so
</IfDefine>


# Near the bottom of httpd.conf you put the port 563 virtual host:

Listen 563

<VirtualHost *:563>

   # Allow proxy CONNECT requests *only* to port 5915.
   # If the machines use different ports, e.g. 5916 list them here as well:
   #
   ProxyRequests On
   AllowCONNECT 5915

   RewriteEngine On

   # Convenience rules to expand applet parameters.  These do not have a trailing "/"
   #
   # /vnc   for http jar file downloading:
   #
   RewriteRule /vnc/([^/]+)$               /vnc/$1/index.vnc?CONNECT=$1+5915&PORT=563&urlPrefix=_2F_vnc_2F_$1 [R,NE,L]
   RewriteRule /vnc/trust/([^/]+)$         /vnc/$1/index.vnc?CONNECT=$1+5915&PORT=563&urlPrefix=_2F_vnc_2F_$1&trustAllVncCerts=yes [R,NE,L]
   RewriteRule /vnc/proxy/([^/]+)$         /vnc/$1/proxy.vnc?CONNECT=$1+5915&PORT=563&urlPrefix=_2F_vnc_2F_$1&forceProxy=yes [R,NE,L]
   RewriteRule /vnc/trust/proxy/([^/]+)$   /vnc/$1/proxy.vnc?CONNECT=$1+5915&PORT=563&urlPrefix=_2F_vnc_2F_$1&forceProxy=yes&trustAllVncCerts=yes [R,NE,L]

   # Read in the allowed host to vnc display mapping file.  It looks like:
   #
   #   host1     15
   #   host2     15
   #   ...
   #
   # the display "15" means 5815 for http applet download, 5915 for SSL vnc.
   #
   RewriteMap vnchosts txt:/dist/apache/conf/vnc.hosts

   # Proxy: check for the CONNECT hostname and port being in the vnc.hosts list.
   #
   RewriteCond %{THE_REQUEST} ^CONNECT [NC]
   RewriteCond %{REQUEST_URI} ^(.*):(.*)$
   RewriteCond ${vnchosts:%1|NOTFOUND} NOTFOUND
   RewriteRule ^.*$ /VNCFAIL [F,L]

   RewriteCond %{THE_REQUEST} ^CONNECT [NC]
   RewriteCond %{REQUEST_URI} ^(.*):(.*)$
   RewriteCond 59${vnchosts:%1}=%2 !^(.*)=(\1)$
   RewriteRule ^.*$ /VNCFAIL [F,L]


   # Remap /vnc to the proxy http download (e.g. http://host:5815)
   #
   # First, fail if it starts with the string /vnc0:
   #
   RewriteRule ^/vnc0.*            /VNCFAIL [F,L]
   #
   # Next, map the prefix to /vnc0/host:protocol:port
   #
   RewriteRule ^/vnc/([^/]+)/(.*)  /vnc0/$1:http:58${vnchosts:$1|NOTFOUND}/$2  [NE]
   #
   # Drop any not found:
   #
   RewriteRule ^/vnc0.*NOTFOUND.*  /VNCFAIL [F,L]

   # Construct the proxy URL and retrieve it:
   #
   RewriteRule ^/vnc0/([^/]+):([^/]+):([^/]+)/(.*) $2://$1:$3/$4 [P,NE,L]

</VirtualHost>
```

Then restart apache (perhaps: "apachectl stop; apachectl start").

Note that the listing of allowed internal workstations is done in an
external file (/dist/apache/conf/vnc.hosts in the example above), the
format is like this:

```
# allowed vnc hosts file:
hostname1  15
hostname2  15
...
```

You list the hostname and the VNC display (always 15 in our example).
Only to these hosts will the external VNC viewers be able to connect
to (via the HTTP CONNECT method).

The above setup requires mod_rewrite and mod_proxy be enabled in the
apache web server. In this example they are loaded as modules (and
note that mod_rewrite must be listed before mod_proxy);

The user at the Java enabled Web browser would simply enter this URL
into the browser:
http://www.gateway.east:563/vnc/host2

to connect to internal workstation host2, etc.

Important: do not put a trailing "/" on the URL, since that will
defeat the RewriteRules that look for the hostname at the very end.

There will be a number of SSL certificate, etc, dialogs he will have
to respond to in addition to any passwords he is required to provide
(this depends on how you set up user authentication for x11vnc).

If a second Web proxy is involved (i.e. the user's browser is inside
another firewall and policy requires using a Web proxy server) then
use this URL:
http://www.gateway.east:563/vnc/proxy/host2

This will involve downloading a signed java viewer applet jar file
that is able to interact with the internal proxy for the VNC
connection. See this FAQ for more info on how this works. Note:
sometimes with the Proxy case if you see 'Bad Gateway' error you will
have to wait 10 or so seconds and then hit reload. This seems to be
due to having to wait for a Connection Keepalive to terminate...

For completeness, the "trust" cases that skip a VNC certificate dialog
(discussed below) would be entered as:

```
http://www.gateway.east:563/vnc/trust/host2
http://www.gateway.east:563/vnc/trust/proxy/host2
```

You can of course choose shorter or more easy to remember URL formats.
Just change the Convenience RewriteRules in httpd.conf.

---

Port Variations:

Note that you can run this on the default HTTP port 80 instead of port
563. If you do not expect to have a browser connecting from inside a
proxying firewall (where sometimes only connections to ports 443 and
563 are allowed) this should be fine. Use "80" instead of "563" in the
httpd.conf config file (you may need to merge it with other default
port 80 things you have there).

Then the URL's will be a bit simpler:

```
http://www.gateway.east/vnc/host2
http://www.gateway.east/vnc/trust/host2
```

etc.

Besides 80 one could use any other random port number (since there are
so many port scans on 80, a little obscurity might be useful).

One option is to use port "443" (the default https:// port) instead of
"563". In this case Apache is not configured for mod_ssl; we just
happen to use port "443" in the way any random port would be used.
This could be handy if the Viewer side environment is restrictive in
that it only allows outgoing connections to ports 80 and 443 (and,
say, you didn't want to use port 80, or you wanted to use 80 for
something else). Another reason for using 443 would be some web proxy
environments only allow the CONNECT method to go to port 443 (and not
even the case 563 we use above).

---

Details:

Let's go through the httpd.conf additions in detail from the top.

The LoadModules directives load the necessary apache modules. Note
that mod_rewrite must be listed first. If you are compiling from
scratch something like this worked for us:

```
./configure --enable-proxy=shared --enable-proxy-connect=shared --enable-ssl=shared --enable-rewrite=shared --prefix=/dist/apache
```

Then the VirtualHost *:563 virtual host section starts.

The "ProxyRequests On" and "AllowCONNECT 5915" enable the web server
to forward proxy requests to port 5915 (and only this port) INSIDE the
firewall. Think about the implications of this thoroughly and test it
carefully.

The RewriteRule's are for convenience only so that the URL entered
into the Web browser does not need the various extra parameters, e.g.:

```
http://www.gateway.east:563/vnc/host2/index.vnc?CONNECT=host2+5915&PORT=563,blah,blah...
```

(or otherwise make direct edits to index.vnc to set these parameters).
The forceProxy=yes parameter is passed to the applet to force the use
of a outgoing proxy socket connection. Use it only if the Web browser
is inside a separate Web proxying environment (i.e. large corporation)

The rewrites with parameter urlPrefix are described under Tricks for
Better Response. The "trust" ones (also described under Tricks) with
trustAllVncCerts tell the Java VNC applet to skip a dialog asking
about the VNC Certificate. They are a bit faster and more reliable
than the original method. In the best situation they lead to being
logged in 20 seconds or less (without them the time to login can be
much longer since a number of connections must timeout).

All of the x11vnc Java Viewer applet parameters are described in the
file classes/ssl/README

The external file /dist/apache/conf/vnc.hosts containing the allowed
VNC server hostnames is read in. Its 2nd column contains the VNC
display of the host (always 15 in our example; if you make it vary you
will need to adjust some lines in the httpd.conf accordingly, e.g.
AllowCONNECT). This list is used to constrain both the Jar file
download URL and the proxy CONNECT the VNC viewer makes to only the
intended VNC servers.

Limiting the proxy CONNECT is done with the two sets of RewriteCond
conditions.

Limiting the Jar file download URL is done in the remaining 4
RewriteRule's.

Note that these index.vnc and VncViewer.jar downloads to the browser
are not encrypted via SSL, and so in principle could be tampered with
by a really bad guy. The subsequent VNC connection, however, is
encrypted through a single SSL connection (it makes a CONNECT straight
to x11vnc). See below for how to have these initial downloads
encrypted as well (if the apache web server has SSL/mod_ssl, i.e.
https, enabled and configured).

Unfortunately the Java VNC viewer applet currently is not able to save
its own list of Certificates (e.g. the user says trust this VNC
certificate 'always'). This is because an applet it cannot open local
files, etc. Sadly, the applet cannot even remember certificates in the
same browser session because it is completely reinitialized for each
connection (see below).

---

Too Much?

If these apache rules are a little too much for you, there is a little
bit simpler scheme where you have to list each of the individual
machines in the httpd.conf and ssl.conf files. It may be a little more
typing to maintain, but perhaps being more straight forward (less
RewriteRule's) is desirable.

---

Problems?

To see example x11vnc output for a successful https://host:5900/
connection with the Java Applet see This Page.

---

Some Ideas for adding extra authentication, etc. for the paranoid:

* VNC passwords: -rfbauth, -passwdfile, or -usepw. Even adding a
  simple company-wide VNC password helps block unwanted access.
* Unix passwords: -unixpw
* SSL Client certificates: -sslverify
* Apache AuthUserFile directive: .htaccess, etc.
* Filter connections based on IP address or hostname.
* Use Port-knocking on your firewall as described in: Enhanced
  TightVNC Viewer (ssvnc).
* Add proxy password authentication (requires Viewer changes?)
* Run a separate instance of Apache that provides this VNC service
  so it can be brought up and down independently of the normal web
  server.
* How secure is the Client side? Public machines in internet cafes,
  etc, are often hacked, with backdoors and VNC servers of their
  own. Prefer using your own firewalled laptop to a public machine.

---

Using non-Java viewers with this scheme:

The ss_vncviewer stunnel wrapper script for VNC viewers has the -proxy
option that can take advantage of this method.

```
ss_vncviewer -proxy www.gateway.east:563   host1:15
```

For the case of the "double proxy" situation (see below) supply both
separated by a comma.

```
ss_vncviewer -proxy proxy1.foobar.com:8080,www.gateway.east:563   host1:15
```

For the Enhanced TightVNC Viewer (ssvnc) GUI (it uses ss_vncviewer on
Unix) put 'host1:15' into the 'VNC Server' entry box, and here are
possible Proxy/Gateway entries

```
Proxy/Gateway:   www.gateway.east:563
Proxy/Gateway:   proxy1.foobar.com:8080,www.gateway.east:563
```

then click on the 'Connect' button.

---

Downloading the Java applet to the browser via HTTPS:

To have the Java applet downloaded to the user's Web Browser via an
encrypted (and evidently safer) SSL connection the Apache webserver
should be configured for SSL via mod_ssl.

It is actually possible to use the x11vnc Key Management utility
"-sslGenCert" to generate your Apache/SSL .crt and .key files. (In
brief, run something like "x11vnc -sslGenCert server self:apache" then
copy the resulting self:apache.crt file to conf/ssl.crt/server.crt and
extract the private key part from self:apache.pem and paste it into
conf/ssl.key/server.key). Setting the env var REQ_ARGS='-days 1095'
before running x11vnc will bump up the expiration date (3 years in
this case).

Or you can use the standard methods described in the Apache mod_ssl
documentation to create your keys. Then restart Apache, usually
something like "apachectl stop" followed by "apachectl startssl"

In addition to the above sections in httpd.conf one should add the
following to ssl.conf:

```
   SSLProxyEngine  On

   RewriteEngine On

   # Convenience rules to expand applet parameters.  These do not have a traili
ng "/"
   #
   # /vnc   http jar file downloading:
   #
   RewriteRule /vnc/([^/]+)$                        /vnc/$1/index.vnc?CONNECT=$1+5915&PORT=563&httpsPort=443&GET=1&urlPrefix=_2F_vnc_2F_$1 [R,NE,L]
   RewriteRule /vnc/proxy/([^/]+)$                  /vnc/$1/proxy.vnc?CONNECT=$1+5915&PORT=563&httpsPort=443&GET=1&urlPrefix=_2F_vnc_2F_$1&forceProxy=yes [R,N
E,L]
   #
   # (we skipped the "trust" ones above, put them in if you like)
   #
   # /vncs  https jar file downloading:
   #
   RewriteRule /vncs/([^/]+)$                      /vncs/$1/index.vnc?CONNECT=$1+5915&PORT=563&httpsPort=443&GET=1&urlPrefix=_2F_vncs_2F_$1 [R,NE,L]
   RewriteRule /vncs/proxy/([^/]+)$                /vncs/$1/proxy.vnc?CONNECT=$1+5915&PORT=563&httpsPort=443&GET=1&urlPrefix=_2F_vncs_2F_$1&forceProxy=yes [R,NE,l]
   RewriteRule /vncs/trust/([^/]+)$                /vncs/$1/index.vnc?CONNECT=$1+5915&PORT=563&httpsPort=443&GET=1&urlPrefix=_2F_vncs_2F_$1&trustAllVncCerts=yes [R,NE,L]
   RewriteRule /vncs/trust/proxy/([^/]+)$          /vncs/$1/proxy.vnc?CONNECT=$1+5915&PORT=563&httpsPort=443&GET=1&urlPrefix=_2F_vncs_2F_$1&forceProxy=yes&trustAllVncCerts=yes [R,NE,L]

   # Convenience rules used for the connect_switch helper (requires Listen 127.0.0.1:443 above):
   #
   RewriteRule /vnc443/([^/]+)$                    /vncs/$1/index.vnc?CONNECT=$1+5915&PORT=443&httpsPort=443&GET=1&urlPrefix=_2F_vncs_2F_$1 [R,NE,L]
   RewriteRule /vnc443/proxy/([^/]+)$              /vncs/$1/proxy.vnc?CONNECT=$1+5915&PORT=443&httpsPort=443&GET=1&urlPrefix=_2F_vncs_2F_$1&forceProxy=yes [R,NE,L]
   RewriteRule /vnc443/trust/([^/]+)$              /vncs/$1/index.vnc?CONNECT=$1+5915&PORT=443&httpsPort=443&GET=1&urlPrefix=_2F_vncs_2F_$1&trustAllVncCerts=yes [R,NE,L]
   RewriteRule /vnc443/trust/proxy/([^/]+)$        /vncs/$1/proxy.vnc?CONNECT=$1+5915&PORT=443&httpsPort=443&GET=1&urlPrefix=_2F_vncs_2F_$1&forceProxy=yes&trustAllVncCerts=yes [R,NE,L]

   # Read in the allowed host to vnc display mapping file.  It looks like:
   #
   #   host1     15
   #   host2     15
   #   ...
   #
   # the display "15" means 5915 for SSL VNC and 5815 for http applet download.
   #
   RewriteMap vnchosts txt:/dist/apache/conf/vnc.hosts


   # Remap /vnc and /vncs to the proxy http download (e.g. https://host:5915)
   #
   # First, fail if it starts with the string /vnc0:
   #
   RewriteRule ^/vnc0.*            /VNCFAIL [F,L]
   #
   # Next, map the prefix to /vnc0:host:protocol:port
   #
   RewriteRule ^/vnc/([^/]+)/(.*)  /vnc0/$1:http:58${vnchosts:$1|NOTFOUND}/$2  [NE]
   RewriteRule ^/vncs/([^/]+)/(.*) /vnc0/$1:https:59${vnchosts:$1|NOTFOUND}/$2  [NE]
   #
   # Drop any not found:
   #
   RewriteRule ^/vnc0.*NOTFOUND.*  /VNCFAIL [F,L]

   # Construct the proxy URL and retrieve it:
   #
   RewriteRule ^/vnc0/([^/]+):([^/]+):([^/]+)/(.*) $2://$1:$3/$4 [P,NE,L]
```

This is all in the "<VirtualHost _default_:443>" section of ssl.conf.

The user could then point the Web Browser to:

```
https://www.gateway.east/vnc/host2
```

or

```
https://www.gateway.east/vnc/proxy/host2
```

for the "double proxy" case. (Important: do not put a trailing "/" on
the URL, since that will defeat the RewriteRules.)

As with the httpd.conf case, the external file
(/dist/apache/conf/vnc.hosts in the above example) contains the
hostnames of the allowed VNC servers.

Note that inside the firewall the Java applet download traffic is not
encrypted (only over the Internet is SSL used) for these cases:

```
https://www.gateway.east/vnc/host2
https://www.gateway.east/vnc/proxy/host2
```

However for the special "vncs" rules above:

```
https://www.gateway.east/vncs/host2
```

the Java applet download is encrypted via SSL for both legs. Note that
the two legs are two separate SSL sessions. So the data is decrypted
inside an apache process and reencrypted by the apache process for the
2nd SSL session inside the same apache process (a very small gap one
might overlook).

The "vncs/trust" ones are like the "trust" ones described earlier

```
https://www.gateway.east/vncs/trust/mach2
```

and similarly for the httpsPort ones. See Tricks for Better Response.

In all of the above cases the VNC traffic from Viewer to x11vnc is
encrypted end-to-end in a single SSL session, even for the "double
proxy" case because the CONNECT method is used (there are actually two
CONNECT's for the "double proxy" case). This part (the VNC traffic) is
the most important part to have encrypted.

Note that the Certificate dialogs the user has in his web browser will
be for the Apache Certificate, while for the Java applet it will be
the x11vnc certificate.

Note also that you can have Apache serve up the Jar file VncViewer.jar
and/or index.vnc/proxy.vnc instead of each x11vnc if you want to.

The rules in ssl.conf are similar to the ones in httpd.conf and so are
not discussed in detail. The only really new thing is the /vncs
handling to download the applet jar via HTTPS on port 5915.

The special entries "/vnc443" are only used for the special helper
program (connect_switch) for the https port 443 only mode discussed
here.

---

INETD automation:

The "single-port" (i.e. 5915) HTTPS applet download and VNC connection
aspect shown here is convenient and also enables having x11vnc run out
of inetd. That way x11vnc is run on demand instead of being run all
the time (the user does not have to remember to start it). The first
connections to inetd download index.vnc and the Jar file (via https)
and the the last connection to inetd establishes the SSL VNC
connection. Since x11vnc is restarted for each connection, this will
be a bit slower than the normal process.

For example, the /etc/inetd.conf line could be:

```
5915 stream tcp nowait root /usr/sbin/tcpd /usr/local/bin/x11vnc_ssl.sh
```

where the script x11vnc_ssl.sh looks something like this:

```
#!/bin/sh

/usr/local/bin/x11vnc -inetd -oa /var/log/x11vnc-15.log \
        -ssl SAVE -http -unixpw -localhost \
        -display :0 -auth /home/THE_USER/.Xauthority
```

where, as usual, the inetd launching needs to know which user is
typically using the display on that machine. One could imagine giving
different users different ports, 5915, 5916, etc. to distinguish (then
the script would need to be passed the username). mod_rewrite could be
used to automatically map username in the URL to his port number.

A better way is to use the "-display WAIT:cmd=FINDDISPLAY" feature to
autodetect the user and Xauthority data:

```
#!/bin/sh

/usr/local/bin/x11vnc -inetd -oa /var/log/x11vnc-15.log \
        -ssl SAVE -http -unixpw -localhost -users unixpw= \
        -find
```

(we have used the alias -find for "-display WAIT:cmd=FINDDISPLAY".)
This way the user must supply his Unix username and password and then
his display and Xauthority data on that machine will be located and
returned to x11vnc to allow it to attach. If he doesn't have a display
running on that machine or he fails to log in correctly, the
connection will be dropped.

The variant "-display WAIT:cmd=FINDCREATEDISPLAY" (aliased by
"-create") will actually create a (virtual or real) X server session
for the user if one doesn't already exist. See here for details.

To enable inetd operation for the non-HTTPS Java viewer download (port
5815 in the above httpd.conf example) you will need to run x11vnc in
HTTPONCE mode on port 5815: For example, the /etc/inetd.conf line
could be:

```
5815 stream tcp nowait root /usr/sbin/tcpd /usr/local/bin/x11vnc \
     -inetd -prog /usr/local/bin/x11vnc -oa /var/log/x11vnc-15.log \
     -http_ssl -display WAIT:cmd=HTTPONCE
```

where the long inetd.conf line has been split. Note how the -http_ssl
tries to automatically find the .../classes/ssl subdirectory. This
requires the -prog option available in x11vnc 0.8.4 (a shell script
wrapper, e.g. /usr/local/bin/x11vnc_http.sh can be used to work around
this).

Also note the use of "-ssl SAVE" above. This way a saved server.pem is
used for each inetd invocation (rather generating a new one each time
as happens for "-ssl TMP"). Note that it cannot have a protecting
passphrase because inetd will not be able to supply it.

Another option is:

```
5815 stream tcp nowait root /usr/sbin/tcpd /usr/local/bin/x11vnc \
     -inetd -httpdir /usr/local/share/x11vnc/classes/ssl \
     -oa /var/log/x11vnc-15.log -display WAIT:cmd=HTTPONCE
```

(this also requires a feature found in x11vnc 0.8.4).

---

Other Ideas:

- The above schemes work, but they are a bit complicated with all of
  the rigging. There should be more elegant ways to configure Apache to
  do these, but we have not found them (please let us know if you
  discover something nice). However, once this scheme has been set up
  and is working it is easy to maintain and add/delete workstations,
  etc.
- In general Apache is not required, but it makes things convenient.
  The firewall itself could do the port redirection via its firewall
  rules. Evidently different Internet-facing ports would be required for
  each workstation. This could be set up using iptables rules for
  example. If there were just one or two machines this would be the
  easiest method. For example:

    ```
    iptables -t nat -A PREROUTING -p tcp -d 24.35.46.57 --dport 5901 -j DNAT --to-destination 192.168.1.2:5915
    iptables -t nat -A PREROUTING -p tcp -d 24.35.46.57 --dport 5902 -j DNAT --to-destination 192.168.1.3:5915
    ```

    Where 24.35.46.57 is the internet IP address of the gateway. In this
    example 24.35.46.57:5901 is redirected to the internal machine
    192.168.1.2:5915 and 24.35.46.57:5902 is redirected to another
    internal machine 192.168.1.3:5915, both running x11vnc -ssl ... in SSL
    mode. For this example, the user would point the web browser to, e.g.:

    ```
    https://24.35.46.57:5901/?PORT=5901
    ```

    or using the stunnel wrapper script:

    ```
    ss_vncviewer 24.35.46.57:1
    ```

    One can achieve similar things with dedicated firewall/routers (e.g.
    Linksys) using the device's web or other interface to configure the
    firewall.

    If the user may be coming out of a firewall using a proxy it may be
    better to redirect ports 443 and 563 (instead of 5901 and 5902) to the
    internal machines so that the user's proxy will allow CONNECTing to
    them.

- The redirection could also be done at the application level using a
  TCP redirect program (e.g. ip_relay or fancier ones). Evidently more
  careful internal hostname checking, etc., could be performed by the
  special purpose application to add security. See connect_switch which
  is somewhat related.
- One might imagine the ProxyPass could be done for the VNC traffic as
  well (for the ssl.conf case) to avoid the CONNECT proxying completely
  (which would be nice to avoid). Unfortunately we were not able to get
  this to work. Since HTTP is a request-response protocol (as opposed to
  a full bidirectional link required by VNC that CONNECT provides) this
  makes it difficult to do. It may be possible, but we haven't found out
  how yet.

All of the x11vnc Java Viewer applet parameters are described in the
file classes/ssl/README

---

Tricks for Better Response and reliability:

The "original scheme" using httpd.conf and ssl.conf rewrites without
urlPrefix and trustAllVncCerts above should work OK, but may lead to
slow and/or unreliable loading of the applet and final connection to
x11vnc. The following are what I do now to get better response and
reliability. YMMV.

The problem with the "original scheme" is that there is a point where
the VNC Viewer applet can try up to 3 times to retrieve the x11vnc
certificate, since it needs to get it to show it to you and ask you if
you accept it. This can add about 45 seconds to the whole process
(which takes 1 to 1.5 minutes with all the dialogs) since a couple of
those connections must time out. The "trust" items in the config add a
parameter trustAllVncCerts=yes similar to the forceProxy=yes
parameter. This can cut the total time to the VNC password prompt down
to 15 seconds which is pretty good. (Note by ignoring the certificate
this does not protect against man-in-the-middle attacks which are
rare, but maybe the won't be so rare in the future... see
dsniff/webmitm and cain)

First make sure the x11vnc SSL certificate+key is the same as
Apache's. (otherwise you may get one extra dialog and/or one extra
connection that has to time out).

The following RewriteRule's are the same now advocated in the
instructions above.

The httpsPort and urlPrefix= parameters give hints to the applet to
improve connecting: This is what goes in httpd.conf:

```
RewriteEngine On
RewriteRule /vnc/([^/]+)$               /vnc/$1/index.vnc?CONNECT=$1+5915&PORT=563&urlPrefix=_2F_vnc_2F_$1 [R,NE]
RewriteRule /vnc/trust/([^/]+)$         /vnc/$1/index.vnc?CONNECT=$1+5915&PORT=563&urlPrefix=_2F_vnc_2F_$1&trustAllVncCerts=yes [R,NE]
RewriteRule /vnc/proxy/([^/]+)$         /vnc/$1/proxy.vnc?CONNECT=$1+5915&PORT=563&urlPrefix=_2F_vnc_2F_$1&forceProxy=yes [R,NE]
RewriteRule /vnc/trust/proxy/([^/]+)$   /vnc/$1/proxy.vnc?CONNECT=$1+5915&PORT=563&urlPrefix=_2F_vnc_2F_$1&forceProxy=yes&trustAllVncCerts=yes [R,NE]
```

The httpsPort and urlPrefix provide useful hints to the VNC Viewer
applet when it connects to x11vnc to glean information about Proxies,
certificates, etc.

This is what goes into ssl.conf:

```
RewriteEngine On
RewriteRule /vnc/([^/]+)$                /vnc/$1/index.vnc?CONNECT=$1+5915&PORT=563&httpsPort=443&GET=1&urlPrefix=_2F_vnc_2F_$1 [R,NE]
RewriteRule /vnc/proxy/([^/]+)$          /vnc/$1/proxy.vnc?CONNECT=$1+5915&PORT=563&httpsPort=443&GET=1&urlPrefix=_2F_vnc_2F_$1&forceProxy=yes [R,NE]
RewriteRule /vncs/([^/]+)$              /vncs/$1/index.vnc?CONNECT=$1+5915&PORT=563&httpsPort=443&GET=1&urlPrefix=_2F_vncs_2F_$1 [R,NE]
RewriteRule /vncs/proxy/([^/]+)$        /vncs/$1/proxy.vnc?CONNECT=$1+5915&PORT=563&httpsPort=443&GET=1&urlPrefix=_2F_vncs_2F_$1&forceProxy=yes [R,NE]
RewriteRule /vncs/trust/([^/]+)$        /vncs/$1/index.vnc?CONNECT=$1+5915&PORT=563&httpsPort=443&GET=1&urlPrefix=_2F_vncs_2F_$1&trustAllVncCerts=yes [R,NE]
RewriteRule /vncs/trust/proxy/([^/]+)$  /vncs/$1/proxy.vnc?CONNECT=$1+5915&PORT=563&httpsPort=443&GET=1&urlPrefix=_2F_vncs_2F_$1&forceProxy=yes&trustAllVncCerts=yes [R,NE]
```

The rest is the same.

The httpsPort and urlPrefix and GET provide useful hints to the VNC
Viewer applet when it connects to x11vnc to glean information about
Proxies, certificates, etc, and also for the ultimate VNC connection
(GET speeds this up by sending a special HTTP GET to cause x11vnc to
immediately switch to the VNC protocol).

To turn these into URLs, as was done above, take the string in the
RewriteRule, e.g. /vncs and turn it into
https://gateway/vncs/machinename Similarly for non-https:
http://gateway:563/vnc/machinename

If you use the 'trust' ones, you are performing NO checks, visual or
otherwise, on the VNC SSL certificate. It is trusted without question.
This speeds things up because it avoids a dialog about certificates,
but of course has some risk WRT Man in the Middle attacks. I don't
recommend them. It is better to use /vnc or /vncs and the first time
you connect carefully check the Certificate and then tell your Browser
and Java Virtual Machine to trust the certificate 'Always'. Then if
you later get an unexpected dialog, you know something is wrong.
Nearly always it is just a changed or expired certificate, but better
safe than sorry...
