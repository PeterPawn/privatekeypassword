# SPDX-License-Identifier: GPL-2.0-or-later
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
# This 'Makefile' is - together with 'privatekeypassword.h' - intended as a 'mini documentation' for  #
# the whole project from https://github.com/PeterPawn/privatekeypassword ... comments here try to     #
# give some hints, what's to consider while integrating it into any toolchain.                        #
#                                                                                                     #
#######################################################################################################
#                                                                                                     #
# project name                                                                                        #
#                                                                                                     #
#######################################################################################################
PROJECT:=privatekeypassword
BASENAME:=$(PROJECT)
#######################################################################################################
#                                                                                                     #
# Target binary, it only writes the password to STDOUT and it may be used as a simple test for the    #
# library, if you compare its outcome and the output of the shell script 'privatekeypassword.sh'.     #
#                                                                                                     #
#######################################################################################################
BINARY:=$(BASENAME)
#######################################################################################################
#                                                                                                     #
# Target library settings, the main outcome of this project is a static or dynamic library, which     #
# provides simple functions to get the password as a C formatted string.                              #
#                                                                                                     #
# See 'privatekeypassword.h' for interface definitions, some older and unsed functions have been      #
# removed now, because they're not necessary any longer.                                              #
#                                                                                                     #
#######################################################################################################
LIBNAME:=lib$(BASENAME)
LIB_SO:=$(LIBNAME).so
LIB_A:=$(LIBNAME).a
LIBHDR:=$(BASENAME).h
#######################################################################################################
#                                                                                                     #
# source files used                                                                                   #
#                                                                                                     #
#######################################################################################################
BIN_SRCS = privatekeypassword.c
BIN_OBJS = $(BIN_SRCS:%.c=%.o)
LIB_SRCS = pkpwd.c md5.c
LIB_OBJS = $(LIB_SRCS:%.c=%.o)
#######################################################################################################
#                                                                                                     #
# Uncomment the following lines, if you have to rely on local redefinitions for the tools to be used  #
# during a cross-build.                                                                               #
#                                                                                                     #
# Some users want to pass them from outside, if there is a complete toolchain around it and some want #
# to set them explicitely here ... even 'gcc' is only one possible assumption (and mostly the same    #
# version as 'cc').                                                                                   #
#                                                                                                     #
# CC is usually a predefined make variable (see 'make -p' on your build host) for the C compiler to   #
# use and AR is the name of the static library (which is an archive of multiple object files) build   #
# tool. RANLIB isn't necessary while using the GNU versions of 'binutils', it's the same as an 'ar s' #
# call for the archive.                                                                               #
#                                                                                                     #
#######################################################################################################
#CC = $(CROSS_COMPILE)gcc
#AR = $(CROSS_COMPILE)ar
#######################################################################################################
#                                                                                                     #
# We want to create a 'clean' object file without any unsafe or unclear constructs and so we enable   #
# all warnings (and some extras). The source files use some constructs from C99 specifications (e.g.  #
# local variables with block scope and initialization together with declaration), so we specify the   #
# used C standard version explicitely. Medium code optimizations are also possible (-o2).             #
# To limit the visible functions from the shared library to the intended interface, the default visi- #
# bility of symbols is set to 'hidden', the code declares each really exported function with appro-   #
# priate attributes itself.                                                                           #
#                                                                                                     #
#######################################################################################################
ifdef DEBUG
# no optimizations, with GDB compatible debug symbols - if we want to debug (step into) the library
CC_OPT = -O0 -ggdb
else
CC_OPT = -O2
endif
override CFLAGS += -W -Wall -std=c99 -fvisibility=hidden $(CC_OPT)
#######################################################################################################
#                                                                                                     #
# Uncomment this rule to prevent use of CPPFLAGS settings from some unknown source - or even a        #
# complete different rule in an unknown (or not thoroughly checked) build environment. It may be      #
# necessary to avoid CPPFLAGS, if it contains any "visibility" related settings and will be specified #
# AFTER our own CFLAGS value while calling the compiler.                                              #
#                                                                                                     #
#######################################################################################################
#%.o: %.c
#	$(CC) $(CFLAGS) -c $< -o $@
#######################################################################################################
#                                                                                                     #
# Generate position independent code for the library.                                                 #
#                                                                                                     #
#######################################################################################################
$(LIB_OBJS): CFLAGS += -fPIC
#######################################################################################################
#                                                                                                     #
# Link our binaries with these libraries, too.                                                        #
#                                                                                                     #
# Be carefully, what the current content of LDLIBS is while running a cross-build, but it's naturally #
# possible to change (predefined) LDLIBS from the implicit rules to include our own additional        #
# libraries, as long as no sub-make will be called later, which inherits unwanted additions from      #
# here.                                                                                               #
#                                                                                                     #
#######################################################################################################
#LDLIBS = <no additional libraries are needed anymore>
$(BINARY): LDFLAGS += -L.
$(BINARY): LDLIBS += -l$(BASENAME)
#######################################################################################################
#                                                                                                     #
# various targets to make                                                                             #
#                                                                                                     #
#######################################################################################################
.PHONY: all clean
#######################################################################################################
#                                                                                                     #
# make all files (static and shared library, command line utility)                                    #
#                                                                                                     #
#######################################################################################################
all: $(LIB_SO) $(LIB_A) $(BINARY)
#######################################################################################################
#                                                                                                     #
# install library related files from Freetz or any other toolchain, which wants to see any 'install'  #
# target instead of copying files (DSO, static library and header file) itself; the CLI binary is not #
# installed here                                                                                      #
#                                                                                                     #
# DESTDIR will be set (by the caller) to the target (staging) directory.                              #
#                                                                                                     #
# There's (intentionally) no other 'install' target for 'make' ... copy the files manually to the     #
# right location(s) or specify your build directory as additional source for include files and link   #
# libraries while building applications with this project.                                            #
#                                                                                                     #
#######################################################################################################
install-lib: $(LIB_SO) $(LIB_A) $(LIBHDR)
	mkdir -p $(DESTDIR)/usr/include/$(BASENAME) $(DESTDIR)/usr/lib
	cp -a $(LIBHDR) $(DESTDIR)/usr/include/$(BASENAME)
	cp -a $(LIB_SO) $(LIB_A) $(DESTDIR)/usr/lib/
#######################################################################################################
#                                                                                                     #
# shared library                                                                                      #
#                                                                                                     #
#######################################################################################################
$(LIB_SO): $(LIB_OBJS)
	$(CC) -shared -o $@ $^
#######################################################################################################
#                                                                                                     #
# static library                                                                                      #
#                                                                                                     #
#######################################################################################################
$(LIB_A): $(LIB_OBJS)
	-$(RM) $@ 2>/dev/null
	$(AR) rcu $@ $^
	$(AR) s $@
#######################################################################################################
#                                                                                                     #
# the CLI binary                                                                                      #
#                                                                                                     #
#######################################################################################################
$(BINARY): $(BIN_OBJS) | $(LIB_SO)
#######################################################################################################
#                                                                                                     #
# everything to make, if a header file changes                                                        #
#                                                                                                     #
#######################################################################################################
$(LIB_OBJS) $(BIN_OBJS): $(LIBHDR)
#######################################################################################################
#                                                                                                     #
# cleanup                                                                                             #
#                                                                                                     #
#######################################################################################################
clean:
	-$(RM) *.o $(LIB_SO) $(LIB_A) $(BINARY) 2>/dev/null
#######################################################################################################
#                                                                                                     #
# end of file                                                                                         #
#                                                                                                     #
#######################################################################################################
