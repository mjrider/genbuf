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
#include "input_unix.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stddef.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <err.h>

#include "input_tools.h"
#include "log.h"

static int make_named_socket (const char *filename)
{
	struct sockaddr_un name;
	int sock;
	size_t size;

	/* Create the socket. */
	sock = socket (PF_LOCAL, SOCK_DGRAM, 0);
	if (sock < 0)
	{
		perror ("socket");
		exit (EXIT_FAILURE);
	}

	/* Bind a name to the socket. */
	name.sun_family = AF_LOCAL;
	strncpy (name.sun_path, filename, sizeof (name.sun_path));
	name.sun_path[sizeof (name.sun_path) - 1] = '\0';

	size = (offsetof (struct sockaddr_un, sun_path) + strlen (name.sun_path) + 1);

	return bind(sock, (struct sockaddr *) &name, size);
}

static int connect_to_named_socket (const char *filename)
{
	struct sockaddr_un name;
	int sock;
	size_t size;

	/* Create the socket. */
	sock = socket (PF_LOCAL, SOCK_DGRAM, 0);
	if (sock < 0)
	{
		perror ("socket");
		exit (EXIT_FAILURE);
	}

	/* Bind a name to the socket. */
	name.sun_family = AF_LOCAL;
	strncpy (name.sun_path, filename, sizeof (name.sun_path));
	name.sun_path[sizeof (name.sun_path) - 1] = '\0';

	size = (offsetof (struct sockaddr_un, sun_path) + strlen (name.sun_path) + 1);

	return connect(sock, (struct sockaddr *) &name, size);
}

struct input_handler *input_handler_unix_init (char *res)
{
	int fd;

	fd = connect_to_named_socket(res);
	if (fd == -1)
	{
		SysErr(errno, "While trying to connect to socket");
		Log(debug, "Trying to create a unix socket");
		SysFatal((fd = make_named_socket(res)) == -1, errno, "On creating unix socket");
	}

	return input_handler_common_init("unix", res, fd);
}
