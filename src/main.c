#include "defines.h"

#include <pthread.h>
#include <signal.h>
#include <getopt.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "log.h"
#include "types.h"
#include "input.h"
#include "output.h"
#include "setup.h"
#include "options.h"

#include "buffer.h"
#include "reader.h"
#include "logger.h"

/* pthread identifier variables are global for the signal handler to be work */
static pthread_t logthread;
static pthread_t readthread;

static void signal_handler (int signal)
{
	/* Determine action depending on signal */
	switch (signal)
	{
		/* Nice shutdown requested */
		case SIGHUP:
		case SIGTERM:
			fprintf(stderr, "I got shutdown signal: %d\n", signal);
			pthread_cancel(readthread);
			break;

		/* Ignore other signals */
		default:
			fprintf(stderr, "I ignored signal: %d\n", signal);
			return;
	}
}

static void run (struct reader *rd, struct logger *ld)
{
	/* Ignore signals for rest of threads */
	signal(SIGHUP, SIG_IGN);
	signal(SIGTERM, SIG_IGN);
	signal(SIGPIPE, SIG_IGN);

	Log(error, "Starting threads!\n");

	/* Create the worker threads */
	SysFatal(pthread_create(&logthread,  NULL, (void*) ld->run, ld), errno, "On logger thread start");
	SysFatal(pthread_create(&readthread, NULL, (void*) rd->run, rd), errno, "On reader thread start");
	
	/* Register signal handlers for nice shutdown */
	signal(SIGHUP, signal_handler);
	signal(SIGTERM, signal_handler);
	signal(SIGPIPE, signal_handler);

	Log(error, "Waiting for readthread to terminate!\n");

	/* Wait for the reader to finish and cleanup */
	SysFatal(pthread_join(readthread, NULL), errno, "While waiting for reader thread to finish");
	rd->cleanup(rd);

	Log(error, "Waiting for logthread to terminate!\n");

	/* Wait for the logger to finish and cleanup */
	SysFatal(pthread_join(logthread, NULL), errno, "While waiting for logger thread to finish");
	ld->cleanup(ld);

	Log(error, "All threads terminated!\n");
}

static int parse_type (char *type)
{
	if (strcasecmp(type, "file") == 0)
		return type_file;
	else if (strcasecmp(type, "udp") == 0)
		return type_udp;
	else if (strcasecmp(type, "tcp") == 0)
		return type_tcp;
	else if (strcasecmp(type, "unix") == 0)
		return type_unix;
	else
		return type_unknown;
}

int main (int argc, char **argv)
{
	struct buffer *buffer;
	struct reader *rd;
	struct logger *ld;
	struct output_handler *outhandler;
	struct input_handler *inhandler;
	enum io_types out_type = type_file;
	enum io_types in_type = type_file;
	char *out_res = NULL;
	char *in_res = NULL;
	int c;

	static struct option long_options[] =
	{
		{"verbose",     no_argument,       NULL, 'v'},
		{"help",        no_argument,       NULL, 'h'},
		{"in",          required_argument, NULL, 'i'},
		{"out",         required_argument, NULL, 'o'},
		{"src",         required_argument, NULL, 's'},
		{"source",      required_argument, NULL, 's'},
		{"dst",         required_argument, NULL, 'd'},
		{"dest",        required_argument, NULL, 'd'},
		{"destination", required_argument, NULL, 'd'},
		{"backlog",     required_argument, NULL, 'b'},
		{ NULL,         0,                 NULL,  0 }
	};
	int option_index = 0;

	Log(critical, "Genbuf starting!\n");

	buffer = buffer_init();
	rd     = reader_init(buffer);
	ld     = logger_init(buffer);
	current_log_level = impossible;

	/* While we're busy */
	while(TRUE)
	{
		/* Get option */
		c = getopt_long (argc, argv, "vhi:o:s:d:b:", long_options, &option_index);

		/* Detect the end of the options is reached */
		if (c == -1)
			break;

		/* Sneaky redefine of long options to their short counterparts */
		if (c == 0)
			c = long_options[option_index].val;
	
		/* Determine which option the user supplied */
		switch(c)
		{
			case 'v':
				/* Increase verbose level */
				if (current_log_level < debug)
					current_log_level++;
				break;

			case 'i':
				/* Set the input type for the next source */
				in_type = parse_type(optarg);
				if (in_type == type_unknown)
				{
					fprintf(stderr, "Unknown in-type: %s\n", optarg);
					return EXIT_FAILURE;
				}
				break;

			case 'o':
				/* Set the output type for the next destination */
				out_type = parse_type(optarg);
				if (out_type == type_unknown)
				{
					fprintf(stderr, "Unknown out-type: %s\n", optarg);
					return EXIT_FAILURE;
				}
				break;

			case 's':
				/* Add a source */
				in_res = strdup(optarg);
				if (in_res == NULL)
				{
					perror("String duplication failed");
					return EXIT_FAILURE;
				}

				inhandler = create_input_handler(in_type, in_res);

				/* Register with reader */
				rd->add_source(rd, inhandler);
				break;

			case 'd':
				/* Set the destination */
				if (out_type == type_unknown)
				{
					fprintf(stderr, "No output type specified for destination!\n");
					return EXIT_FAILURE;
				}
				
				if (out_res != NULL)
				{
					fprintf(stderr, "Destination already set!\n");
					return EXIT_FAILURE;
				}
				
				out_res = strdup(optarg);
				if (out_res == NULL)
				{
					perror("String duplication failed");
					return EXIT_FAILURE;
				}
				
				outhandler = create_output_handler(out_type, out_res);

				/* Register with logger */
				ld->set_destination(ld, outhandler);
				break;

			case 'b':
				/* Set backlog file */
				ld->backlog_file = strdup(optarg);
				if (ld->backlog_file == NULL)
				{
					perror("String duplication failed");
					return EXIT_FAILURE;
				}
				break;

			case 'h':
				/* Print a helpfull message */
				fprintf(
					stderr,
						"%s:\n"
						"\t-h/--help\n"
						"\t-v/--verbose\n"
						"\t-b/--backlog filename\n"
						"\t[-i/--in  <type> -s/--s(ou)rc(e)      <res>]+\n"
						"\t[-o/--out <type> -d/--d(e)st(ination) <res>]\n"
						"\n"
						"\t<type>=file/udp/tcp/unix\n"
						"\t<res>=filename/host:port\n"
						"For more documentation check the readme",
					argv[0]);
				return EXIT_FAILURE;
				
			case '?':
				/* getopt_long already printed an error message. */
				return EXIT_FAILURE;
				
			default:
				/* Print error */
				fprintf(stderr, "Error(%c)!\n", c);
				return EXIT_FAILURE;
		}
	}

	/* Run main program loop */
	run(rd, ld);

	/* Do some cleanups */
	free(in_res);
	free(out_res);

	/* Cleanup some last things */
	buffer_cleanup(buffer);

	/* All went well, exit */
	Log(critical, "Program finished succesfully..");
	return EXIT_SUCCESS;
}
