import argparse
import os
import shutil


def validate_path(path):
    if path is None:
        raise ValueError("Path cannot be None")

    if not isinstance(path, str):
        raise ValueError("Path must be a string")

    cleaned = path.strip()

    if cleaned == "":
        raise ValueError("Path cannot be empty or whitespace")


def ensure_directory_exists(file_path):
    parent = os.path.dirname(file_path)

    if parent != "":
        os.makedirs(parent, exist_ok=True)


def f_create(path):
    try:
        validate_path(path)

        ensure_directory_exists(path)

        file_obj = open(path, "w", encoding="utf-8")
        file_obj.close()

        print(f"[+] File created successfully: {path}")

    except Exception as e:
        print(f"[-] Failed to create file '{path}'. {e}")
        raise


def f_delete(path):
    try:
        validate_path(path)

        exists = os.path.exists(path)

        if exists is False:
            raise FileNotFoundError(f"File '{path}' does not exist")

        directory = os.path.isdir(path)

        if directory:
            raise IsADirectoryError(f"Path '{path}' is a directory")

        os.remove(path)

        print(f"[+] File deleted successfully: {path}")

    except Exception as e:
        print(f"[-] Failed to delete file '{path}'. {e}")
        raise


def f_write(path, content):
    try:
        validate_path(path)

        if content is None:
            raise ValueError("Content cannot be None")

        if os.path.isdir(path) is True:
            raise IsADirectoryError(f"Path '{path}' is a directory")

        ensure_directory_exists(path)

        with open(path, "w", encoding="utf-8") as writer:
            writer.write(content)

        print(f"[+] Content written successfully to: {path}")

    except Exception as e:
        print(f"[-] Failed to write to file '{path}'. {e}")
        raise


def f_read(path):
    try:
        validate_path(path)

        file_exists = os.path.exists(path)

        if not file_exists:
            raise FileNotFoundError(f"File '{path}' does not exist")

        if os.path.isdir(path):
            raise IsADirectoryError(f"Path '{path}' is a directory")

        with open(path, "r", encoding="utf-8") as reader:
            data = reader.read()

        print(f"[+] File read successfully: {path}")

        return data

    except Exception as e:
        print(f"[-] Failed to read file '{path}'. {e}")
        raise


def f_copy(src, dest):
    try:
        validate_path(src)
        validate_path(dest)

        if os.path.exists(src) is False:
            raise FileNotFoundError(f"Source '{src}' does not exist")

        if os.path.isdir(src):
            raise IsADirectoryError(f"Source '{src}' is a directory")

        destination_is_dir = os.path.exists(dest) and os.path.isdir(dest)

        if destination_is_dir:
            raise IsADirectoryError(f"Destination '{dest}' is a directory")

        ensure_directory_exists(dest)

        shutil.copy2(src, dest)

        print(f"[+] File copied successfully from '{src}' to '{dest}'")

    except Exception as e:
        print(f"[-] Failed to copy file from '{src}' to '{dest}'. {e}")
        raise


def f_rename(src, dest):
    try:
        validate_path(src)
        validate_path(dest)

        if not os.path.exists(src):
            raise FileNotFoundError(f"Source '{src}' does not exist")

        if os.path.isdir(src):
            raise IsADirectoryError(f"Source '{src}' is a directory")

        ensure_directory_exists(dest)

        os.rename(src, dest)

        print(f"[+] File renamed successfully from '{src}' to '{dest}'")

    except Exception as e:
        print(f"[-] Failed to rename file from '{src}' to '{dest}'. {e}")
        raise


def execute_command(arguments):
    command_map = {
        "create": lambda: f_create(arguments.path),
        "delete": lambda: f_delete(arguments.path),
        "write": lambda: f_write(arguments.path, arguments.content),
        "read": lambda: f_read(arguments.path),
        "copy": lambda: f_copy(arguments.src, arguments.dest),
        "rename": lambda: f_rename(arguments.src, arguments.dest),
    }

    action = command_map.get(arguments.command)

    if action is None:
        parser.print_help()
        return

    action()


def build_parser():
    parser_instance = argparse.ArgumentParser()

    parser_instance.add_argument("command")
    parser_instance.add_argument("--path")
    parser_instance.add_argument("--src")
    parser_instance.add_argument("--dest")
    parser_instance.add_argument("--content")

    return parser_instance


def main():
    global parser

    parser = build_parser()

    parsed_args = parser.parse_args()

    execute_command(parsed_args)


if __name__ == "__main__":
    main()
