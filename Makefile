INCLDIRS = -I./include -I./plooshfinder/include
SRC = $(wildcard src/*)
TVOS_SRC = src/patches/platform/tvos.c
OBJDIR = obj
OBJS = $(patsubst src/%,$(OBJDIR)/%,$(SRC:.c=.o))
TVOS_OBJS = $(OBJDIR)/patches/platform/tvos.o
PLOOSHFINDER = plooshfinder/libplooshfinder.a

LDFLAGS ?= -fuse-ld=lld -L./plooshfinder
CC := clang
LIBS = -lplooshfinder

.PHONY: $(PLOOSHFINDER) all

all: dirs $(PLOOSHFINDER) $(TVOS_OBJS) $(OBJS) Dyld64Patcher

submodules:
	@git submodule update --init --remote --recursive || true

dirs:
	@mkdir -p $(OBJDIR)
	@mkdir -p $(OBJDIR)/patches
	@mkdir -p $(OBJDIR)/patches/platform

clean:
	@rm -rf Dyld64Patcher obj
	@$(MAKE) -C plooshfinder clean

Dyld64Patcher: $(TVOS_OBJS) $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(LIBS) $(INCLDIRS) $(TVOS_OBJS) $(OBJS) -o $@

$(OBJDIR)/%.o: src/%.c
	$(CC) $(CFLAGS) $(INCLDIRS) -c -o $@ $<

$(OBJDIR)/patches/platform/%.o: patches/platform/%.c
	$(CC) $(CFLAGS) $(INCLDIRS) -c -o $@ $<

$(PLOOSHFINDER):
	$(MAKE) -C plooshfinder all