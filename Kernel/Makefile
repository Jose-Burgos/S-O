include Makefile.inc

KERNEL=kernel.bin
SOURCES=$(wildcard [^_]*.c)
SOURCES_ASM=$(wildcard asm/*.asm)
OBJECTS=$(SOURCES:%.c=object/%.o)
OBJECTS_ASM=$(SOURCES_ASM:asm/%.asm=object/%.asm.o)
LOADERSRC=loader.asm

LOADEROBJECT=$(LOADERSRC:%.asm=object/%.asm.o)
STATICLIBS=

all: $(KERNEL)
$(KERNEL): $(LOADEROBJECT) $(OBJECTS) $(STATICLIBS) $(OBJECTS_ASM)
	$(LD) $(LDFLAGS) -T kernel.ld -o $(KERNEL) $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_ASM) $(STATICLIBS)

object/%.o: %.c
	$(GCC) $(GCCFLAGS) -I./include -c $< -o $@

object/%.asm.o : asm/%.asm
	$(ASM) $(ASMFLAGS) $< -o $@

$(LOADEROBJECT):
	mkdir object
	$(ASM) $(ASMFLAGS) $(LOADERSRC) -o $(LOADEROBJECT)

clean:
	rm -rf object

.PHONY: all clean
