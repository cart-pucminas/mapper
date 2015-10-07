#
# Copyright(C) 2013 Pedro H. Penna <pedrohenriquepenna@gmail.com>
#

# Source files.
SRC = $(wildcard src/*.c)        \
	  $(wildcard src/kmeans/*.c) \
	  $(wildcard src/list/*.c)   \
	  $(wildcard src/matrix/*.c) \
	  $(wildcard src/vector/*.c) \

# Executable files.
EXEC_RELEASE = mapper
EXEC_DEBUG = mapper_debug

# MyLib
export MYLIB = mylib-0.5

# Directories.
export PREFIX      = $(CURDIR)
export BINDIR      = $(CURDIR)/bin
export CONTRIBDIR = $(CURDIR)/contrib
export INCDIR     = $(CURDIR)/include
export LIBDIR     = $(CURDIR)/lib
export SRCDIR     = $(CURDIR)/src
export DOCDIR     = $(CURDIR)/doc

# Toolchain.
export CC = gcc

# Libraries.
export LIBS = $(LIBDIR)/libmy.a -fopenmp -lm

# Toolchain configuration.
export CFLAGS += -std=c99
# export CFLAGS += -Wall -Wextra
export CFLAGS += -O3
export CFLAGS += -I $(INCDIR) 

# Phony list.
.PHONY: tools

# Builds the release and debug versions.
all: tools release debug documentation

# Builds the release version.
release: lib $(SRC)
	$(CC) $(CFLAGS) -D NDEBUG $(SRC) -o $(BINDIR)/$(EXEC_RELEASE) -lm

# Builds the debug version.
debug: lib
	$(CC) $(CFLAGS) -g $(SRC) -o $(BINDIR)/$(EXEC_DEBUG) -lm

# Builds library.
lib:
	cd $(CONTRIBDIR) && \
	mkdir -p $(MYLIB) && \
	tar -xjvf $(MYLIB).tar.bz2 --directory $(MYLIB) && \
	cd $(MYLIB) &&\
	$(MAKE) install PREFIX=$(PREFIX)
	rm -rf $(CONTRIBDIR)/$(MYLIB)

# Builds the documentation:
documentation: $(DOCDIR)/mapper.1
	man -t $< | ps2pdf - > $(DOCDIR)/mapper.pdf

# Builds all toosl.
tools: lib
	cd tools/ && $(MAKE) all

# Clean.
clean:
	cd tools && $(MAKE) clean
	rm -rf $(INCDIR)/mylib
	rm -rf $(LIBDIR)
	rm -f $(BINDIR)/$(EXEC_DEBUG)
	rm -f $(BINDIR)/$(EXEC_RELEASE)
	rm -f $(BINDIR)/$(EXEC_RELEASE)
	rm -f $(DOCDIR)/*.pdf
	
