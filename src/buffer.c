/*Generic multiplexing line buffering tool
 * Copyright (C) 2004 Justin Ossevoort
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "defines.h"
#include "buffer.h"

#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h>

#include "log.h"

#define REALLOC_SIZE 100

static void buffer_push (struct buffer *buff, char *str)
{
	pthread_mutex_lock(&(buff->mutex));

	if ((buff->count % REALLOC_SIZE) == 0)
	{
		buff->strings = realloc(buff->strings, (buff->count + REALLOC_SIZE) * sizeof(char*));
	}

	buff->strings[buff->count] = str;
	buff->count++;
	
	pthread_mutex_unlock(&(buff->mutex));
	
	sem_post(&(buff->semaphore));
}

static void buffer_unpop (struct buffer *buff, char *msg)
{
	pthread_mutex_lock(&(buff->mutex));

	if ((buff->count % REALLOC_SIZE) == 0)
	{
		buff->strings = realloc(buff->strings, (buff->count + REALLOC_SIZE) * sizeof(char*));
	}

	memmove(buff->strings + 1, buff->strings, buff->count * sizeof(char*));
	
	buff->strings[0] = msg;
	buff->count++;
	
	pthread_mutex_unlock(&(buff->mutex));
	
	sem_post(&(buff->semaphore));
}

static char *buffer_pop (struct buffer *buff)
{
	char *retval;

	sem_wait(&(buff->semaphore));

	pthread_mutex_lock(&(buff->mutex));

	buff->count--;
	retval = buff->strings[0];
	memmove(buff->strings, buff->strings + 1, buff->count * sizeof(char*));
	buff->strings[buff->count] = NULL;

	if ((buff->count % REALLOC_SIZE) == 0)
	{
		buff->strings = realloc(buff->strings, (buff->count + REALLOC_SIZE) * sizeof(char*));
	}

	
	pthread_mutex_unlock(&(buff->mutex));

	return retval;
}

static int buffer_size (struct buffer *buff)
{
	int retval;
	
	sem_getvalue(&(buff->semaphore), &retval);

	return retval;
}

struct buffer*
	buffer_init ()
{
	struct buffer *buff = (struct buffer*) malloc (sizeof(struct buffer));

	Fatal(buff == NULL, "Malloc returned NULL", "Buffer creation failed");
	
	sem_init(&(buff->semaphore), 0, 0);
	pthread_mutex_init(&(buff->mutex), NULL);

	buff->count   = 0;
	buff->strings = NULL;

	buff->push  = buffer_push;
	buff->pop   = buffer_pop;
	buff->unpop = buffer_unpop;
	buff->size  = buffer_size;

	return buff;
}

int buffer_cleanup(struct buffer *buff)
{
	int i;
	
	pthread_mutex_destroy(&(buff->mutex));
	sem_destroy(&(buff->semaphore));

	for (i = 0; i < buff->count; i++)
		free(buff->strings[i]);

	free(buff->strings);
	free(buff);

	return 0;
}
