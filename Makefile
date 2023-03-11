INCLDIRS = -I./include
SRC = $(wildcard src/*)
TVOS_SRC = src/patches/platform/tvos.c
OBJDIR = obj
OBJS = $(patsubst src/%,$(OBJDIR)/%,$(SRC:.c=.o))
TVOS_OBJS = $(OBJDIR)/patches/platform/tvos.o

LDFLAGS ?= -fuse-ld=lld
CC := clang

all: dirs $(TVOS_OBJS) $(OBJS) Dyld64Patcher

dirs:
	@mkdir -p $(OBJDIR)
	@mkdir -p $(OBJDIR)/patches
	@mkdir -p $(OBJDIR)/patches/platform

clean:
	@rm -rf Dyld64Patcher obj

Dyld64Patcher: $(TVOS_OBJS) $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLDIRS) $(TVOS_OBJS) $(OBJS) -o $@

$(OBJDIR)/%.o: src/%.c
	$(CC) $(CFLAGS) $(INCLDIRS) -c -o $@ $<

$(OBJDIR)/patches/platform/%.o: patches/platform/%.c
	$(CC) $(CFLAGS) $(INCLDIRS) -c -o $@ $<