CC=gcc

IDIR=include
SDIR=src
ODIR=obj

CFLAGS=-I$(IDIR) -IGL -Wall -Wextra -O0 -g -Wno-unused-function -Wno-unused-parameter
LDFLAGS=-lglfw -lGLEW -lGLU -lGL -lm

# _DEPS=glmath2.h lib.h shaders.h util.h image.h window.h camera.h model.h text.h texture.h entity.h vao.h
# DEPS=$(patsubst %,$(IDIR)/%,$(_DEPS))

# _SRCS=main.c shaders.c util.c glmath2.c image.c window.c camera.c model.c text.c texture.c entity.c vao.c
# SRCS=$(patsubst %,$(SDIR)/%,$(_SRCS))

# _OBJS=$(_SRCS:.c=.o)
# OBJS=$(patsubst %,$(ODIR)/%,$(_OBJS))

SRCS=$(filter-out ,$(wildcard $(SDIR)/*.c))
OBJS=$(addprefix $(ODIR)/, $(notdir $(SRCS:.c=.o)))
DEPS=$(wildcard $(IDIR)/*.h) $(wildcard $(LDIR)/*/*.h)

BINNAME=main

all: $(BINNAME)

$(BINNAME): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	@mkdir -p $(ODIR)
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean
clean:
	rm $(ODIR)/*.o
	rm $(BINNAME)
