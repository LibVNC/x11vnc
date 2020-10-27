# Notes on x11vnc SSL Certificates and Key Management

The simplest scheme ("x11vnc -ssl TMP") is where x11vnc generates a
temporary, self-signed certificate each time (automatically using
openssl(1)) and the VNC viewer client accepts the certificate without
question (e.g. user clicks "Yes" in a dialog box. Perhaps the dialog
allows them to view the certificate too). Also note stunnel's default
is to quietly accept all certificates.

The encryption this provides protects against all passive sniffing of
the VNC traffic and passwords on the network and so it is quite good,
but it does not prevent a Man-In-The-Middle active attack: e.g. an
attacker intercepts the VNC client stream and sends it his own Public
key for SSL negotiation (pretending to be the server). Then it makes a
connection to SSL x11vnc itself and forwards the data back and forth.
He can see all the traffic and modify it as well.

Most people don't seem to worry about Man-In-The-Middle attacks these
days; they are more concerned about passive sniffing of passwords,
etc. Perhaps someday that will change if attack tools are used more
widely to perform the attack. NOTE: There are hacker tools like
dsniff/webmitm and cain that implement SSL Man-In-The-Middle attacks.
They all rely on the client not bothering to check that the cert is
valid.

If you are not worried about Man-In-The-Middle attacks you do not have
to read the techniques described in the rest of this document.

To prevent Man-In-The-Middle attacks, certificates must somehow be
verified. This requires the VNC client side have some piece of
information that can be used to verify the SSL x11vnc server.
Alternatively, although rarely done, x11vnc can verify VNC Clients'
certificates, see the -sslverify option that is discussed below.

There are a number of ways to have the client authenticate the SSL
x11vnc server. The quickest way perhaps would be to copy (safely) the
certificate x11vnc prints out:

```
26/03/2006 21:12:00 Creating a temporary, self-signed PEM certificate...
...
-----BEGIN CERTIFICATE-----
MIIC4TCCAkqgAwIBAgIJAMnwCaOjvEKaMA0GCSqGSIb3DQEBBAUAMIGmMQswCQYD
VQQGEwJBVTEOMAwGA1UEBxMFTGludXgxITAfBgNVBAsTGGFuZ2VsYS0xMTQzNDI1
NTIwLjQxMTE2OTEPMA0GA1UEChMGeDExdm5jMS4wLAYDVQQDEyV4MTF2bmMtU0VM
(more lines) ...
-----END CERTIFICATE-----
```

to the client machine(s) and have the client's SSL machinery (e.g.
stunnel, Web Browser, or Java plugin) import the certificate. That way
when the connection to x11vnc is made the client can verify that is it
the desired server on the other side of the SSL connection.

So, for example suppose the user is using the SSL enabled Java VNC
Viewer and has incorporated the x11vnc certificate into his Web
browser on the viewing side. If he gets a dialog that the certificate
is not verified he knows something is wrong. It may be a
Man-In-The-Middle attack, but more likely x11vnc certificate has
changed or expired or his browser was reinstalled and/or lost the
certificate, etc, etc.

As another example, if the user was using stunnel with his VNC viewer
(this is mentioned in this FAQ), e.g. STUNNEL.EXE on Windows, then he
would have to set the "CAfile = path-to-the-cert" and "verify = 2"
options in the stunnel.conf file before starting up the tunnel. If a
x11vnc certificate cannot be verified, stunnel will drop the
connection (and print a failure message in its log file).

A third example, using the VNC viewer on Unix with stunnel the wrapper
script can be used this way: "ss_vncviewer -verify ./x11vnc.crt
far-away.east:0" where ./x11vnc.crt is the copied certificate x11vnc
printed out.

As fourth example, our SSVNC enhanced tightvnc viewer can also use
these certificate files for server authentication. You can load them
via the SSVNC 'Certs...' dialog and set 'ServerCert' to the
certificate file you safely copied there.

Note that in principle the copying of the certificate to the client
machine(s) itself could be altered by a Man-In-The-Middle attack! You
can't win; it is very difficult to be completely secure. It is
unlikely the attacker could predict how you were going to send it
unless you had, say, done it many times before the same way. SSH is a
very good way to send it (but of course it too depends on public keys
being sent unaltered between the two machines!).

If you are really paranoid, I'm sure you'll figure out a really good
way to transport the certificates. See the Certificate Authority
scheme below for a way to make this easier (you just have to do it
once).

---

## Saving SSL certificates and keys

Now, it would be very inconvenient to copy the new temporary
certificate every time x11vnc is run in SSL mode. So for convenience
there is the "SAVE" keyword to instruct x11vnc to save the certificate
it creates:

```
x11vnc -ssl SAVE -display :0 ...
```

This behavior is now the default, you must use "TMP" for a temporary
one. It will save the certificate and private key in these files:

```
~/.vnc/certs/server.crt
~/.vnc/certs/server.pem
```

The ".crt" file contains only the certificate and should be safely
copied to the VNC Viewer machine(s) that will be authenticating the
x11vnc server. The ".pem" file contains both the certificate and the
private key and should be kept secret. (If you don't like the default
location ~/.vnc/certs, e.g. it is on an NFS share and you are worried
about local network sniffing, use the -ssldir dir option to point to a
different directory.)

So the next time you run "x11vnc -ssl SAVE ..." it will read the
server.pem file directly instead of creating a new one.

You can manage multiple SSL x11vnc server keys in this simple way by
using:

```
x11vnc -ssl SAVE-key2 -display :0 ...
```

etc, where you put whatever name you choose for the key after "SAVE-".
E.g. "-ssl SAVE-fred".

Also, if you want to be prompted to possibly change the made up names,
etc. that x11vnc creates (e.g. "x11vnc-SELF-SIGNED-CERT-7762" for the
CommonName) for the certificates distinguished name (DN), then use
"x11vnc -ssl SAVE_PROMPT ...", "x11vnc -ssl SAVE_PROMPT-fred ..." etc.
when you create the key the first time.

Tip: when prompting, if you choose the CommonName entry to be the full
internet hostname of the machine the clients will be connecting to
then that will avoid an annoying dialog box in their Web browsers that
warn that the CommonName doesn't match the hostname.

---

## Passphrases for server keys

Well, since now with the "SAVE" keyword the certificate and key will
be longer lived, one can next worry about somebody stealing the
private key and pretending to be the x11vnc server! How to guard
against this?

The first is that the file is created with perms 600 (i.e. -rw-------)
to make it harder for an untrusted user to copy the file. A better way
is to also encrypt the private key with a passphrase. You are prompted
whether you want to do this or not when the key is first created under
"-ssl SAVE" mode ("Protect key with a passphrase? y/n"). It is
suggested that you use a passphrase. The inconvenience is every time
you run "x11vnc -ssl SAVE ..." you will need to supply the passphrase
to access the private key:

```
06/04/2006 11:39:11 using PEM /home/runge/.vnc/certs/server.pem  0.000s
```

A passphrase is needed to unlock an OpenSSL private key (PEM file).

```
Enter passphrase>
```

before x11vnc can continue.

---

## Being your own Certificate Authority

A very sophisticated way that scales well if the number of users is
large is to use a Certificate Authority (CA) whose public certificate
is available to all of the VNC clients and whose private key has been
used to digitally sign the x11vnc server certificate(s).

The idea is as follows:

* A special CA cert and key is generated.
* Its private key is always protected by a good passphrase since it
  is only used for signing.
* The CA cert is (safely) distributed to all machines where VNC
  clients will run.
* One or more x11vnc server certs and keys are generated.
* The x11vnc server cert is signed with the CA private key.
* x11vnc is run using the server key. (e.g. "-ssl SAVE")
* VNC clients (viewers) can now authenticate the x11vnc server
  because they have the CA certificate.

The advantage is the CA cert only needs to be distributed once to the
various machines, that can be done even before x11vnc server certs are
generated.

As above, it is important the CA private key and the x11vnc server key
are kept secret, otherwise someone could steal them and pretend to be
the CA or the x11vnc server if they copied the key. It is recommended
that the x11vnc server keys are also protected via a passphrase (see
the previous section).

Optionally, VNC viewer certs and keys could also be generated to
enable the x11vnc server to authenticate each client. This is not
normally done (usually a simple viewer password scheme is used), but
this can be useful in some situations. These optional steps go like
this:

* One or more VNC client certs and keys are generated.
* These VNC client certs are signed with the CA private key.
* The VNC client certs+keys are safely distributed to the
  corresponding client machines.
* x11vnc is told to verify clients by using the CA cert. (e.g.
  "-sslverify CA")
* When VNC clients (viewers) connect, they must authenticate
  themselves to x11vnc by using their client key.

Again, it is a good idea if the client private keys are protected with
a passphrase, otherwise if stolen they could be used to gain access to
the x11vnc server. Once distributed to the client machines, there is
no need to keep the client key on the CA machine that generated and
signed it. You can keep the client certs if you like because they are
public.

---

## How to do the above CA steps with x11vnc

Some utility commands are provided to ease the cert+key creation,
signing, and management: -sslGenCA, -sslGenCert, -sslDelCert,
-sslEncKey, -sslCertInfo. They basically run the openssl(1) command
for you to manage the certs/keys. It is required that openssl(1) is
installed on the machine and available in PATH. All commands can be
pointed to an alternate toplevel certificate directory via the -ssldir
option if you don't want to use the default ~/.vnc/certs.

1. To generate your Certificate Authority (CA) cert and key run this:

    ```
    x11vnc -sslGenCA
    ```

    Follow the prompts, you can modify any informational strings you care
    to. You will also be required to encrypt the CA private key with a
    passphrase. This generates these files:

    ```
    ~/.vnc/certs/CA/cacert.pem             (the CA public certificate)
    ~/.vnc/certs/CA/private/cakey.pem      (the encrypted CA private key)
    ```

    If you want to use a different directory use -ssldir It must supplied
    with all subsequent SSL utility options to point them to the correct
    directory.

2. To generate a signed x11vnc server cert and key run this:

    ```
    x11vnc -sslGenCert server
    ```

    As with the CA generation, follow the prompts and you can modify any
    informational strings that you care to. This will create the files:

    ```
    ~/.vnc/certs/server.crt             (the server public certificate)
    ~/.vnc/certs/server.pem             (the server private key + public cert)
    ```

    It is recommended to protect the server private key with a passphrase
    (you will be prompted whether you want to). You will need to provide
    it whenever you start x11vnc using this key.

3. Start up x11vnc using this server key:

    ```
    x11vnc -ssl SAVE -display :0 ...
    ```

    (SAVE corresponds to server.pem, see -sslGenCert server somename info
    on creating additional server keys, server-somename.crt ...)

4. Next, safely copy the CA certificate to the VNC viewer (client)
   machine(s). Perhaps:

    ```
    scp ~/.vnc/CA/cacert.pem clientmachine:.
    ```

5. Then the tricky part, make it so the SSL VNC Viewer uses this
   certificate! There are a number of ways this might be done, it depends
   on what your client and/or SSL tunnel is. Some examples:

    For the SSL Java VNC viewer supplied with x11vnc in
    classes/ssl/VncViewer.jar or classes/ssl/SignedVncViewer.jar:

    * Import the cacert.pem cert into your Web Browser (e.g. Edit ->
    Preferences -> Privacy & Security -> Manage Certificates ->
    WebSites -> Import)
    * Or Import the cacert.pem cert into your Java Plugin (e.g. run
    ControlPanel, then Security -> Certificates -> Secure Site ->
    Import)

    When importing, one would give the browser/java-plugin the path to the
    copied cacert.pem file in some dialog. Note that the Web browser or
    Java plugin is used for the server authentication. If the user gets a
    "Site not verified" message while connecting he should investigate
    further.

    For the use of stunnel (e.g. on Windows) one would add this to the
    stunnel.conf:

    ```
    # stunnel.conf:
    client = yes
    options = ALL
    CAfile = /path/to/cacert.pem          # or maybe C:\path\to\cacert.pem
    [myvncssl]
    accept = 5901
    connect = far-away.east:5900
    ```

    (then point the VNC viewer to localhost:1).

    Here is an example for the Unix stunnel wrapper script ss_vncviewer in
    our SSVNC package:

    ```
    ss_vncviewer -verify ./cacert.pem far-away.east:0
    ```

    Our SSVNC enhanced tightvnc viewer GUI can also use the certificate
    file for server authentication. You can load it via the SSVNC
    'Certs...' dialog and set 'ServerCert' to the cacert.pem file you
    safely copied there.

---

## Tricks for server keys

To create additional x11vnc server keys do something like this:

```
x11vnc -sslGenCert server myotherkey
```

and use it this way:

```
x11vnc -ssl SAVE-myotherkey ...
```

The files will be ~/.vnc/certs/server-myotherkey.{crt,pem}

You can also create a self-signed server key:

```
x11vnc -sslGenCert server self:third_key
```

and use it this way:

```
x11vnc -ssl SAVE-self:third_key ...
```

This key is not signed by your CA. This can be handy to have a key set
separate from your CA when you do not want to create a 2nd CA
cert+key.

---

## Using external CA's

You don't have to use your own CA cert+key, you can use a third
party's instead. Perhaps you have a company-wide CA or you can even
have your x11vnc certificate signed by a professional CA (e.g.
www.thawte.com or www.verisign.com or perhaps the free certificate
service www.startcom.org or www.cacert.org).

The advantage to doing this is that the VNC client machines will
already have the CA certificates installed and you don't have to
install it on each machine.

To generate an x11vnc server cert+key this way you should generate a
"request" for a certicate signing something like this (we use the name
"external" in this example, it could be anything you want):

```
x11vnc -sslGenCert server req:external
```

This will create the request file:

```
~/.vnc/certs/server-req:external.req
```

Which you should send to the external CA. When you get the signed
certificate back from them, save it in the file:

```
~/.vnc/certs/server-req:external.crt
```

and create the .pem this way:

```
mv  ~/.vnc/certs/server-req:external.key    ~/.vnc/certs/server-req:external.pem
chmod 600 ~/.vnc/certs/server-req:external.pem
cat ~/.vnc/certs/server-req:external.crt >> ~/.vnc/certs/server-req:external.pem
```

You also rename the two files (.crt and .pem) to have a shorter
basename if you like. E.g.:

```
mv  ~/.vnc/certs/server-req:external.pem  ~/.vnc/certs/server-ext.pem
mv  ~/.vnc/certs/server-req:external.crt  ~/.vnc/certs/server-ext.crt
```

and the use via "x11vnc -ssl SAVE-ext ...", etc.

On the viewer side make sure the external CA's certificate is
installed an available for the VNC viewer software you plan to use.

---

## Using Client Keys for Authentication

You can optionally create certs+keys for your VNC client machines as
well. After distributing them to the client machines you can have
x11vnc verify the clients using SSL. Here is how to do this:

```
x11vnc -sslGenCert client dilbert
x11vnc -sslGenCert client wally
x11vnc -sslGenCert client alice
...
```

As usual, follow the prompts if you want to change any of the info
field values. As always, it is a good idea (although inconvenient) to
protect the private keys with a passphrase. These files are created:

```
~/.vnc/certs/clients/dilbert.crt
~/.vnc/certs/clients/dilbert.pem
...
```

Note that these are kept in a clients subdirectory.

Next, safely copy the .pem files to each corresponding client machine
and incorporate them into the VNC viewer / SSL software (see the ideas
mentioned above for the CA and server keys). The only difference is
these certificates might be referred to as "My Certificates" or
"Client Certificates". They are used for client authentication (which
is relatively rare for SSL).

After copying them you can delete the clients/*.pem files for extra
safety because the private keys are not needed by the x11vnc server.
You don't really need the clients/*.crt files either (because they
have been signed by the CA). But they could come in handy for tracking
or troubleshooting, etc.

Now start up x11vnc and instruct it to verify connecting clients via
SSL and the CA cert:

```
x11vnc -ssl SAVE -sslverify CA
```

The "CA" special token instructs x11vnc to use its CA signed certs for
verification.

For arbitrary self-signed client certificates (no CA) it might be
something like this:

```
x11vnc -ssl SAVE -sslverify path/to/client.crt
x11vnc -ssl SAVE -sslverify path/to/client-hash-dir
x11vnc -ssl SAVE -sslverify path/to/certs.txt
```

Where client.crt would be an individual client certificate;
client-hash-dir a directory of file names based on md5 hashes of the
certs (see -sslverify); and certs.txt signifies a single file full of
client certificates.

Finally, connect with your VNC viewer using the key. Here is an
example for the Unix stunnel wrapper script ss_vncviewer: using client
authentication (and the standard server authentication with the CA
cert):

```
ss_vncviewer -mycert ./dilbert.pem -verify ./cacert.pem far-away.east:0
```

Our SSVNC enhanced tightvnc viewer can also use these openssl .pem
files (you can load them via Certs... -> MyCert dialog).

It is also possible to use -sslverify on a per-client key basis, and
also using self-signed client keys (x11vnc -sslGenCert client
self:dilbert)

Now a tricky part is to get Web browsers or Java Runtime to import and
use the openssl .pem cert+key files. See the next paragraph on how to
convert them to pkcs12 format. If you find a robust way to import them
and and get them to use the cert please let us know!

Here is how to convert our openssl crt/pem files to pkcs12 format
(contains both the client certificate and key) that can be read by Web
browsers and Java for use in client authentication:

```
openssl pkcs12 -export -in mycert.crt -inkey mycert.pem -out mycert.p12
```

it will ask for a passphrase to protect mycert.p12. Some software
(e.g. Java ControlPanel) may require a non-empty passphrase. Actually,
since our .pem contains both the certificate and private key, you
could just supply it for the -in and remove the -inkey option. It
appears that for certificates only importing, our .crt file is
sufficient and can be read by Mozilla/Firefox and Java...

If you have trouble getting your Java Runtime to import and use the
cert+key, there is a workaround for the SSL-enabled Java applet. On
the Web browser URL that retrieves the VNC applet, simply add a
"/?oneTimeKey=..." applet parameter (see ssl-portal for more details
on applet parameters; you don't need to do the full portal setup
though). The value of the oneTimeKey will be the very long string that
is output of the onetimekey program found in the classes/ssl x11vnc
directory. Or you can set oneTimeKey=PROMPT in which case the applet
will ask you to paste in the long string. These scheme is pretty ugly,
but it works. A nice application of it is to make one time keys for
users that have already logged into a secure HTTPS site via password.
A cgi program then makes a one time key for the logged in user to use:
it is passed back over HTTPS as the applet parameter in the URL and so
cannot be sniffed. x11vnc is run to use that key via -sslverify.

Update: as of Apr 2007 in the 0.9.1 x11vnc tarball there is a new
option setting "-users sslpeer=" that will do a switch user much like
-unixpw does, but this time using the emailAddress field of the
Certificate subject of the verified Client. This mode requires
-sslverify turned on to verify the clients via SSL. This mode can be
useful in situations using -create or -svc where a new X server needs
to be started up as the authenticated user (but unlike in -unixpw
mode, the unix username is not obviously known).

---

## Revoking Certificates

A large, scaled-up installation may benefit from being able to revoke
certificates (e.g. suppose a user's laptop with a vnc client or server
key is compromised.) You can use this option with x11vnc: -sslCRL. See
the info at that link for a guide on what openssl(1) commands you will
need to run to revoke a certificate.

---

## Additional utlities

You can get information about your keys via -sslCertInfo. These lists
all your keys:

```
x11vnc -sslCertInfo list
x11vnc -sslCertInfo ll
```

(the latter is long format).

These print long output, including the public certificate, for
individual keys:

```
x11vnc -sslCertInfo server
x11vnc -sslCertInfo dilbert
x11vnc -sslCertInfo all             (every key, very long)
```

If you want to add a protecting passphrase to a key originally created
without one:

```
x11vnc -sslEncKey SAVE
x11vnc -sslEncKey SAVE-fred
```

To delete a cert+key:

```
x11vnc -sslDelCert SAVE
x11vnc -sslDelCert SAVE-fred
x11vnc -sslDelCert wally
```

(but rm(1) will be just as effective).

---

## Chained Certificates

There is increasing interest in using chained CA's instead of a single
CA. The merits of using chained CA's are not described here besides to
say its use may make some things easier when a certificate needs to be
revoked.

x11vnc supports chained CA certificates. We describe a basic use case
here.

Background: Of course the most straight forward way to use SSL with
x11vnc is to use no CA at all (see above): a self-signed certificate
and key is used and its certificate needs to be safely copied to the
client side. This is basically the same as the SSH style of managing
keys. Next level up, one can use a single CA to sign server keys: then
only the CA's certificate needs to be safely copied to the client
side, this can happen even before any server certs are created (again,
see all of the discussion above.)

With a certificate chain there are two or more CA's involved. Perhaps
it looks like this:

```
root_CA ---> intermediate_CA ---> server_cert
```

Where the arrow basically means "signs".

In this usage mode the client (viewer-side) will have root_CA's
certificate available for verifying (and nothing else.) If the viewer
only received server_cert's certificate, it would not have enough info
to verify the server. The client needs to have intermediate_CA's cert
as well. The way to do this with x11vnc (i.e. an OpenSSL using app) is
to concatenate the server_cert's pem and the intermediate_CA's
certificate together.

For example, suppose the file intermediate_CA.crt had
intermediate_CA's certificate. And suppose the file server_cert.pem
had the server's certificate and private key pair as described above
on this page. We need to do this:

```
cat intermediate_CA.crt >> server_cert.pem
```

(Note: the order of the items inside the file matters; intermediate_CA
must be after the server key and cert) and then we run x11vnc like
this:

```
x11vnc -ssl ./server_cert.pem ...
```

Then, on the VNC viewer client side, the viewer authenticates the
x11vnc server by using root_CA's certificate. Suppose that is in a
file named root_CA.crt, then using the SSVNC wrapper script
ss_vncviewer (which is also included in the SSVNC package) as our
example, we have:

```
ss_vncviewer -verify ./root_CA.crt hostname:0
```

(where "hostname" is the machine where x11vnc is running.) One could
also use the SSVNC GUI setting Certs -> ServerCert to the root_CA.crt
file. Any other SSL enabled VNC viewer would use root_CA.crt in a
similar way.

---

### Creating Chained Certificates

Here is a fun example using VeriSign's "Trial Certificate" program.
Note that VeriSign has a Root CA and also an Intermediate CA and uses
the latter to sign customers certificates. So this provides an easy
way to test out the chained certificates mechanism with x11vnc.

First we created a test x11vnc server key:

```
openssl genrsa -out V1.key 1024
```

then we created a certificate signing request (CSR) for it:

```
openssl req -new -key V1.key -out V1.csr
```

(we followed the prompts and supplied information for the various
fields.)

Then we went to VeriSign's page http://www.verisign.com/ssl/index.html
and clicked on "FREE TRIAL" (the certificate is good for 14 days.) We
filled in the forms and got to the point where it asked for the CSR
and so we pasted in the contents of the above V1.csr file. Then, after
a few more steps, VeriSign signed and emailed us our certificate.

The VeriSign Trial certificates were found here:

```
http://www.verisign.com/support/verisign-intermediate-ca/Trial_Secure_Server_Root/index.html
http://www.verisign.com/support/verisign-intermediate-ca/trial-secure-server-intermediate/index.html
```

The former was pasted into a file V-Root.crt and the latter was pasted
into V-Intermediate.crt

We pasted our Trial certificate that VeriSign signed and emailed to us
into a file named V1.crt and then we typed:

```
cat V1.key V1.crt > V1.pem
cat V1.pem V-Intermediate.crt > V1-combined.pem
chmod 600 V1.pem V1-combined.pem
```

So now the file V1-combined.pem has our private key and (VeriSign
signed) certificate and VeriSign's Trial Intermediate certificate.

Next, we start x11vnc:

```
x11vnc -ssl ./V1-combined.pem ...
```

and finally, on the viewer side (SSVNC wrapper script example):

```
ss_vncviewer -verify ./V-Root.crt hostname:0
```

One will find that only that combination of certs and keys will work,
i.e. allow the SSL connection to be established. Every other
combination we tried failed (note that ss_vncviewer uses the external
stunnel command to handle the SSL so we are really testing stunnel's
SSL implementation on the viewer side); and so the system works as
expected.

---

### VNC Client Authentication using Certificate Chains

Now, going the other way around with the client authenticating himself
via this chain of SSL certificates, x11vnc is run this way:

```
x11vnc -ssl SAVE -sslverify ./V-Root.crt ...
```

   (note since the server must always supply a cert, we use its normal
   self-signed, etc., one via "-ssl SAVE" and use the VeriSign root cert
   for client authentication via -sslverify. The viewer must now supply
   the combined certificates, e.g.:

```
  ss_vncviewer -mycert ./V1-combined.pem hostname:0
```

---

### Using OpenSSL and x11vnc to create Certificate Chains

Although the x11vnc CA mechanism (-sslGenCA and -sslGenCert; see
above) was designed to only handle a single root CA (to sign server
and/or client certs) it can be coerced into creating a certificate
chain by way of an extra openssl(1) command.

We will first create two CA's via -sslGenCA; then use one of these CA
to sign the other; create a new (non-CA) server cert; and append the
intermediate CA's cert to the server cert to have everything needed in
the one file.

Here are the commands we ran to do what the previous paragraph
outlines.

First we create the two CA's, called CA_root and CA_Intermediate here,
in separate directories via x11vnc:

```
x11vnc -ssldir ~/CA_Root -sslGenCA
(follow the prompts, we included "CA_Root", e.g. Common Name, to aid identifying it)
```

```
x11vnc -ssldir ~/CA_Intermediate -sslGenCA
(follow the prompts, we included "CA_Intermediate", e.g. Common Name, to aid identifying it)
```

Next backup CA_Intermediate's cert and then sign it with CA_Root:

```
mv ~/CA_Intermediate/CA/cacert.pem ~/CA_Intermediate/CA/cacert.pem.ORIG
cd ~/CA_Root
openssl ca -config ./CA/ssl.cnf -policy policy_anything -extensions v3_ca -notext -ss_cert ~/CA_Intermediate/CA/cacert.pem.ORIG -out ~/CA_Intermediate/CA/cacert.pem
```

Note that it is required to cd to the ~/CA_Root directory and run the
openssl command from there.

You can print out info about the cert you just modified by:

```
openssl x509 -noout -text -in ~/CA_Intermediate/CA/cacert.pem
```

Now we create an x11vnc server cert named "test_chain" that is signed
by CA_Intermediate:

```
x11vnc -ssldir ~/CA_Intermediate -sslGenCert server test_chain
(follow the prompts)
```

You can print out information about this server cert just created via
this command:

```
x11vnc -ssldir ~/CA_Intermediate -sslCertInfo SAVE-test_chain
```

This will tell you the full path to the server certificate, which is
needed because we need to manually append the CA_Intermediate cert for
the chain to work:

```
cat ~/CA_Intermediate/CA/cacert.pem >> ~/CA_Intermediate/server-test_chain.pem
```

Now we are finally ready to use it. We can run x11vnc using this
server cert+key by either this command:

```
x11vnc -ssldir ~/CA_Intermediate -ssl SAVE-test_chain ...
```

or this command:

```
x11vnc -ssl ~/CA_Intermediate/server-test_chain.pem ...
```

since they are equivalent (both load the same pem file.)

Finally we connect via VNC viewer that uses CA_Root to verify the
server. As before we use ss_vncviewer:

```
ss_vncviewer -verify ~/CA_Root/CA/cacert.pem hostname:0
```

Client Certificates (see above) work in a similar manner.

So although it is a little awkward with the extra steps (e.g.
appending the CA_Intermediate cert) it is possible. If you want to do
this entirely with openssl(1) you will have to learn the openssl
commands corresponding to -genCA and -genCert. You may be able to find
guides on the Internet to do this. Starting with x11vnc 0.9.10, you
can have it print out the wrapper scripts it uses via: -sslScripts
(you will still need to fill in a few pieces of information; ask if it
is not clear from the source code.)

---

## More info

See also this article for some some general info and examples using
stunnel and openssl on Windows with VNC. Also
http://www.stunnel.org/faq/certs.html is a very good source of
information on SSL certificate creation and management.
