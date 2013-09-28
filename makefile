#
# Copyright(C) 2013 Pedro H. Penna <pedrohenriquepenna@gmail.com>
#

# Source files.
SRC = $(wildcard src/*.c)        \
	  $(wildcard src/list/*.c)   \
	  $(wildcard src/matrix/*.c) \
	  $(wildcard src/vector/*.c) \

# Executable file.
EXEC = mapper

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

# Build.
all: $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(BINDIR)/$(EXEC) -lm

# Clean.
clean:
	rm -f $(EXEC)
	
