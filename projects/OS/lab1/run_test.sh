rm bootsect.bin kernel_cpp.bin

fasm bootsect.asm bootsect.bin

g++ -ffreestanding -fno-stack-protector -m32 -fno-pie -c kernel.cpp -o kernel.o
ld -m elf_i386 -T linker.ld -o kernel_cpp.bin --oformat binary kernel.o

qemu-system-i386 -fda bootsect.bin -fdb kernel_cpp.bin 
