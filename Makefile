PLATFORM_SRC = $(wildcard patches/platform/*)
SRC = $(wildcard src/*)
OBJDIR = obj
PLATFORM_OBJS = $(patsubst patches/platform/%,$(OBJDIR)/patches/platform/%,$(PLATFORM_SRC:.c=.o))
OBJS = $(patsubst src/%,$(OBJDIR)/%,$(SRC:.c=.o)) $(PLATFORM_OBJS)
PLOOSHFINDER = plooshfinder/libplooshfinder.a
INCLDIRS = -I./include -I./plooshfinder/include

LDFLAGS ?= -fuse-ld=lld -L./plooshfinder
CC := clang
LIBS = -lplooshfinder
CFLAGS ?= -O2

.PHONY: $(PLOOSHFINDER) all

all: dirs $(PLOOSHFINDER) $(OBJS) Dyld64Patcher

submodules:
	@git submodule update --init --remote --recursive || true

dirs:
	@mkdir -p $(OBJDIR)
	@mkdir -p $(OBJDIR)/patches
	@mkdir -p $(OBJDIR)/patches/platform

clean:
	@rm -rf Dyld64Patcher obj
	@$(MAKE) -C plooshfinder clean

Dyld64Patcher: $(OBJS) $(PLOOSHFINDER)
	$(CC) $(CFLAGS) $(LDFLAGS) $(LIBS) $(INCLDIRS) $(OBJS) -o $@

$(OBJDIR)/%.o: src/%.c
	$(CC) $(CFLAGS) $(INCLDIRS) -c -o $@ $<

$(OBJDIR)/patches/platform/%.o: patches/platform/%.c
	$(CC) $(CFLAGS) $(INCLDIRS) -c -o $@ $<

$(PLOOSHFINDER):
	$(MAKE) -C plooshfinder all