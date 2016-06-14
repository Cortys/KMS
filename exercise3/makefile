CC = gcc
CFLAGS = -std=c99

SRCDIR = src
OBJDIR = obj

main = aufg5
matrix = Matrix

all: $(main)
.PHONY : all

$(main): $(OBJDIR)/$(main).o $(OBJDIR)/$(matrix).o
	$(CC) $(CFLAGS) $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(SRCDIR)/$(matrix).h mkdirs
	$(CC) $(CFLAGS) -c $< -o $@

mkdirs:
	mkdir -p $(OBJDIR)

clean:
	rm -r $(main) $(OBJDIR)

test: all
	bash test.sh
