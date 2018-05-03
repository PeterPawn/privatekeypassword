# Purpose

- provide the password needed to open the encrypted private key file (`/var/flash/websrv_ssl_key.pem`) from FRITZ!OS versions above 06.20
- new version computes the password itself from ```maca``` value
- no more hassle with static vs. dynamic linking and/or using the proxy binary
- the project contains a library (shared and static), some older and (as far as I know) unused functions have been removed from the interface
- the computation of MD5 hashes (in `md5.c`) was copied (with some minor changes) from the `uClibc` sources - the (default) `libuclibc/libcrypt` does not export the needed functions, so they were duplicated here
- the provided binary ```privatekeypassword``` may be used from other programs (or shell scripts) to get the clear-text of the password for the private key file

## How to build it?

- if you want to use **this version** of the library in a ```Freetz``` build, create a sub-module for it:

```shell
git rm -r make/privatekeypassword                                                             # <== remove the existing sub-directory for this project from the clone
git submodule add https://github.com/PeterPawn/privatekeypassword.git make/privatekeypassword # <== re-create the sub-directory as a submodule
```

- if you want to build it outside of ```Freetz``` (but it's designed to work in a uClibc-based environment only), use the supplied `Makefile` in the `src` sub-folder

```shell
cd src
make all
make install-lib DESTDIR=<target_staging_directory>
```

- the binary `privatekeypassword` will not be installed automatically - you may add another install target to the `Makefile` yourself
