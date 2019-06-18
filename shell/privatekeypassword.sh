#! /bin/sh
# SPDX-License-Identifier: GPL-2.0-or-later
#######################################################################################################
#                                                                                                     #
# compute the password for the private key of a FRITZ!Box router                                      #
#                                                                                                     #
#######################################################################################################
#                                                                                                     #
# Copyright (C) 2014-2017 P.Hämmerlein (peterpawn@yourfritz.de)                                       #
#                                                                                                     #
# This program is free software; you can redistribute it and/or modify it under the terms of the GNU  #
# General Public License as published by the Free Software Foundation; either version 2 of the        #
# License, or (at your option) any later version.                                                     #
#                                                                                                     #
# This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without   #
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU      #
# General Public License under http://www.gnu.org/licenses/gpl-2.0.html for more details.             #
#                                                                                                     #
#######################################################################################################
#                                                                                                     #
# The script takes the 'maca' address of a FRITZ!Box device, computes the password used to encrypt    #
# the private key of a box from it and writes the result to STDOUT.                                   #
#                                                                                                     #
# This password is simply a MD5 hash over the 'maca' address (hexadecimal digits are always in        #
# uppercase) and the 6 lower bits of the first 8 byte of the hash value are translated to a string    #
# with a simple translation of "a-zA-Z0-9" for the 64 possible decimal values.                        #
#                                                                                                     #
# The result should be the same as it the C program beneath this script will show - but this script   #
# needs only a 'md5sum' command to compute it and it's not necessary to have a 'foreign binary' on    #
# your device, if you want to use this password for other purposes. All other needed commands (sed,   #
# printf and expr) should be available from each shell and you may use this script even outside an    #
# AVM device, if you want to prepare your own file for secure storage on the device (e.g. an own SSH  #
# host key for ED-25519 - that's my current use case).                                                #
#                                                                                                     #
# The 'maca' value has to be the first (and only) parameter (beside the -d option for debug output)   #
# and if it's missing, the script assumes it's running in a FRITZ!OS environment and tries to read    #
# the value from procfs.                                                                              #
#                                                                                                     #
#######################################################################################################
#                                                                                                     #
# constants                                                                                           #
#                                                                                                     #
#######################################################################################################
translation_table='abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!$'
procfs_path="/proc/sys/urlader/environment"
maca_name="maca"
#######################################################################################################
#                                                                                                     #
# subfunctions                                                                                        #
#                                                                                                     #
#######################################################################################################
#                                                                                                     #
# try to read the 'maca' value from environment                                                       #
#                                                                                                     #
#######################################################################################################
__get_maca_from_environment()
(
	[ -f "$procfs_path" ] || return 1
	sed -n -e "s|^${maca_name}[ \t]*\([0-9A-F:]*\)|\1|p" "$procfs_path" 
	return 0
)
#######################################################################################################
#                                                                                                     #
# check debug option                                                                                  #
#                                                                                                     #
#######################################################################################################
if [ "$1" = "-d" -o "$1" = "--debug" ]; then
	debug=1
	shift
else
	debug=0
fi
#######################################################################################################
#                                                                                                     #
# check parameters                                                                                    #
#                                                                                                     #
#######################################################################################################
if [ $# -gt 1 ]; then
	[ $debug -eq 1 ] && printf "Please specify only the 'maca' value as the 1st and only one parameter.\n" 1>&2
	exit 1
fi
maca="$1"
if [ ${#maca} -eq 0 ]; then
	maca="$(__get_maca_from_environment)"
	if [ ${#maca} -ne 17 ]; then
		[ $debug -eq 1 ] && printf "Unable to read 'maca' value from the system.\n" 1>&2
		exit 1
	fi
fi
if [ ${#maca} -ne 17 ]; then
	[ $debug -eq 1 ] && printf "The specified MAC address '%s' should contain exactly 17 characters.\n" "$maca" 1>&2
	exit 1
fi
if ! [ "$(expr "$maca" : "\([A-F0-9:]*\)")" = "$maca" ]; then
	[ $debug -eq 1 ] && printf "The specified MAC address '%s' contains unexpected characters.\n" "$maca" 1>&2
	exit 1
fi
#######################################################################################################
#                                                                                                     #
# compute the hash now                                                                                #
#                                                                                                     #
#######################################################################################################
for v in $(printf "%s" "$maca" | md5sum - 2>/dev/null | sed -n -e "s|^\([0-9a-fA-F]\{16\}\).*|\1|p" | sed -e "s|..|& |g"); do
	printf "%c" "$(expr "$translation_table" : ".\{$(( 0x$v % 64 ))\}\(.\).*")"
done
printf "\n"
exit 0
#######################################################################################################
#                                                                                                     #
# end of script                                                                                       #
#                                                                                                     #
#######################################################################################################
