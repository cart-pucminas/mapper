#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

int main(int argc, char **argv)
{
	char rw;
	int size;
	uint64_t addr;
	char line[80];

	(void)(argc);
	(void)(argv);

	while (fgets(line, sizeof(line), stdin) != NULL)
	{
		sscanf(line, "%c%d%" PRIu64 "%*d", &rw, &size, &addr);
		fprintf(stdout, "%c;%u;%" PRIx64 "\n", rw, size, addr);
	}

	return (EXIT_SUCCESS);
}
