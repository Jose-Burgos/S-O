include ../Makefile.inc

MODULE=0000-uCodeModule.bin
SOURCES=$(wildcard [^_]*.c)
SOURCES_ASM=$(wildcard asm/*.asm)
OBJECTS_ASM=$(SOURCES_ASM:asm/%.asm=object/%.asm.o)
SOURCES_TESTS=$(wildcard tests/*.c)

all: $(MODULE)

$(MODULE): $(SOURCES) $(OBJECTS_ASM)
	$(GCC) $(GCCFLAGS) -I./include -I./tests/include -T uCodeModule.ld _loader.c $(OBJECTS_ASM) $(SOURCES) $(SOURCES_TESTS) -o ../$(MODULE)
	$(GCC) $(GCCFLAGS) -I./include -I./tests/include -T uCodeModule.ld -Wl,--oformat=elf64-x86-64 _loader.c $(OBJECTS_ASM) $(SOURCES) $(SOURCES_TESTS) -o ../0000-sampleCodeModule.elf


object/%.asm.o : asm/%.asm
	mkdir -p object
	$(ASM) $(ASMFLAGS) $< -o $@

clean:
	rm -rf *.o
	rm -rf object

.PHONY: all clean print
