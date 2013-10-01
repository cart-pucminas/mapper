#
# Copyright(C) 2013 Pedro H. Penna <pedrohenriquepenna@gmail.com>
#

# Source files.
SRC = $(wildcard src/*.c)        \
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

# Toolchain.
CC = gcc

# Toolchain configuration.
CFLAGS += -ansi -pedantic
CFLAGS += -Wall -Wextra
CFLAGS += -O3
CFLAGS += -I $(INCDIR) 

# Builds release and debug versions.
all: release debug

# Builds release version.
release: $(SRC)
	$(CC) $(CFLAGS) -D NDEBUG $(SRC) -o $(BINDIR)/$(EXEC_RELEASE) -lm

# Builds debug version.
debug:
	$(CC) $(CFLAGS) -g $(SRC) -o $(BINDIR)/$(EXEC_DEBUG) -lm
	

# Clean.
clean:
	rm -f $(EXEC)
	
