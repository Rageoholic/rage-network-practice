PROGS = showip server client listener talker

_RUTILS = network process
RUTILS_DIR= rutils/
RUTILS = $(patsubst %,$(RUTILS_DIR)%.o, $(_RUTILS))

WARNINGS = -Wall -Wextra -Werror -Wno-error=unused-variable -Wno-error=unused-parameter -Wno-missing-field-initializers -Wno-ignored-qualifiers
CFLAGS = $(WARNINGS) --std=c11 -MD -MP
DEPS = $(shell find . -name "*.d")


default: debug-opt

all: $(PROGS)

debug: CFLAGS += -O0 -g
debug:

debug-opt: CFLAGS += -O2 -g -flto
debug-opt: LDFLAGS += -flto -O2 -g
debug-opt: all

release: CFLAGS += -DNDEBUG
release: debug-opt

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

.PHONY: all clean info debug debug-opt release default
