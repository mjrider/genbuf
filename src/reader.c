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
#include "reader.h"

#include <sys/select.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include "buffer.h"
#include "log.h"

#define HANDLERS_STEPPING	8

static void reader_add_source (struct reader *this, struct input_handler *handler)
{
	void *tmp;

	if (this->handlers_alloc % HANDLERS_STEPPING == 0)
	{
		tmp = realloc(this->handlers, (this->handler_count + HANDLERS_STEPPING + 1) * sizeof(struct handler_list));

		if (tmp != NULL)
		{
			this->handlers = (struct handler_list*) tmp;
			
			this->handlers[this->handler_count].handler = handler;
			this->handlers[this->handler_count].fd      = handler->getfd(handler);

			Require(this->handlers[this->handler_count].fd != -1);

			this->handler_count++;
		}
		else
		{
			SysErr(errno, "[Reader] Realloc for more handlers failed");
			Log2(warning, "Memory reallocation failed", "Input handler ignored");
			handler->cleanup(handler);
		}
	}
}

static void remove_handler (struct reader *this, int index)
{
	struct input_handler *handler;
	
	Require(index >= 0 && index < this->handler_count);

	FD_CLR(this->handlers[index].fd, &this->fds);
	handler = this->handlers[index].handler;
	
	if (index < this->handler_count - 1)
	{
		/* If not move the rest one place to the front */
		memmove(
			this->handlers + index,
			this->handlers + (index + 1),
			sizeof(struct handler_list)
		);
	}

	/* Make the handler tidy up */
	handler->cleanup(handler);
	
	/* One handler less to worry about */
	this->handler_count--;
}

static void reader_run (struct reader *this)
{
	int i, n, fd, active, status;

	do
	{
		active = 0;
		n = 0;
	
		/* Cycle through the handlers to see who's responisble */
		for (i = 0; i < this->handler_count; i++)
		{
			fd = this->handlers[i].fd;
			
			if (FD_ISSET(fd, &this->fds))
			{
				struct input_handler *handler = this->handlers[i].handler;
				
				/* Got message, push it on the queue */
			Log(debug, "I'm trying to cope with something here");
				status = handler->read(handler, this);
			Log(debug, "I think I coped with that quite nicely");
				
				switch (status)
				{
					case -1:
						remove_handler(this, i);
						break;
					case 0:
						/* Nothing special to report */
						break;
					default:
						fprintf(stderr, "Handler returned: %d!\n", status);
				}
			}
			else
			{
				FD_SET(fd, &this->fds);
			}

			n = MAX(fd, n);
		}
		
		/* Check if there is anything left listening for */
		if (this->handler_count == 0)
			return;

		n++;
		Log(debug, "Calling select()");
	}
	while ((active = select(n, &this->fds, NULL, NULL, NULL)) != -1);
	
	SysErr(errno, "Select failed");
}

static void reader_report_data (struct reader *this, char *data)
{
	Require(data != NULL);
	
	this->buffer->push(this->buffer, data);
}

static void reader_cleanup (struct reader *this)
{
	int i;
	
	/* Cleanup the handlers */
	for (i = 0; i < this->handler_count; i++)
	{
		this->handlers[i].handler->cleanup(this->handlers[i].handler);
	}

	/* Add the finished symbol to the buffer */
	this->buffer->push(this->buffer, NULL);

	/* Cleanup the handlers_list */
	if (this->handlers != NULL)
		free(this->handlers);
	
	/* Cleanup ourselves */
	free(this);
}

struct reader *reader_init(struct buffer *buffer)
{
	struct reader *retval = (struct reader*) malloc (sizeof(struct reader));

	SysFatal (retval == NULL, errno, "On reader structure allocation");
		
	retval->handler_count  = 0;
	retval->handlers_alloc = 0;
	retval->handlers       = NULL;
	retval->buffer         = buffer;

	FD_ZERO(&retval->fds);

	retval->add_source  = reader_add_source;
	retval->run         = reader_run;
	retval->report_data = reader_report_data;
	retval->cleanup     = reader_cleanup;

	return retval;
}

