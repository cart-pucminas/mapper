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
BINDIR = bin
INCDIR = include
SRCDIR = src
DOCDIR = doc

# Toolchain.
CC = gcc

# Toolchain configuration.
CFLAGS += -ansi -pedantic
CFLAGS += -Wall -Wextra
CFLAGS += -O3
CFLAGS += -I $(INCDIR) 

# Builds the release and debug versions.
all: release debug documentation

# Builds the release version.
release: $(SRC)
	$(CC) $(CFLAGS) -D NDEBUG $(SRC) -o $(BINDIR)/$(EXEC_RELEASE) -lm

# Builds the debug version.
debug:
	$(CC) $(CFLAGS) -g $(SRC) -o $(BINDIR)/$(EXEC_DEBUG) -lm

# Builds the documentation:
documentation: $(DOCDIR)/mapper.1
	man -t $< | ps2pdf - > $(DOCDIR)/mapper.pdf

# Clean.
clean:
	rm -f $(BINDIR)/$(EXEC_DEBUG)
	rm -f $(BINDIR)/$(EXEC_RELEASE)
	rm -f $(DOCDIR)/*.pdf
	
