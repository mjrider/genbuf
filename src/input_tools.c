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
#include "input_tools.h"

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <err.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "input_buffer.h"
#include "log.h"

void set_error (struct input_handler *this, char *err, char *msg)
{
	/* Calculate the error message size (+2 for ': ') */
	int errlen, msglen, len;

	errlen = strlen(err);
	msglen = strlen(msg);
	len    = errlen + msglen + 2;

	/* If old error present, clear it */
	if (this->err != NULL)
	{
		free(this->err);
	}

	/* Allocate buffer for error message */
	this->err = malloc (len + 1);

	/* Check buffer allocation */
	SysFatal(this->err == NULL, errno, "On error message allocation");

	/* Fill buffer */
	memcpy(this->err, err, errlen);
	memcpy(this->err + errlen, ": ", 2);
	memcpy(this->err + errlen + 2, msg, msglen);
	this->err[errlen] = '\0';
}

int input_handler_common_read (struct input_handler *this, struct reader *report)
{
	int err, readcount;
	char *msg;

	Log2(debug, "Read requested", "[input_tools.c]{read}");

	/* Check if this input source is in a valid state */
	if (DATA->state == is_eof)
	{
		Log2(debug, "Already at EOF", "[input_tools.c]{read}");
		return -1;
	}

	/* Read data */
	Require (DATA->inbuf->write > 0);
	Require (DATA->inbuf->available > 0);
	readcount = read(DATA->fd, DATA->inbuf->current, DATA->inbuf->write);
	err = errno;

	switch (readcount)
	{
		case -1:
			SysErr(err, "Error occured during read from input source");
			/* Error */
			switch(err)
			{
				case EAGAIN:
				case EINTR:
					/* Recoverable */
					Log2(debug, "Recovered", "[input_tools.c]{read}");
					return 0;
					
				default:
					/* Fatal */
					Log2(debug, "Input is dead", "[input_tools.c]{read}");
					DATA->state = is_eof;
					return -1;
			}

		case 0:
			/* EOF? */
			Log2(debug, "Read '0' bytes from input source", "[input_tools.c]{read}");
			SysErr(err, "Read '0' bytes from input source");
			DATA->state = is_eof;
			return -1;

		default:
			/* Successfull read */
			Log2(debug, "Succesfully read data", "[input_tools.c]{read}");
			input_buffer_update(DATA->inbuf, readcount);
	}

	/* Check if we're in an error state */
	if (DATA->state == is_err)
	{
		/* Purge state */
		if (! input_buffer_purgeline(DATA->inbuf))
		{
			Log2(debug, "Purging is not done", "[input_tools.c]{read}");
			DATA->state = is_err;
			return 0;
		}
		
		/* We purged the remainder of the oversized line, continue reading */
		DATA->state = is_ready;
	}
	
	/* Report all complete read lines */
	while ((msg = input_buffer_getline(DATA->inbuf)) != NULL)
	{
		/* Successfully read a line */
		Log2(debug, msg, "[input_tools.c]{read} data");
		report->report_data(report, msg);
	}

	/* Check if there's no buffer space left */
	if (DATA->inbuf->available == 0)
	{
		Log2(debug, "Buffer is full and no line is available, start purging", "[input_tools.c]{read}");
		DATA->state = is_err;
		Fatal(input_buffer_purgeline(DATA->inbuf), "I should have read a line", "input_tools_common_read->purgeline(2)");
	}

	Log2(debug, "Read done", "[input_tools.c]{read}");
	return 0;
}

int input_handler_common_getfd (struct input_handler *this)
{
	/* Return the filedescriptor */
	return DATA->fd;
}

int input_handler_common_cleanup (struct input_handler *this)
{
	input_buffer_free(DATA->inbuf);
	
	/* Close the filedescriptor */
	if (close(DATA->fd) == -1)
	{
		SysErr(errno, "[Common input] On closing file");
		return 0;
	}

	free(DATA);
	free(this);
	
	/* All went well */
	return 1;
}

struct input_handler *input_handler_common_init (char *type, char *res, int fd)
{
	/* Declare and create basic input_handler structure */
	struct input_handler *this =
		(struct input_handler*) malloc (sizeof(struct input_handler));

	/* Check memory allocation */
	Fatal(this == NULL, "Memory allocation failed!", "Error when loading input handler");

	/* Print a checkpoint log message */
	Log2(debug, res, "CP:IH[file]->init");
	
	/* Allocate private data space */
	this->priv = malloc (sizeof(struct ih_common_priv));

	/* Check to see wheter resource specification succeeded */
	SysFatal(this->priv == NULL, errno, "When allocating private data");
	
	/* Initialize fields */
	this->err     = NULL;
	this->read    = input_handler_common_read;
	this->getfd   = input_handler_common_getfd;
	this->cleanup = input_handler_common_cleanup;

	/* Allocate initial msg buffer */
	DATA->inbuf = input_buffer_create(GENCACHE_MAX_MSG_SIZE);

	/* Register FD */
	DATA->fd    = fd;
	DATA->state = is_ready;

	return this;
}
