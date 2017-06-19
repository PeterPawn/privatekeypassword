/*
 * vim: set tabstop=4 syntax=c :
 *
 * Copyright (C) 2014-2017, Peter Haemmerlein (peterpawn@yourfritz.de)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program, please look for the file LICENSE.
 */

#include <features.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic error "-Wfatal-errors"

#if !defined(__UCLIBC__)
#error This library is designed to work with the uClibc library on a FRITZ!OS device.
#pragma GCC error "Compilation aborted."
#endif

#pragma GCC diagnostic pop

#include <stddef.h>
#include <ctype.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "privatekeypassword.h"

#define	pkpwd_setError(err)				__privateKeyPassword_error = PRIVATEKEYPASSWORD_ERROR_##err

#define	pkpwd_returnError(err,value)	{ pkpwd_setError(err); return (value); }

#define	MAX_CACHED_PASSWORD_SIZE		31

#define	URLADER_ENV						"/proc/sys/urlader/environment"

#define	MACA_NAME						"maca\t"

#define	MD5_SIZE						16

#define	MACA_SIZE						18

#define	PASSWORD_SIZE					8

#define	TRANSLATION_TABLE				"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789$!"

#define	EXPORTED						__attribute__((__visibility__("default"), used, externally_visible))

#define	UNUSED							__attribute__((unused))

// the static variable for our error code, it will be cleared after reading

static	privateKeyPassword_error_t	__privateKeyPassword_error = PRIVATEKEYPASSWORD_ERROR_NOERROR;

// the static variable for our error code, it will be cleared after reading

UNUSED	static	privateKeyPassword_method_t	__privateKeyPassword_method = PRIVATEKEYPASSWORD_METHOD_DL;

// the password is cached in a static variable once read, 'cause it's never 
// changed anymore

static char	__privateKeyPassword_cache[MAX_CACHED_PASSWORD_SIZE + 1] = { '\0' };

// MD5 functions from md5.c

struct MD5Context {
    uint32_t			state[4];
    uint32_t			count[2];
    unsigned char		buffer[64];
};

UNUSED	extern	void	__md5_Init(struct MD5Context *);
UNUSED	extern	void	__md5_Update(struct MD5Context *, const unsigned char *, unsigned int);
UNUSED	extern	void	__md5_Final(unsigned char [16], struct MD5Context *);

// internal helper functions

bool	__privateKeyPassword_GetMAC(unsigned char * maca)
{
	*maca = '\0';

	char *				buffer = malloc(1024);

	if (!buffer)
		return false;

	FILE *				env = fopen(URLADER_ENV, "r");

	if (!env)
		return false;

	while (fgets(buffer, 1024, env))
	{
		if (!strncmp(buffer, MACA_NAME, strlen(MACA_NAME)))
		{
			strncpy((char *) maca, buffer + strlen(MACA_NAME), 18);
			*(maca + 17) = '\0';
			break;
		}
	}

	fclose(env);
	free(buffer);

	return true;
}

void	__privateKeyPassword_Compute(void)
{
	struct MD5Context	ctx;
	unsigned char		hash[MD5_SIZE];
	unsigned char		maca[MACA_SIZE];
	char *				table = TRANSLATION_TABLE;

	__privateKeyPassword_cache[0] = '\0';
	
	if (!__privateKeyPassword_GetMAC(maca))
		return;

	__md5_Init(&ctx);
	__md5_Update(&ctx, maca, strlen((char *) maca));
	__md5_Final(hash, &ctx);
	
	for (size_t i = 0; i < PASSWORD_SIZE; i++)
	{
		char			c = hash[i];

		c = (c & 63);
		__privateKeyPassword_cache[i] = *(table + c);
	}
	__privateKeyPassword_cache[PASSWORD_SIZE] = '\0';
}

char *	__privateKeyPassword_malloc(const char * source)
{
	size_t				len = strlen(source);

	if (!len)
		pkpwd_returnError(NOPASSWORD, NULL);

	char *				password = malloc(len + 1);

	if (!password)
		pkpwd_returnError(NOMEMORY, NULL);

	strcpy(password, source);

	pkpwd_returnError(NOERROR, password);
}

char *	__privateKeyPassword_fromCache(void)
{
	int 				len = strlen((char *) __privateKeyPassword_cache);

	if (len) 
		pkpwd_returnError(NOERROR, __privateKeyPassword_cache);

	__privateKeyPassword_Compute();

	return __privateKeyPassword_cache;
}

int __privateKeyPassword_dynamic(char * buffer)
{
	char *				password = __privateKeyPassword_fromCache();
	
	if (password && strlen(password))
	{
		strcpy(buffer, password);
	}
	
	pkpwd_returnError(NOERROR, strlen(buffer));
}

int	__privateKeyPassword_proxy(char * buffer)
{
	return __privateKeyPassword_dynamic(buffer);	
}

// privateKeyPassword_error_t getPrivateKeyPassword_Error(void)
//
// - get the latest error code from library
// - the error code is reset after reading it once
EXPORTED	privateKeyPassword_error_t	getPrivateKeyPassword_Error(void)
{
	privateKeyPassword_error_t 	error = __privateKeyPassword_error;

	__privateKeyPassword_error = PRIVATEKEYPASSWORD_ERROR_NOERROR;

	return error;
}

// static char *getStaticPrivateKeyPassword(void)
//
// - returns the pointer to a static string containing the password
// - the returned string is empty in case of any error
// - call getPrivateKeyPassword_Error() to get an error code explaining the
//   reason for the latest error
EXPORTED	char *	getStaticPrivateKeyPassword(void)
{
	return __privateKeyPassword_fromCache();
}

// char *getPrivateKeyPassword(void)
//
// - returns the pointer to a dynamically allocated buffer containing
//   the password string (incl. trailing '\0' character)
// - the returned value is NULL in case of any error
// - the caller is responsible to free the buffer, if the pointer isn't NULL
// - call getPrivateKeyPassword_Error() to get an error code explaining the
//   reason for the latest error
EXPORTED	char *	getPrivateKeyPassword(void)
{
	return __privateKeyPassword_malloc(__privateKeyPassword_fromCache());
}

// void getPrivateKeyPassword_setMethod(privateKeyPassword_method_t method)
//
// - set the method used to get the password
// - calling vendor's function directly isn't supported from statically linked
//   binaries and will result in a SIGSEGV while doing dlopen() calls
// - the alternative way (calling a proxy binary) has other disadvantages like
//   an additional dependency and a higher 'costs' starting another process
EXPORTED	void	getPrivateKeyPassword_setMethod(UNUSED privateKeyPassword_method_t method)
{
	return;
}

// int getPrivateKeyPassword_OpenSSL_Callback()
//
// - an additional function with the needed interface for OpenSSL password 
//   callbacks
EXPORTED	int	getPrivateKeyPassword_OpenSSL_Callback(char * buf, int size, int rwflag, void * userdata)
{
	size_t				len = 0;

	if (rwflag)
		return len;		// no password for encoding

	// if userdata pointer is supplied, use the string it's pointing to
	// instead of any other possible passwords, so we can use this pointer
	// to override any automatic password extraction

	char *				source = userdata;

	if (!source)
		source = __privateKeyPassword_fromCache();

	if ((len = strlen(source)))
	{
		if ((size_t) size <= len)
			len = size;

		strncpy(buf, source, len);
	}

	return len;
}

// int getPrivateKeyPassword_WithBuffer(char *buf, size_t size)
//
// - an additional function with the needed interface to be compatible with
//   the library from er13
EXPORTED	int	getPrivateKeyPassword_WithBuffer(char * buf, size_t size)
{
	int 				len = getPrivateKeyPassword_OpenSSL_Callback(buf, size - 1, 0, NULL);

	return (len == 0 ? -1 : len);
}
