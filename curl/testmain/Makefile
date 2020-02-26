#####################################################################
# Copyright : Chinese
# Name      : Makefile
# Date      : 2020/2/26
# Author    : OpenSource
######################################################################
INCLUDE_DIR = include
INCLUDE_INSTALL_DIR = $(COMMONLIB_INCLUDE_DIR)
CC = gcc
CPLUSPLUS = g++
CFLAGS = -O2 -std=c99 -L./lib -lcurl
CFLAGS += -I$(INCLUDE_DIR)

RELTARGET = libcurlInterface.a

C_SOURCES   = $(wildcard *.c)
CPP_SOURCES = $(wildcard *.cpp)

HEADERS = $(wildcard $(INCLUDE_DIR)/*.h)

C_RELOBJFILES = $(C_SOURCES:%.c=%.o)
CPP_RELOBJFILES = $(CPP_SOURCES:%.cpp=%.o)

.PHONY: clean install

all: install

install: $(RELTARGET)
	install -d $(INCLUDE_INSTALL_DIR)
	install $(HEADERS) $(INCLUDE_INSTALL_DIR)

$(RELTARGET): $(C_RELOBJFILES) $(CPP_RELOBJFILES)
	$(AR) rcs $@ $^ $(LDFLAGS)
	
.c.o:
	$(CC) -c $(CFLAGS) $< -o $@
	
.cpp.o:
	$(CPLUSPLUS) -c $(CFLAGS) $< -o $@
	
clean:
	rm -f $(C_RELOBJFILES) $(CPP_RELOBJFILES) $(RELTARGET)


