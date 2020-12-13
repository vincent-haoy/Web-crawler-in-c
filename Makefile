CC=gcc
CFLAGS=-I.
DEPS = ass1make.h
OBJ = main.o ass1fun.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

crawler: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean
clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 
	rm -f crawler