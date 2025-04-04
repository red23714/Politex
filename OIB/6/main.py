import configparser
import re
import random



def load_config(config_file):
    config = configparser.ConfigParser()
    config.read(config_file)
    return {
        'remove_comments': config.getboolean('DEFAULT', 'remove_comments', fallback=True),
        'remove_whitespace': config.getboolean('DEFAULT', 'remove_whitespace', fallback=True),
        'obfuscate_names': config.getboolean('DEFAULT', 'obfuscate_names', fallback=True),
        'shuffle_functions': config.getboolean('DEFAULT', 'shuffle_functions', fallback=True),
        'name_prefix': config.get('DEFAULT', 'name_prefix', fallback='x_'),
        'add_garbage': config.getboolean('DEFAULT', 'add_garbage', fallback=False)
    }

def remove_comments(code):
    pattern = r'/\*.*?\*/|//.*?$'
    
    def replacer(match):
        if match.group(1): 
            return match.group(1)
        else:
            return ''

    # re.VERBOSE — для читаемости, re.DOTALL — чтобы . включал \n, re.MULTILINE — для ^ и $ построчно
    return re.sub(pattern, replacer, code, flags=re.VERBOSE | re.DOTALL | re.MULTILINE)

def remove_spaces(code):
    code = re.sub(r'^[ \t]+|[ \t]+$', '', code, flags=re.MULTILINE)
    
    # Заменяем множественные пробелы и табы между токенами на один пробел
    code = re.sub(r'([^\w\s])\s+([^\w\s])', r'\1\2', code)  # между спецсимволами
    code = re.sub(r'(\w)\s+([^\w\s])', r'\1\2', code)       # между словом и спецсимволом
    code = re.sub(r'([^\w\s])\s+(\w)', r'\1\2', code)       # между спецсимволом и словом
    code = re.sub(r'(\w)\s+(\w)', r'\1 \2', code)           # между словами (оставляем 1 пробел)
    
    # Удаляем пробелы вокруг операторов, но оставляем после ключевых слов
    code = re.sub(r'\s*([=+\-*/%&|^<>!]=?|&&|\|\|)\s*', r'\1', code)
    
    # Удаляем пробелы после открывающих и перед закрывающими скобками
    code = re.sub(r'\(\s+', '(', code)
    code = re.sub(r'\s+\)', ')', code)
    
    # Удаляем пробелы после открывающих и перед закрывающими фигурными скобками
    code = re.sub(r'\{\s+', '{', code)
    code = re.sub(r'\s+\}', '}', code)
    
    # Удаляем пробелы перед точкой с запятой
    code = re.sub(r'\s+;', ';', code)
    
    # Удаляем пустые строки
    code = re.sub(r'\n\s*\n', '\n', code)
    
    return code


def main():
    file = open("test.c", "r").read()
    file = remove_comments(file)
    print(file)
    print(remove_spaces(file))

main()