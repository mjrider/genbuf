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

#ifndef GENCACHE_INPUT_BUFFER_H
#define GENCACHE_INPUT_BUFFER_H

struct input_buffer {
	char *start;   /* Points to the beginning of the buffer            */
	char *current; /* Current position to write in the buffer          */
	char *border;  /* Internal offset of the buffer                    */
	char *end;     /* Points to the end of the buffer                  */
	int available; /* Number of bytes unused in buffer                 */
	int write;     /* Number of bytes that can be written at 'current' */
};

extern struct input_buffer *input_buffer_create (int size);

extern void  input_buffer_update    (struct input_buffer *buffer, int read);
extern  int  input_buffer_find      (struct input_buffer *buffer, int c);
extern  int  input_buffer_validate  (struct input_buffer *buffer);
extern  int  input_buffer_purgeline (struct input_buffer *buffer);
extern char *input_buffer_getline   (struct input_buffer *buffer);

extern void  input_buffer_free      (struct input_buffer *buffer);

#endif /* GENCACHE_INPUT_BUFFER_H */
