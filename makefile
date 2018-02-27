PROGS = showip server client

WARNINGS = -Wall -Wextra -Werror -Wno-error=unused-variable -Wno-error=unused-parameter -Wno-missing-field-initializers -Wno-ignored-qualifiers
CFLAGS = $(WARNINGS) --std=c11
DEPS = $(shell find . -name "*.d")

ifeq ($(mode),release)
	CFLAGS := -O2 -g $(CFLAGS) -DNDEBUG
	LDFLAGS := -O2 -g
else
ifeq ($(mode),debug)

	CFLAGS := -O0  -g $(CFLAGS) -DDEBUG
	LDFLAGS := -O0 -g
else
	mode = debugOpt
	CFLAGS := -g -O2 $(CFLAGS) -DDEBUG
	LDFLAGS := -O2 -g
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
