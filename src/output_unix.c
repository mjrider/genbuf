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

#include "output_unix.h"

#include <sys/types.h>
#include <strings.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include "output_tools.h"
#include "log.h"

static int output_unix_connect (struct output_handler *this)
{
	return 0;
}

static int output_unix_disconnect (struct output_handler *this)
{
	return 0;
}

static int output_unix_write (struct output_handler *this, char *str, int strlen, int*send)
{
	return 0;
}

static int output_unix_cleanup (struct output_handler *this)
{
	return 0;
}

struct output_handler *output_handler_unix_init (char *res)
{
	struct output_handler *retval = output_handler_common_init("file", res, -1);
	
	retval->connect    = output_unix_connect;
	retval->disconnect = output_unix_disconnect;
	retval->write      = output_unix_write;
	retval->cleanup    = output_unix_cleanup;

	return retval;
}
