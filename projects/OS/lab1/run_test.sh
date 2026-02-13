# rm bootsect.bin kernel_cpp.bin
#
# fasm bootsect.asm bootsect.bin
#
# g++ -ffreestanding -fno-stack-protector -m32 -fno-pie -c kernel.cpp -o kernel.o
# ld -m elf_i386 -T linker.ld -o kernel_cpp.bin --oformat binary kernel.o
#
# qemu-system-i386 -fda bootsect.bin -fdb kernel_cpp.bin 

rm -f *.o bootsect.bin kernel_cpp.bin

fasm bootsect.asm bootsect.bin

# Компилируем каждый cpp файл
g++ -ffreestanding -fno-stack-protector -m32 -fno-pie -c main.cpp -o main.o
g++ -ffreestanding -fno-stack-protector -m32 -fno-pie -c screen.cpp -o screen.o
g++ -ffreestanding -fno-stack-protector -m32 -fno-pie -c keyboard.cpp -o keyboard.o
g++ -ffreestanding -fno-stack-protector -m32 -fno-pie -c commands.cpp -o commands.o
g++ -ffreestanding -fno-stack-protector -m32 -fno-pie -c strings.cpp -o strings.o

# Линкуем все объектные файлы вместе
ld -m elf_i386 -T linker.ld -o kernel_cpp.bin --oformat binary main.o screen.o keyboard.o commands.o strings.o

qemu-system-i386 -fda bootsect.bin -fdb kernel_cpp.bin
