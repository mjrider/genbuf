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

#ifndef GENCACHE_LOG_H
#define GENCACHE_LOG_H

/* Different log levels */
enum log_levels {
	impossible = 0,
	critical   = 1,
	error      = 2,
	warning    = 3,
	info       = 4,
	debug      = 5
};

extern enum log_levels current_log_level;

extern void      Log (enum log_levels level, const char *msg);
extern void     Log2 (enum log_levels level, const char *err, const char *msg);
extern void  Require (int cond);
extern void    Fatal (int cond, const char *err, const char *msg);
extern void SysFatal (int cond, int err, const char *msg);
extern void  NotNull (const void *pointer);
extern void   SysErr (int err, const char *msg);

#endif /* GENCACHE_LOG_H */
