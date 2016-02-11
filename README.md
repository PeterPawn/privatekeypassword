Purpose:

provide the password needed to open the encrypted private key file
(/var/flash/websrv_ssl_key.pem) from FRITZ!OS versions above 06.20

That's finally the "vendor's way" to get the "secret" password for the
private key file.

The vendor owns the needed library interface file(s) to insert dynamic
library calls at compile/link time, we should do this better with
dynamic library support functions to be independent from changes here.

So we'll try to locate the `securestore_get` function from `libboxlib.so`
and call it with an appropriate parameter list to obtain the password
string - if we do not provide a value for the 'mask' parameter (which
leads to XOR with all zeros and therefore does not change the password
string), we do not need to *deobfuscate* the resulting string.

To provide more flexibility, a method to use a proxy process is added
and based on the assumption, that most callers of this library are
using the OpenSSL libraries, there is an additional function to use
this library immediately as callback routine set up with a call to
`SSL_CTX_set_default_passwd_cb`.

To honor the aspect of weakening the security of private key files
with this library, I want to express it cleary: storing the private key
on the flash of a FRITZ!Box is *necessary* and you can't work around
this security threat at all ... so you better do not use the same private
key anywhere else and keep in mind, that the FRITZ!Box key and certificate
(finally the identity of the device) are suspicious anytime.

Nevertheless using a secured connection and a consistent identity of the
FRITZ!Box router is better than using an open connection and many
different identities for various services, because there's a higher
probability that the user gets confused while using different keys.

Having a solution to use the same private key for different services does
not mean, you're obliged to use the same identity, but you get the
*chance* to do so.

REMARKS:

Either the `libboxlib.so` implementation is faulty or under uClibc something
else wents wrong (perhaps with `pthread_atfork()` handlers) => but calling
`fork()` after `dlclose()`-ing the vendor's library leads to an invalid call
to an address, where the library was prior loaded, therefore we load the
library only once (and check this with RTLD_NOLOAD first) and calling
`dlclose()` is avoided.

The `dlopen()` call fails with a SEGV exception, if the calling binary is
built with static linking ... we have to use another implementation for
such binaries to work around this problem => use a prior call to
`getPrivateKeyPassword_setMethod` in this case to force proxy usage.
