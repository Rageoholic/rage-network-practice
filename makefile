CC := clang

PROGS = showip server client

WARNINGS = -Wall -Wextra -pedantic -Werror -Wno-error=unused-variable -Wno-error=unused-parameter -Wno-missing-field-initializers
CFLAGS = $(WARNINGS)
DEPS = $(shell find . -name "*.d")

ifeq ($(mode),release)
	CFLAGS := -O2 -g $(CFLAGS) -DNDEBUG
else
ifeq ($(mode),debug)

	CFLAGS := -O  -g $(CFLAGS) -DDEBUG
else
	mode = debugOpt
	CFLAGS := -g -O2 $(CFLAGS) -DDEBUG
endif
endif


all: $(PROGS)

-include $(DEPS)

showip: showip.o rnetwork.o

server: server.o rnetwork.o rprocess.o

client: client.o rnetwork.o

%.o: %.c
	$(CC) -c $< -o  $@ $(CFLAGS) -MD -MP

clean:
	-$(RM) -f *.o *.d server

.PHONY: all clean
