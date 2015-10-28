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

.PHONY: trace-parser

# Builds all tools.
all: nas2tpz trace-packer trace-parser map2nas

# Builds NAS trace instrumentation tool.
map2nas: map2nas.c
	$(CC) $(CFLAGS) map2nas.c -o $(BINDIR)/map2nas $(LIBS)

# Builds NPB to TOPAZ trace conversor.
nas2tpz: nas2tpz.c
	$(CC) $(CFLAGS) nas2tpz.c -o $(BINDIR)/nas2tpz $(LIBS)

# Builds PIN trace packer.
trace-packer: trace-packer.c
	$(CC) $(CFLAGS) trace-packer.c -o $(BINDIR)/trace-packer

# Builds PIN trace parser.
trace-parser: trace-parser/*.c
	$(CC) $(CFLAGS) trace-parser/*.c -o $(BINDIR)/trace-parser $(LIBS)

# Cleans compilation files.
clean:
	rm -f $(BINDIR)/nas2tpz
	rm -f $(BINDIR)/trace-packer
	rm -f $(BINDIR)/trace-parser

