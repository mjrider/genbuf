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
#include "setup.h"

#include <stdlib.h>

#include "input_tcp.h"
#include "input_udp.h"
#include "input_unix.h"
#include "input_file.h"

#include "output_tcp.h"
#include "output_udp.h"
#include "output_unix.h"
#include "output_file.h"

#include "log.h"

struct input_handler*
	create_input_handler (enum io_types type, char *res)
{
	struct input_handler *retval;
	
	/* Check if valid type is specified */
	Require(type != type_unknown);

	/* Initialize all values for the correct type */
	switch (type)
	{
		case type_file:
			retval = input_handler_file_init(res);
			break;

		case type_unix:
			retval = input_handler_unix_init(res);
			break;

		case type_udp:
			retval = input_handler_udp_init(res);
			break;

		case type_tcp:
			retval = input_handler_tcp_init(res);
			break;

		default:
			Fatal(FALSE, "Unkown io_type value", NULL);
	}

	/* Sanity check the existance of the functions */
	Require (
			  retval          != NULL &&
	          retval->read    != NULL &&
			  retval->getfd   != NULL &&
	          retval->cleanup != NULL
	        );
	
	/* Return input_handler*/
	return retval;
}

struct output_handler*
	create_output_handler (enum io_types type, char *res)
{
	struct output_handler *retval;
	
	/* Check if valid type is specified */
	Require(type != type_unknown);

	/* Initialize all values for the correct type */
	switch (type)
	{
		case type_file:
			retval = output_handler_file_init(res);
			break;

		case type_unix:
			retval = output_handler_unix_init(res);
			break;

		case type_udp:
			retval = output_handler_udp_init(res);
			break;

		case type_tcp:
			retval = output_handler_tcp_init(res);
			break;

		default:
			Fatal(FALSE, "Unkown io_type value", NULL);
	}

	/* Sanity check the existance of the functions */
	Require (
		retval             != NULL &&
		retval->connect    != NULL &&
		retval->disconnect != NULL &&
		retval->timeout    != NULL &&
		retval->write      != NULL &&
		retval->cleanup    != NULL
	);

	/* Return input_handler*/
	return retval;
}

