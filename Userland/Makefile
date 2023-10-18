include Makefile.inc

U_DATA=0001-uDataModule.bin

all: uCodeModule uDataModule

uCodeModule:
	cd uCodeModule; make

uDataModule:
	printf "This is u data." >> $(U_DATA) && dd if=/dev/zero bs=1 count=1 >> $(U_DATA)

clean:
	cd uCodeModule; make clean
	rm -rf *.bin


.PHONY: uCodeModule all clean
