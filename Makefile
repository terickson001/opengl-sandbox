CC=gcc

IDIR=include
SDIR=src
ODIR=obj
DDIR=deps

CFLAGS=-I$(IDIR) -IGL -Wall -Wextra -Wno-unused-function -Wno-unused-parameter
LDFLAGS=-lglfw -lGLEW -lGLU -lGL -lm

SRCS=$(filter-out ,$(wildcard $(SDIR)/*.c))
OBJS=$(addprefix $(ODIR)/, $(notdir $(SRCS:.c=.o)))
DEPS=$(addprefix $(DDIR)/, $(notdir $(SRCS:.c=.d)))

BINNAME=main


all: release

debug: CFLAGS += -g -O0
debug: $(BINNAME)

release: CFLAGS += -O3
release: $(BINNAME)

$(DDIR)/%.d: $(SDIR)/%.c
	@mkdir -p $(DDIR)
	@set -e; rm -f $@; \
	$(CC) -MM $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,$(ODIR)/\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

include $(DEPS)

$(BINNAME): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

$(ODIR)/%.o: $(SDIR)/%.c
	@mkdir -p $(ODIR)
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean
clean:
	rm $(ODIR)/*.o
	rm $(DDIR)/*.d
	rm $(BINNAME)
