CC = gcc
CFLAGS = -std=c99 -lpthread

SRCDIR = src
OBJDIR = obj

shared = shared
producer = producer
consumer = consumer

all: $(producer) $(consumer)
.PHONY : all

$(producer): $(OBJDIR)/$(producer).o $(OBJDIR)/$(shared).o
	$(CC) $(CFLAGS) $^ -o $@

$(consumer): $(OBJDIR)/$(consumer).o $(OBJDIR)/$(shared).o
	$(CC) $(CFLAGS) $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(SRCDIR)/$(shared).h mkdirs
	$(CC) $(CFLAGS) -c $< -o $@

mkdirs:
	mkdir -p $(OBJDIR)

clean:
	rm -r $(producer) $(consumer) $(OBJDIR)

test: all
	sh test.sh
