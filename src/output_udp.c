#include "defines.h"
#include "output_udp.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <strings.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include "net_tools.h"
#include "output_tools.h"
#include "log.h"

#define PRIVATE ((struct priv*) this->priv)
struct priv {
	int proto;
};

static int output_udp_connect (struct output_handler *this)
{
	struct sockaddr_in addr;

	Require(
		this != NULL &&
		(
			(this->state == os_disconnected && this->fd == -1) ||
			(this->state == os_connecting && this->fd >= 0)
		)
	);

	/* If initial connect attempt, create socket */
	if (this->state == os_disconnected)
	{
		Log2(info, "Creating new socket", "[UDP output handler]");
		this->fd = net_create_socket (PRIVATE->proto, this->type);
	}
	
	/* Resolve address */
	if (!net_get_socketaddr(&addr, this->res))
	{
		this->err = errno;
		SysErr(this->err, "While trying to make outgoing udp connection");
		this->state = os_error;
		return FALSE;
	}

	/* Try to connect */
	if (connect(this->fd, &addr, sizeof(struct sockaddr_in)) == -1)
	{
		this->err = errno;
		switch(this->err)
		{
			/* Connection is established */
			case EISCONN:
//				shutdown(this->fd, 0);
				this->state = os_ready;
				return TRUE;
			
			/* Temporary failure */
			case EAGAIN:
			case EINPROGRESS:
			case EALREADY:
				this->state = os_connecting;
				return TRUE;
			
			/* Unknown failure */
			default:
				SysErr(this->err, "While trying to establish outgoing udp connection");
				this->state = os_error;
				return FALSE;
		}
	}

	/* All went well, we're connected */
	this->state = os_ready;
	return TRUE;
}

static int output_udp_disconnect (struct output_handler *this)
{
	Require(
		this != NULL &&
		(
			this->state == os_ready			||
			this->state == os_connecting	||
			this->state == os_error			
		)
	);

	this->state = os_disconnected;
	if (close(this->fd))
	{
		this->fd = -1;
		this->err = errno;
		SysErr(this->err, "While closing outgoing udp connection");
		return FALSE;
	}
	
	this->fd = -1;
	return TRUE;
}

struct output_handler *output_handler_udp_init (char *res)
{
	struct output_handler *this = output_handler_common_init("udp", res, -1);

	/* Allocate private data part */
	struct priv *private = (struct priv*) malloc (sizeof(struct priv));
	SysFatal(private == NULL, errno, "While creating UDP output private data");
	this->priv       = private;

	PRIVATE->proto = net_get_protocol(this->type);
	
	this->connect    = output_udp_connect;
	this->disconnect = output_udp_disconnect;

	return this;
}
