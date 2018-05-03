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

// interface library for private key password access - does not contain
// older and now unused/unnecessary functions anymore

#ifndef PRIVATEKEYPASSWORD_H

#define PRIVATEKEYPASSWORD_H

// error codes to identify the cause of unexpected results

typedef enum {
	// no error
	PRIVATEKEYPASSWORD_ERROR_NOERROR = 0,
	// error allocating a dynamic memory area
	PRIVATEKEYPASSWORD_ERROR_NOMEMORY = 1,
	// error reading 'maca' value from device
	PRIVATEKEYPASSWORD_ERROR_ACCESS = 2,
}	privateKeyPassword_error_t;

// the external interface functions

// const char * getPrivateKeyPassword(void)
//
// - returns the pointer to a buffer containing the password string (incl. 
//   trailing '\0' character) 
// - the returned value is NULL in case of any error
// - call getPrivateKeyPassword_Error() to get an error code explaining the
//   reason for the latest error
const char *	getPrivateKeyPassword(void);

// privateKeyPassword_error_t getPrivateKeyPassword_Error(void)
// 
// - return the latest error code for any call of this library
// - the returned value is cleared after this call
privateKeyPassword_error_t	getPrivateKeyPassword_Error(void);

// int getPrivateKeyPassword_OpenSSL_Callback()
//
// - an additional function with the needed interface for OpenSSL password
//   callbacks
int		getPrivateKeyPassword_OpenSSL_Callback(char * buf, int size, int rwflag, void * userdata);

#endif
