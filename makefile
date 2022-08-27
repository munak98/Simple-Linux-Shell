IDIR =include
CC=gcc
CFLAGS=-I$(IDIR)

ODIR=obj
SDIR = src

_DEPS = parser.h builtins.h executor.h lista_historico.h lista_processos.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o parser.o builtins.o executor.o lista_historico.o lista_processos.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

BRsh: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 