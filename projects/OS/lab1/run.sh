rm bootsect.bin kernel.bin

# Компилируем загрузчик
fasm bootsect.asm bootsect.bin

# Компилируем ядро
fasm kernel.asm kernel.bin

# Запускаем QEMU
qemu-system-i386 -fda bootsect.bin -fdb kernel.bin
