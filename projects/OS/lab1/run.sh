rm bootsect.bin kernel.bin
# fasm bootsect.asm
fasm test.asm
fasm kernel.asm
# qemu-system-i386 -fda bootsect.bin -fdb kernel.bin

cat test.bin kernel.bin > os.bin
qemu-system-i386 -fda os.bin
