fasm bootsect.asm bootsect.bin

# Компилируем каждый cpp файл
g++ -ffreestanding -fno-stack-protector -m32 -fno-pie -c test.cpp -o test.o

ld -m elf_i386 -T linker.ld -o test.bin --oformat binary test.o

qemu-system-i386 -fda bootsect.bin -fdb test.bin
