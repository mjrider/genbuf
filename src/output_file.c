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
#include "output_file.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>

#include "output_tools.h"
#include "log.h"

static int output_file_connect (struct output_handler *this)
{
	/* Check if we're in a valid state */
	Require(this->state == os_disconnected);
	
	/* If output is stdout, do nothing */
	if (this->fd == 1)
	{
		Log2(info, "Using stdout as output file", "output_file.c:{connect}");
		this->state = os_ready;
		return TRUE;
	}

	this->fd = open(this->res, O_WRONLY|O_CREAT|O_APPEND);

	/* Check if open succeeded */
	if (this->fd == -1)
	{
		SysErr(errno, "While trying to open output file");
		this->err = errno;
		return FALSE;
	}
	
	Log2(info, "Using file for output file", "output_file.c{connect}");
	
	/* All went well */
	this->state = os_ready;
	return TRUE;
}

static int output_file_disconnect (struct output_handler *this)
{
	/* Check if we're in a valid state */
	Require(
		this->state == os_connecting ||
		this->state == os_ready      ||
		this->state == os_error
	);

	
	/* If output is stdout, do nothing */
	if (this->fd == 1)
	{
		Log2(warning, "Cannout close stdout as a file", "output_file.c:{disconnect}");
		this->state = os_ready;
		return TRUE;
	}

	/* Try and close the file */
	if (close(this->fd) == -1)
	{
		this->fd = -1;
		this->err = errno;
		SysErr(this->err, "While closing output file");
		return FALSE;
	}
	
	Log2(info, "Closed output file filehandle", "output_file.c:{disconnect}");
	
	/* All went well */
	this->fd = -1;
	this->state = os_disconnected;
	return TRUE;
}

struct output_handler *output_handler_file_init (char *res)
{
	struct output_handler *retval = output_handler_common_init("file", res, (strcmp(res, "-") ? -1 : 1));
	
	retval->state      = os_ready;
	retval->connect    = output_file_connect;
	retval->disconnect = output_file_disconnect;

	return retval;
}
