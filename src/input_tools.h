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

#ifndef GENCACHE_INPUT_TOOLS_H
#define GENCACHE_INPUT_TOOLS_H

#include "input.h"
#include "input_buffer.h"

/* Special states we recognize */
enum input_state {
	is_ready,
	is_err,
	is_eof
};

/* Input handler private data */
#define	DATA	((struct ih_common_priv*) this->priv)
struct ih_common_priv {
                 int  fd;
 struct input_buffer *inbuf;
    enum input_state  state;
};

/* Tooling functions */
extern void
	set_error (struct input_handler *, char *err, char *msg);

extern int
	input_handler_common_getfd(struct input_handler*);
extern int
	input_handler_common_cleanup(struct input_handler*);
extern int
	input_handler_common_read(struct input_handler*, struct reader*);
extern struct input_handler *
	input_handler_common_init(char *type, char *res, int fd);
	
#endif /* GENCACHE_INPUT_TOOLS_H */
