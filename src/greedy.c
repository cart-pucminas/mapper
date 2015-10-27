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
		if (map[i] > 0)
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
	int *visited;      /* Visited cores.    */
	struct task *best; /* Best processor.   */
	struct task *t;    /* Working task.     */
	queue_t tasks;     /* Tasks to process. */
	
	visited = scalloc(proc->ncores, sizeof(int));
	tasks = queue_create(NULL);
	
	/* Enqueue all neighbors. */
	for (int i = 0; i < proc->ncores; i++)
	{
		if (proc->topology[coreid][i])
			queue_enqueue(tasks, task_create(proc->topology[coreid][i], 1));
	}
	
	/* Look for the best neighbor core. */
	best = NULL;
	while (true)
	{
		t = queue_dequeue(tasks);
		
		/* Skip visited cores. */
		if (visited[t->coreid])
		{
			task_destroy(t);
			continue;
		}
		
		visited[t->coreid] = 1;
		
		/* Enqueue all neighbors. */
		for (int i = 0; i < proc->ncores; i++)
		{
			if (proc->topology[t->coreid][i])
			{
				queue_enqueue(tasks, 
					task_create(proc->topology[t->coreid][i], t->radius + 1));
			}
		}
		
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
			break;
		
		/* Best core found. */
		if (t->radius == best->radius)
		{
			if (proc->nlinks[t->coreid] > proc->nlinks[best->coreid])
				best = t;
		}
	}
	
	best_coreid = best->coreid;
	
	/* House keeping. */
	while (!queue_empty(tasks))
		task_destroy(queue_dequeue(tasks));
	free(visited);
	
	return (best_coreid);
}

/**
 * @brief Maps threads using a greedy heuristic.
 *
 * @param threads  Threads.
 * @param nthreads Number of threads.
 * @param args     Additional arguments.
 *
 * @returns A process map.
 */
int *map_greedy(const vector_t *threads, int nthreads, void *args)
{
	int *map;               /* Thread map.              */
	int threadid;           /* Best thread.             */
	int coreid;             /* Best core.               */
	struct processor *proc; /* Processor's information. */
	
	/* Sanity check. */
	assert(threads != NULL);
	assert(nthreads > 0);
	assert(args != NULL);
	
	proc = ((struct greedy_args *)args)->proc;
	
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
	
	return (map);
}
