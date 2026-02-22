include config.mk

SRC=cwal.c
OBJ=$(SRC:.c=.o)

all: cwal

.c.o:
	$(CC) $(CFLAGS) -c $<

$(OBJ): config.h config.mk

cwal: $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS)

install: cwal
	mkdir -p $(PREFIX)/bin
	cp -f cwal $(PREFIX)/bin
	chmod 755 $(PREFIX)/bin/cwal

uninstall:
	rm -f $(PREFIX)/bin/cwal

clean:
	rm -f cwal $(OBJ)