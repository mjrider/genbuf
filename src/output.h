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

#ifndef GENCACHE_OUTPUT_H
#define GENCACHE_OUTPUT_H

#include "types.h"

/** Output Handler module interface
 *
 * Params:
 *   res   -> Resource specifier (module dependant)
 *   str   -> String to be send
 *
 * Returns:
 *   int   -> Non-zero on failure
 *
 */
struct output_handler {
	enum os_state {
		os_disconnected,
		os_connecting,
		os_ready,
		os_sending,
		os_error
	} state;

	int   fd;
	int   err;
	int   retry;
	char *res;
	char *type;
	void *priv;
	
	int   (*connect)    (struct output_handler*);	/* Connect to destination resource */
	int   (*disconnect) (struct output_handler*);	/* Disconnect from destination resource */
	int   (*timeout)    (struct output_handler*);	/* Timeout occured, destination dependant action */
	int   (*write)      (struct output_handler*, char *str, int strlen, int *send);	/* (Continue?) Send message to destination */
	int   (*cleanup)    (struct output_handler*);	/* Tidy up */
};

extern int deliver_message (struct output_handler*, char *msg);	/* Overall statefull logic processor, easy to use sender :) */

#endif /* GENCACHE_OUTPUT_H */
