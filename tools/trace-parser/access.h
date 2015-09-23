/*
 * Copyright(C) 2015 Amanda Amorim <amandamp.amorim@gmail.com>
 *                   Pedro H. Penna <pedrohenriquepenna@gmail.com>
 * 
 * This file is part of Mapper.
 * 
 * Mapper is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Mapper is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Mapper. If not, see <http://www.gnu.org/licenses/>.
 */
 
#ifndef ACCESS_H_
#define ACCESS_H_

	#include <stdio.h>

	/**
	 * @brief Memory access.
	 */
	struct access
	{
		uint64_t addr;  /** Address.            */
		int access[12]; /**< Access per thread. */
	};
	
	/* Forward definitions. */
	extern struct access *access_create(void);
	extern void access_destroy(struct access *);

#endif /* ACCESS_H_ */
