
all:  bootloader kernel userland image

buddy:  bootloader kernelb userland imageb

bootloader:
	cd Bootloader; make all

kernel:
	cd Kernel; make all

kernelb:
	cd Kernel; make buddy

userland:
	cd Userland; make all

image: kernel bootloader userland
	cd Image; make all

imageb: kernelb bootloader userland
	cd Image; make all

clean:
	cd Bootloader; make clean
	cd Image; make clean
	cd Kernel; make clean
	cd Userland; make clean

.PHONY: bootloader image collections kernel userland all clean buddy 
