# Компилируем загрузчик
fasm test.asm test.bin

# Компилируем ядро
fasm test_kernel.asm kernel.bin

# Создаем образ дискеты 1.44МБ (2880 секторов по 512 байт)
dd if=/dev/zero of=disk.img bs=512 count=2880

# Копируем загрузочный сектор
dd if=test.bin of=disk.img conv=notrunc

# Копируем ядро начиная со второго сектора
dd if=kernel.bin of=disk.img bs=512 seek=1 conv=notrunc

# Запускаем QEMU
qemu-system-i386 -fda disk.img
