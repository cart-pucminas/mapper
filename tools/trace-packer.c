/*
 * Copyright(C) 2015 Pedro H. Penna <pedrohenriquepenna@gmail.com>
 *
 * This file is part of Mapper.
 *
 * Mapper is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Mapper is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MyLib. If not, see <http://www.gnu.org/licenses/>.
 */

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
