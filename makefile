PROGS = showip server client listener talker

_RUTILS = network process
RUTILS_DIR= rutils/

# TODO: Come up with actual ways to set these properly (shell script?)
PLATFORM = LINUX

ARCHITECTURE = X86_64


WARNINGS = -Wall -Wextra -Werror -Wno-error=unused-variable	\
-Wno-error=unused-parameter -Wno-missing-field-initializers	\
-Wno-unknown-pragmas -Wno-unused-function
BASECFLAGS = $(WARNINGS) --std=c99 -MD -MP -D$(PLATFORM) -D$(ARCHITECTURE)
DEPS = $(shell find . -name "*.d")


default: debug-opt

-include $(DEPS)
-include rutils/rutils.mk

-include .user.mk		#User defined includes. Use this to configure specific settings on build files

all: $(PROGS)

debug: CFLAGS = -O0 -g $(BASECFLAGS)
debug: all

debug-opt: CFLAGS = -O2 -g -flto $(BASECFLAGS)
debug-opt: LDFLAGS += -flto -O2 -g $(BASECFLAGS)
debug-opt: all

release: CFLAGS += -DNDEBUG
release: debug-opt



showip: showip.o rutils.a

server: server.o rutils.a

client: client.o rutils.a

listener: listener.o rutils.a

talker: talker.o rutils.a

tcp-sent: tcp-sent.o rutils.a

clean:
	-$(RM) -f *.o *.d server

.PHONY: all clean info debug debug-opt release default
