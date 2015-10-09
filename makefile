# Copyright(C) 2015 Pedro H. Penna <pedrohenriquepenna@gmail.com>
#
# This file is part of Mapper.
#
# Mapper is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Mapper is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with MyLib. If not, see <http://www.gnu.org/licenses/>.

# Executable files.
export EXEC = mapper


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
export MYLIB = mylib-0.6
export LIBS = $(LIBDIR)/libmy.a -fopenmp -lm

# Toolchain configuration.
export CFLAGS += -std=c99
export CFLAGS += -Wall -Wextra
export CFLAGS += -O3
export CFLAGS += -I $(INCDIR) 

# Phony list.
.PHONY: tools

# Builds mapper.
all: lib
	cd $(SRCDIR) && $(MAKE) all

# Builds library.
lib:
	cd $(CONTRIBDIR) &&                                \
	mkdir -p $(MYLIB) &&                               \
	tar -xjvf $(MYLIB).tar.bz2 --directory $(MYLIB) && \
	cd $(MYLIB) &&                                     \
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
	cd src && $(MAKE) clean
	rm -rf $(INCDIR)/mylib
	rm -rf $(LIBDIR)
	rm -f $(DOCDIR)/*.pdf
	
