CC=gcc
CFLAGS=-g -Wall

### For production builds
# CFLAGS=-g -Wall -DNDEBUG

all: genbuf

genbuf: main.o setup.o buffer.o options.o input_file.o input_tcp.o input_tcp_connection.o input_udp.o input_unix.o input_tools.o input_buffer.o output.o output_file.o output_tcp.o output_udp.o output_unix.o output_tools.o net_tools.o buffer.o reader.o logger.o log.o
	${CC} ${CFLAGS} -lpthread -o $@ main.o setup.o options.o input_file.o input_tcp.o input_tcp_connection.o input_udp.o input_unix.o input_tools.o input_buffer.o output.o output_file.o output_tcp.o output_udp.o output_unix.o output_tools.o net_tools.o buffer.o reader.o logger.o log.o

main.o: defines.h types.h options.h input.h output.h setup.h log.h main.c
setup.o: defines.h setup.h types.h input.h output.h input_tcp.h input_udp.h input_unix.h input_file.h output_tcp.h output_udp.h output_unix.h output_file.h log.h setup.c
options.o: defines.h options.h log.h options.c

input_file.o:   defines.h input.h  input_file.h  log.h input_file.c
input_udp.o:    defines.h input.h  input_udp.h   log.h input_udp.c
input_unix.o:   defines.h input.h  input_unix.h  log.h input_unix.c
input_tools.o:  defines.h input.h  input_tools.h input_buffer.h log.h input_tools.c
input_buffer.o: defines.h input_buffer.h log.h input_buffer.c
input_tcp.o:    defines.h input.h  input_tcp.h   log.h input_tcp.c
input_tcp_connection.o:   defines.h input.h  input_tcp_connection.h   log.h input_tcp_connection.c

output.o:		defines.h output.h log.h output.c
output_file.o:  defines.h output.h output_file.h  log.h output_file.c
output_tcp.o:   defines.h output.h output_tcp.h   log.h output_tcp.c
output_udp.o:   defines.h output.h output_udp.h   log.h output_udp.c
output_unix.o:  defines.h output.h output_unix.h  log.h output_unix.c
output_tools.o: defines.h output.h output_tools.h log.h output_tools.c

net_tools.o: defines.h net_tools.h log.h net_tools.c

buffer.o: defines.h buffer.h log.h buffer.c
reader.o: defines.h buffer.h reader.h log.h reader.c
logger.o: defines.h buffer.h logger.h log.h logger.c

log.o: defines.h options.h log.h log.c

clean:
	rm -f genbuf *.o *~
