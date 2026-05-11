import os
import shutil


def print_err(msg, path, e):
    print(f"[-] {msg}: '{path}'\n{e}")


def f_create(path):
    try:
        if os.path.exists(path):
            raise FileExistsError

        with open(path, "x"):
            pass

        print(f"[+] File created: {path}")

    except FileExistsError as e:
        print_err("File already exists", path, e)

    except PermissionError as e:
        print_err("Permission denied", path, e)

    except IsADirectoryError as e:
        print_err("Path is a directory", path, e)

    except OSError as e:
        print_err("OS error", path, e)


def f_delete(path):
    try:
        if not os.path.isfile(path):
            raise FileNotFoundError

        os.remove(path)

        print(f"[+] File deleted: {path}")

    except FileNotFoundError as e:
        print_err("File not found", path, e)

    except PermissionError as e:
        print_err("Permission denied", path, e)

    except IsADirectoryError as e:
        print_err("Expected file but got directory", path, e)

    except OSError as e:
        print_err("OS error", path, e)


def f_write(path, content):
    try:
        if not os.path.isfile(path):
            raise FileNotFoundError

        with open(path, "w", encoding="utf-8") as f:
            f.write(content)

        print(f"[+] Written to file: {path}")

    except FileNotFoundError as e:
        print_err("File not found", path, e)

    except PermissionError as e:
        print_err("Permission denied", path, e)

    except IsADirectoryError as e:
        print_err("Path is a directory", path, e)

    except TypeError as e:
        print_err("Content must be string", path, e)

    except OSError as e:
        print_err("OS error", path, e)


def f_read(path):
    try:
        if not os.path.isfile(path):
            raise FileNotFoundError

        with open(path, "r", encoding="utf-8") as f:
            data = f.read()

        return data

    except FileNotFoundError as e:
        print_err("File not found", path, e)

    except PermissionError as e:
        print_err("Permission denied", path, e)

    except IsADirectoryError as e:
        print_err("Path is a directory", path, e)

    except UnicodeDecodeError as e:
        print_err("Encoding error", path, e)

    except OSError as e:
        print_err("OS error", path, e)


def f_copy(src, dest):
    try:
        if not os.path.isfile(src):
            raise FileNotFoundError

        shutil.copy(src, dest)

        print(f"[+] File copied: {src} -> {dest}")

    except FileNotFoundError as e:
        print_err("Source file not found", src, e)

    except PermissionError as e:
        print_err("Permission denied", f"{src} or {dest}", e)

    except shutil.SameFileError as e:
        print_err("Source and destination are the same", src, e)

    except IsADirectoryError as e:
        print_err("Directory error", f"{src} or {dest}", e)

    except OSError as e:
        print_err("OS error", f"{src} or {dest}", e)


def f_rename(src, dest):
    try:
        if not os.path.isfile(src):
            raise FileNotFoundError

        if os.path.exists(dest):
            raise FileExistsError

        os.rename(src, dest)

        print(f"[+] File renamed: {src} -> {dest}")

    except FileNotFoundError as e:
        print_err("Source file not found", src, e)

    except FileExistsError as e:
        print_err("Destination already exists", dest, e)

    except PermissionError as e:
        print_err("Permission denied", f"{src} or {dest}", e)

    except OSError as e:
        print_err("OS error", f"{src} or {dest}", e)
