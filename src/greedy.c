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

#include <stdlib.h>

#include <mylib/queue.h>
#include <mylib/util.h>
#include <mylib/vector.h>
#include <mylib/matrix.h>

#include "mapper.h"

/**
 * @brief Looks for the best thread not yet mapped.
 * 
 * @param threads  Communication costs.
 * @param nthreads Number of threads.
 * 
 * @returns The ID of the best thread not yet mapped.
 */
static int best_thread(const vector_t *threads, int nthreads)
{
	int best;      /* Best thread id.            */
	double *costs; /* Total communication costs. */
	
	costs = scalloc(nthreads, sizeof(double));
	
	/* Compute total costs. */
	for (int i = 0; i < nthreads; i++)
	{
		for (int j = 0; j < nthreads; j++)
			costs[i] += vector_get(threads[i], j);
	}
	
	/* Look for best thread. */
	best = 0;
	for (int i = 1; i < nthreads; i++)
	{
		if (costs[i] > costs[best])
			best = i;
	}
	
	/* House keeping. */
	free(costs);
	
	return (best);
}

/**
 * @brief Looks for the best core not in use.
 * 
 * @param Processor's information.
 * 
 * @returns The ID of the best core not in use.
 */
static int best_core(const struct processor *proc)
{
	int best;
	
	/* Look for best processor. */
	best = 0;
	for (int i = 1; i < proc->ncores; i++)
	{
		if (proc->nlinks[i] > proc->nlinks[best])
			best = i;
	}
	
	return (best);
}


/**
 * @brief Looks for the best neighbor thread not yet mapped.
 * 
 * @param thread Thread communication cost.
 * @param map    Current thread map.
 * @param nprocs Number of threads.
 * 
 * @returns The ID of the best thread.
 */
static int best_neighbor_thread(vector_t thread, int *map, int nthreads)
{
	int best;
	
	/* Look for the best neighbor thread. */
	best = -1;
	for (int i = 0; i < nthreads; i++)
	{
		/* Skip mapped threads. */
		if (map[i] >= 0)
			continue;
		
		/* First non-mapped thread. */
		if (best < 0)
		{
			best = i;
			continue;
		}
		
		/* Best thread found. */
		if (vector_get(thread, i) > vector_get(thread, best))
			best = i;
	}
	
	return (best);
}

/**
 * @brief Task.
 */
struct task
{
	int coreid; /**< Core ID. */
	int radius; /**< Radius.  */
};

/**
 * @brief Creates a task.
 * 
 * @param coreid Core ID.
 * @param radius Radius to origin core.
 * 
 * @returns A task.
 */
static struct task *task_create(int coreid, int radius)
{
	struct task *t;
	
	/* Create task. */
	t = smalloc(sizeof(struct task));
	t->coreid = coreid;
	t->radius = radius;
	
	return (t);
}

/**
 * @brief Destroys a task.
 * 
 * @param t Target task.
 */
static void task_destroy(struct task *t)
{
	free(t);
}

/**
 * @brief Asserts if a core is in use.
 * 
 * @param map      Thread map.
 * @param nthreads Number of threads.
 * @param coreid   ID of target core.
 * 
 * @returns True if the core is in use, and false otherwise.
 */
static bool core_in_use(int *map, int nthreads, int coreid)
{
	for (int i = 0; i < nthreads; i++)
	{
		if (map[i] == coreid)
			return (true);
	}
	
	return (false);
}

/**
 * @brief Looks for the best neighbor core not in use.
 * 
 * @param core    Processor's information.
 * @param map     Thread map.
 * @param nthread Number of threads.
 * @param coreid  ID of target core.
 */
static int best_neighbor_core
(const struct processor *proc, int *map, int nthreads, int coreid)
{
	int best_coreid;   /* ID of best core.  */
	int *states;       /* Processing state. */
	struct task *best; /* Best processor.   */
	struct task *t;    /* Working task.     */
	queue_t tasks;     /* Tasks to process. */
	
	/* Processing states. */
	enum states {NOT_VISITED, VISITING, VISITED};
	
	tasks = queue_create(NULL);
	
	/* Initialize processing states. */
	states = smalloc(proc->ncores*sizeof(int));
	for (int i = 0; i < proc->ncores; i++)
		states[i] = NOT_VISITED;
	
	/* Enqueue all neighbors. */
	for (int i = 0; i < proc->ncores; i++)
	{
		if (proc->topology[coreid][i])
		{
			states[i] = VISITING;
			queue_enqueue(tasks, task_create(coreid, 1));
		}
	}
	
	/* Look for the best neighbor core. */
	best = NULL;
	while (true)
	{
		t = queue_dequeue(tasks);
		
		/* Enqueue all neighbors. */
		for (int i = 0; i < proc->ncores; i++)
		{
			if (proc->topology[t->coreid][i])
			{
				states[i] = VISITING;
				queue_enqueue(tasks, task_create(i, t->radius + 1));
			}
		}
		
		states[t->coreid] = VISITED;
		
		/* Skip cores that are in use. */
		if (core_in_use(map, nthreads, t->coreid))
		{
			task_destroy(t);
			continue;
		}
		
		/* First core that is not in use. */
		if (best == NULL)
		{
			best = t;
			continue;
		}
		
		/* Done. */
		if (t->radius > best->radius)
		{
			task_destroy(t);
			break;
		}
		
		/* Best core found. */
		if (t->radius == best->radius)
		{
			if (proc->nlinks[t->coreid] > proc->nlinks[best->coreid])
			{
				task_destroy(best);
				best = t;
				continue;
			}
		}
		
		task_destroy(t);
	}
	
	best_coreid = best->coreid;
	
	/* House keeping. */
	task_destroy(best);
	while (!queue_empty(tasks))
		task_destroy(queue_dequeue(tasks));
	queue_destroy(tasks);
	free(states);
	
	return (best_coreid);
}

/**
 * @brief Maps threads using a greedy heuristic.
 *
 * @param communication Communication matrix.
 * @param args          Additional arguments.
 *
 * @returns A process map.
 */
int *map_greedy(matrix_t communication, void *args)
{
	int *map;               /* Thread map.              */
	int threadid;           /* Best thread.             */
	int coreid;             /* Best core.               */
	struct processor *proc; /* Processor's information. */
	int nthreads;           /* Number of threads.       */
	vector_t *threads;      /* Threads.                 */
	
	/* Sanity check. */
	assert(communication != NULL);
	assert(args != NULL);
	
	proc = ((struct greedy_args *)args)->proc;
	
	nthreads = matrix_height(communication);
	
	/* Create processes. */
	threads = smalloc(nthreads*sizeof(vector_t));
	for (int i = 0; i < nthreads; i++)
	{
		threads[i] = vector_create(nthreads);
		for (int j = 0; j < nthreads; j++)
			vector_set(threads[i], j, matrix_get(communication, i, j));
	}
	
	/* Initialize process map. */
	map = smalloc(nthreads*sizeof(int));
	for (int i = 0; i < nthreads; i++)
		map[i] = -1;
	
	threadid = best_thread(threads, nthreads);
	coreid = best_core(proc);
	
	map[threadid] = coreid;
	
	/* Map all threads. */
	for (int i = 1; i < nthreads; i++)
	{
		threadid = best_neighbor_thread(threads[threadid], map, nthreads);
		coreid = best_neighbor_core(proc, map, nthreads, coreid);
		
		map[threadid] = coreid;
	}

	/* House keeping. */
	for (int i = 0; i < nthreads; i++)
		vector_destroy(threads[i]);
	free(threads);
	
	return (map);
}
