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

#ifndef GENCACHE_INPUT_H
#define GENCACHE_INPUT_H

#include "reader.h"
#include "types.h"

/** Input Handler module interface
 *
 * Params:
 *   res   -> Resource specifier (module dependant)
 *
 * Returns:
 *   int   -> Non-zero on failure
 *
 */
struct input_handler {
	void *priv;
	char *err;
	int   (*read)    (struct input_handler*, struct reader*);            /* read returns: -1 on EOF */
	int   (*getfd)   (struct input_handler*);
	int   (*cleanup) (struct input_handler*);
};

#else
struct input_handler;
#endif /* GENCACHE_INPUT_H */
