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
#include "input_file.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <err.h>

#include "input_tools.h"
#include "log.h"

struct input_handler *input_handler_file_init (char *res)
{
	int fd;
	
	/* Open the input stream */
	if (strcmp(res, "-") == 0)
	{
		/* This means STDIN */
		fd = 0;
	}
	else
	{
		/* Resource specifier should be a file, so try opening it */
		fd = open(res, O_RDONLY|O_NONBLOCK);
	
		/* Check to see if open failed */
		SysFatal(fd == -1, errno, "[File input handler] On opening file");
	}

	return input_handler_common_init("file", res, 0);
}
