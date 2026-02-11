rm bootsect_test.bin kernel_cpp.bin

fasm bootsect_test.asm bootsect_test.bin

g++ -ffreestanding -m32 -fno-pie -fno-rtti -fno-exceptions -fno-threadsafe-statics -c kernel.cpp -o kernel.o
ld -m elf_i386 -T linker.ld -o kernel_cpp.bin --oformat binary kernel.o

qemu-system-i386 -fda bootsect_test.bin -fdb kernel_cpp.bin
