### Purpose

- provide the password needed to open the encrypted private key file (/var/flash/websrv_ssl_key.pem) from FRITZ!OS versions above 06.20
- new version computes the password itself from ```maca``` value
- no more hassle with static vs. dynamic linking and/or using the proxy binary
- the project contains a library (shared and static), some older and (as far as I know) unused functions have been removed from the interface
- the computation of MD5 hashes (in md5.c) was copied (with some minor changes) from the uClibc sources - the (default) libuclibc/libcrypt does not export the needed functions, so they were duplicated here
- the provided binary ```privatekeypassword``` may be used from other programs (or shell scripts) to get the clear-text of the password for the private key file
