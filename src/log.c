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
#include "log.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>

#define TIMESPEC_LEN	64

enum log_levels current_log_level = impossible;

/* To shut up the compiler, we don't consider this a bug (see 'man strftime')*/
static int my_strftime(char *buf, int buflen, const char *format, const struct tm *thetime)
{
	return strftime(buf, buflen, format, thetime);
}

void Log2 (enum log_levels level, const char *err, const char *msg)
{
	char timespec[TIMESPEC_LEN];
	struct tm *localized;
	time_t now;

	/* Check if we shoud log this */
	if (level > current_log_level)
		return;
	
	/* Resolve the localized time */
	time(&now);
	localized = localtime(&now);
	my_strftime(timespec, TIMESPEC_LEN, "%c", localized);

	/* If not specified, specify log message*/
	if (msg == NULL)
		msg = "Log";
	
	/* Print it */
	fprintf(stderr, ">>> %s genbuf: %s: %s\n", timespec, msg, err);
}

void Log (enum log_levels level, const char *msg)
{
	Log2(level, msg, NULL);
}

void Require (int cond)
{
	if (! cond)
	{
		Log2(critical, "Failed!", "Requirement");
		abort();
	}
}

void Fatal (int cond, const char *err, const char *msg)
{
	if (cond)
	{
		if (msg == NULL)
			msg = "Fatal";
		
		Log2(critical, err, msg);

		abort();
	}
}

void SysFatal (int cond, int err, const char *msg)
{
	if (cond)
	{
		if (msg == NULL)
			msg = "SysFatal";

		Log2(critical, strerror(err), msg);

		abort();
	}
}

void NotNull (const void *pointer)
{
	Fatal (pointer == NULL, "Failed!", "Not-NULL check");
}

void SysErr (int err, const char *msg)
{
	if (msg == NULL)
		msg = "System error";
	
	Log2(error, strerror(errno), msg);
}
