IDIR = include
CC = gcc
CFLAGS=-I$(IDIR) -g -Wall

ODIR = .
LDIR = ../lib
BDIR = .

LIBS = -lm

_DEPS = lexer.h def.h main.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o lexer.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

_EXE = scc
EXE = $(BDIR)/$(_EXE)

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(EXE): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(_EXE)
