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

#ifndef OUTPUT_TOOLS_H
#define OUTPUT_TOOLS_H

#include "output.h"

extern int output_handler_common_write   (struct output_handler*, char *msg, int msglen, int *done);
extern int output_handler_common_cleanup (struct output_handler *);
extern int output_handler_common_nothing (struct output_handler*);

extern struct output_handler *output_handler_common_init  (char *type, char *res, int fd);

#endif /* OUTPUT_TOOLS_H */
