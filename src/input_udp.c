#include "defines.h"
#include "input_udp.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <err.h>

#include "input_tools.h"
#include "net_tools.h"
#include "log.h"

struct input_handler *input_handler_udp_init (char *res)
{
	int fd, proto;
	
	/* Open the input stream */
	proto = net_get_protocol ("udp");
	fd    = net_create_listening_socket(res, "udp", proto);

	return input_handler_common_init("udp", res, fd);
}
