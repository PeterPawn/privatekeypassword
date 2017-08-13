# project
#
BASENAME:=privatekeypassword
#
# target binary, used as proxy too
# 
BINARY:=$(BASENAME)
#
# library settings
#
LIBNAME:=lib$(BASENAME)
LIB_SO:=$(LIBNAME).so
LIB_A:=$(LIBNAME).a
LIBHDR:=$(BASENAME).h
#
# source files
#
BIN_SRCS = privatekeypassword.c
BIN_OBJS = $(BIN_SRCS:%.c=%.o)
LIB_SRCS = pkpwd.c md5.c
LIB_OBJS = $(LIB_SRCS:%.c=%.o)
#
# tools
#
CC = $(CROSS_COMPILE)gcc
RM = rm
AR = $(CROSS_COMPILE)ar
RANLIB = $(CROSS_COMPILE)ranlib
#
# flags for calling the tools
#
override CFLAGS += -W -Wall -std=c99 -O2 -fvisibility=hidden 
#override CFLAGS += -W -Wall -std=c99 -O0 -ggdb -fvisibility=hidden
#
# how to build objects from sources
#
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
#
# generate position independent code for the library
#
$(LIB_OBJS): CFLAGS += -fPIC
#
# link binaries with this libraries too
#
LIBS = 
#
# targets to make
#
.PHONY: all clean
#
all: $(LIB_SO) $(LIB_A) $(BINARY)
#
# install library files into the Freetz build system
# DESTDIR will be set to the target directory while calling this target
#
install-lib: $(LIB_SO) $(LIB_A) $(LIBHDR)
	mkdir -p $(DESTDIR)/usr/include/$(BASENAME) $(DESTDIR)/usr/lib
	cp -a $(LIBHDR) $(DESTDIR)/usr/include/$(BASENAME)
	cp -a $(LIB_SO) $(LIB_A) $(DESTDIR)/usr/lib/
#
# shared library
#
$(LIB_SO): $(LIB_OBJS) $(LIBHDR) libcrypt.h
	$(CC) -shared -o $@ $<
#
# static library
#
$(LIB_A): $(LIB_OBJS) $(LIBHDR) libcrypt.h
	-$(RM) $@ 2>/dev/null
	$(AR) rcu $@ $<
	$(RANLIB) $@
#
# the CLI binary
#
$(BINARY): $(BIN_OBJS) $(LIB_SO)
	$(CC) $(LDFLAGS) $(filter %.o,$<) -L. -l$(BASENAME) -o $@ $(LIBS)
#
# everything to make, if header file changes
#
$(LIB_OBJS) $(BIN_OBJS): $(LIBHDR) libcrypt.h
#
# cleanup 	
#
clean:
	-$(RM) *.o $(LIB_SO) $(LIB_A) $(BINARY) 2>/dev/null
