# project
#
BASENAME:=privatekeypassword
#
# library settings
#
LIBNAME:=lib$(BASENAME)
LIBRARY:=$(LIBNAME).so
LIB:=$(LIBNAME).a
LIBHDR:=$(BASENAME).h
#
# source files
#
LIB_SRCS = $(BASENAME).c md5.c
LIB_OBJS = $(LIB_SRCS:%.c=%.o)
#
# tools
#
CC = gcc
RM = rm
AR = ar
RANLIB = ranlib
#
# flags for calling the tools
#
#override CFLAGS += -W -Wall -std=c99 -O2 -fvisibility=hidden 
override CFLAGS += -W -Wall -std=c99 -O0 -ggdb -fvisibility=hidden
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
LIBS = -lcrypt
#
# targets to make
#
.PHONY: all clean
#
all: $(LIBRARY) $(LIB)
#
# install library files into the Freetz build system
# DESTDIR will be set to the target directory while calling this target
#
install-lib: $(LIBRARY) $(LIB) $(LIBHDR)
	mkdir -p $(DESTDIR)/usr/include/$(BASENAME) $(DESTDIR)/usr/lib
	cp -a $(LIBHDR) $(DESTDIR)/usr/include/$(BASENAME)
	cp -a $(LIBRARY) $(LIB) $(DESTDIR)/usr/lib/
#
# shared library
#
$(LIBRARY): $(LIB_OBJS) 
	$(CC) -shared -o $@ $<
#
# static library
#
$(LIB): $(LIB_OBJS) $(LIBHDR)
	-$(RM) $@ 2>/dev/null
	$(AR) rcu $@ $<
	$(RANLIB) $@
#
# everything to make, if header file changes
#
$(LIB_OBJS): $(LIBHDR)
#
# cleanup 	
#
clean:
	-$(RM) *.o $(LIB) $(LIBRARY) 2>/dev/null
