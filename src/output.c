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

#include "output.h"

#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "log.h"

int deliver_message (struct output_handler *handler, char *msg)
{
	struct timeval timeout;
	int todo, retry, msglen;
	fd_set fds;

	/* Initialize variables */
	FD_ZERO(&fds);
	todo = msglen = strlen(msg);
	retry = handler->retry;

	/* Try to send the message with reasonable effort */
	while (retry > 0)
	{
		/* If not connected, connect */
		if (handler->state == os_disconnected)
		{
			if (!handler->connect(handler))
			{
				retry--;
			}
		}

		/* If valid filedescriptor, listen for signals */
		if (handler->fd != -1)
		{
			/* Add the handler's fd to the watchlist */
			FD_SET(handler->fd, &fds);
		}
		
		/* Set maximum timeout */
		timeout.tv_sec  = 10;
		timeout.tv_usec = 0;
		
		switch(select(handler->fd + 1, NULL, &fds, NULL, &timeout))
		{
			/* Error occured */
			case -1:
				SysErr(errno, "While doing select for message sending");
				return 0;

			/* Timeout occured */
			case 0:
				Log2(debug, "Output handling timeout", "[output.c]{deliver_message}");
				retry--;
				break;

			/* Activity on filedescriptor */
			default:
				Log2(debug, "Output is active", "[output.c]{deliver_message}");
				switch (handler->state)
				{
					/* Try to connect */
					case os_disconnected:
						handler->connect(handler);
						break;
						
					/* Check if connection is established */
					case os_connecting:
						handler->connect(handler);
						break;
					
					/* Try to send the message */
					case os_ready:
					case os_sending:
						if (FD_ISSET(handler->fd, &fds) && handler->write(handler, msg, msglen, &todo))
						{
							/* Write succesfully completed */
							return 1;
						}
						break;
						
					/* Do a reset */
					case os_error:
						break;
						
					/* Impossible */
					default:
						Log2(impossible, "IMPOSSIBLE state", "[output.c]{deliver_message}");
						return 0;
				}

				/* Continue */
				break;
		}
		
		/* If we're in error state, disconnect */
		if (handler->state == os_error)
		{
			SysErr(handler->err, "[output.c]{deliver_message} Handler is in an error state");
			handler->disconnect(handler);
			retry --;
		}
	}

	/* Out of retry's */
	Log(error, "Maximum amount of retry's reached");
	return 0;
}
