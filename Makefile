CC=gcc

IDIR=include
SDIR=src
ODIR=obj

CFLAGS=-I$(IDIR) -IGL -Wall -Wextra -Wno-unused-function -Wno-unused-parameter
LDFLAGS=-lglfw -lGLEW -lGLU -lGL -lm

SRCS=$(filter-out ,$(wildcard $(SDIR)/*.c))
OBJS=$(addprefix $(ODIR)/, $(notdir $(SRCS:.c=.o)))
DEPS=$(wildcard $(IDIR)/*.h) $(wildcard $(LDIR)/*/*.h)

BINNAME=main

all: release

debug: CFLAGS += -g -O0
debug: $(BINNAME)

release: CFLAGS += -O3
release: $(BINNAME)

$(BINNAME): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	@mkdir -p $(ODIR)
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean
clean:
	rm $(ODIR)/*.o
	rm $(BINNAME)
