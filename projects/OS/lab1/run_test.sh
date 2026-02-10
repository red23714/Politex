rm test.bin kernel.bin

fasm test.asm test.bin
fasm test_kernel.asm kernel.bin

qemu-system-i386 -fda test.bin -fdb kernel.bin
