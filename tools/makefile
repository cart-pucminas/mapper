#
# Copyright(C) 2015-2015 by Pedro H. Penna <pedrohenriquepenna@gmail.com>
#

.PHONY: trace-parser

# Builds all tools.
all: nas2tpz trace-packer trace-parser


# Builds NPB to TOPAZ trace conversor.
nas2tpz: nas2tpz.c
	$(CC) $(CFLAGS) nas2tpz.c -o $(BINDIR)/nas2tpz

# Builds PIN trace packer.
trace-packer: trace-packer.c
	$(CC) $(CFLAGS) trace-packer.c -o $(BINDIR)/trace-packer

# Builds PIN trace parser.
trace-parser: trace-parser/*.c
	$(CC) $(CFLAGS) trace-parser/*.c -o $(BINDIR)/trace-parser

# Cleans compilation files.
clean:
	rm -f $(BINDIR)/nas2tpz
	rm -f $(BINDIR)/trace-packer
	rm -f $(BINDIR)/trace-parser

