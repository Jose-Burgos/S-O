MP=mp.bin
SOURCES=$(wildcard *.c)

all: $(MP)

$(MP): $(SOURCES)
	gcc $(SOURCES) -fno-pic -o $(MP)

clean:
	rm -rf $(MP)

.PHONY: all clean
