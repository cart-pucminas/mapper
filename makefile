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

# Directories.
export BINDIR = $(CURDIR)/bin
export INCDIR = $(CURDIR)/include
export SRCDIR = $(CURDIR)/src
export DOCDIR = $(CURDIR)/doc

# Toolchain.
export CC = gcc

# Toolchain configuration.
# export CFLAGS += -ansi -pedantic
# export CFLAGS += -Wall -Wextra
export CFLAGS += -O3
export CFLAGS += -I $(INCDIR) 

# Phony list.
.PHONY: tools

# Builds the release and debug versions.
all: tools release debug documentation

# Builds the release version.
release: $(SRC)
	$(CC) $(CFLAGS) -D NDEBUG $(SRC) -o $(BINDIR)/$(EXEC_RELEASE) -lm

# Builds the debug version.
debug:
	$(CC) $(CFLAGS) -g $(SRC) -o $(BINDIR)/$(EXEC_DEBUG) -lm

# Builds the documentation:
documentation: $(DOCDIR)/mapper.1
	man -t $< | ps2pdf - > $(DOCDIR)/mapper.pdf

# Builds all toosl.
tools:
	cd tools/ && $(MAKE) all

# Clean.
clean:
	rm -f $(BINDIR)/$(EXEC_DEBUG)
	rm -f $(BINDIR)/$(EXEC_RELEASE)
	rm -f $(DOCDIR)/*.pdf
	cd tools && $(MAKE) clean
	
