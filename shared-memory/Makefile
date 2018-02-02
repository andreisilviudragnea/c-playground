CFLAGS = -O3 -flto -Wall -Werror -Wextra -Wstrict-prototypes \
	-Wmissing-prototypes -Wmissing-declarations

all: server client

test_writer: test_writer.c
	$(CC) $(CFLAGS) $^ -o $@

test_reader: test_reader.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	$(RM) server client

.PHONY: all clean