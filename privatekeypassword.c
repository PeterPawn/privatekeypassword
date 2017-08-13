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

/* 'privatekeypassword' command line interface tool */

// simply call the library function to display the password (and a trailing 
// newline after it)

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include "privatekeypassword.h"

int main(__attribute__((unused)) int argc, __attribute__((unused)) char** argv)
{
	char *password = getStaticPrivateKeyPassword();
	int len = strlen(password);

	if (len) printf("%s\n", password);
	else fprintf(stderr, "Error %u reading private key password.\n", getPrivateKeyPassword_Error());

	exit(len ? EXIT_SUCCESS : EXIT_FAILURE);
}
