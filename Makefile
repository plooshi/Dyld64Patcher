INCLDIRS = -I./include -I./plooshfinder/include
SRC = $(wildcard src/*) src/patches/platform/ios15.c
OBJDIR = obj
OBJS = $(patsubst src/%,$(OBJDIR)/%,$(SRC:.c=.o))
PLOOSHFINDER = plooshfinder/libplooshfinder.a

LDFLAGS ?= -fuse-ld=lld -L./plooshfinder
CC := clang
LIBS = -lplooshfinder

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

Dyld64Patcher: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(LIBS) $(INCLDIRS) $(OBJS) -o $@

$(OBJDIR)/%.o: src/%.c
	$(CC) $(CFLAGS) $(INCLDIRS) -c -o $@ $<

$(OBJDIR)/patches/platform/%.o: patches/platform/%.c
	$(CC) $(CFLAGS) $(INCLDIRS) -c -o $@ $<

$(PLOOSHFINDER):
	$(MAKE) -C plooshfinder all