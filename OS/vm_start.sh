#!/bin/bash

# Конфигурационные переменные
VM_NAME="pintos"
SSH_USER="red"
SSH_PORT="2222"
MOUNT_POINT="/mnt/vm_full"

# Получаем UID и GID текущего пользователя
MY_UID=$(id -u)
MY_GID=$(id -g)

# Проверяем и настраиваем права папки
if [ ! -d "$MOUNT_POINT" ]; then
    sudo mkdir -p "$MOUNT_POINT"
    sudo chown $USER:$USER "$MOUNT_POINT"
    sudo chmod 755 "$MOUNT_POINT"
fi

# Запускаем виртуальную машину
echo "Запускаем виртуальную машину '$VM_NAME'..."
VBoxManage startvm "$VM_NAME" --type headless
sleep 10

# Монтируем с правильными правами
echo "Монтируем файловую систему с правами пользователя $USER..."
sshfs -p $SSH_PORT $SSH_USER@localhost:/ "$MOUNT_POINT" \
  -o uid=$MY_UID \
  -o gid=$MY_GID \
  -o allow_other \
  -o reconnect \
  -o ServerAliveInterval=15

# Проверяем права
echo "Права в смонтированной папке:"
ls -la "$MOUNT_POINT" | head -5

# Подключаемся по SSH
echo "Подключаемся к виртуальной машине..."
ssh $SSH_USER@localhost -p $SSH_PORT

# После выхода размонтируем
echo "Размонтируем файловую систему..."
fusermount -u "$MOUNT_POINT"
echo "Готово!"
