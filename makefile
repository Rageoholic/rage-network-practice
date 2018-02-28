PROGS = showip server client listener talker

_RUTILS = network process
RUTILS_DIR= rutils/
RUTILS = $(patsubst %,$(RUTILS_DIR)%.o, $(_RUTILS))

WARNINGS = -Wall -Wextra -Werror -Wno-error=unused-variable -Wno-error=unused-parameter -Wno-missing-field-initializers -Wno-ignored-qualifiers
CFLAGS = $(WARNINGS) --std=c11 -MD -MP
DEPS = $(shell find . -name "*.d")

ifeq ($(mode),release)
	CFLAGS := -O2 -g $(CFLAGS) -DNDEBUG -flto
	LDFLAGS := -O2 -g -flto
else
ifeq ($(mode),debug)

	CFLAGS := -O0  -g $(CFLAGS) -DDEBUG
	LDFLAGS := -O0 -g
else
	mode = debugOpt
	CFLAGS := -g -O2 $(CFLAGS) -DDEBUG -flto
	LDFLAGS := -O2 -g -flto
endif
endif

all: $(PROGS)

-include $(DEPS)

rutils.a: $(RUTILS)
	$(AR) rcs $@ $^
	ranlib $@

showip: showip.o rutils.a

server: server.o rutils.a

client: client.o rutils.a

listener: listener.o rutils.a

talker: talker.o rutils.a

%.o: %.c
	$(CC) -c $< -o  $@ $(CFLAGS) -MD -MP

clean:
	-$(RM) -f *.o *.d server

info:
	@echo building $(PROGS) $(RUTILS)

.PHONY: all clean info
