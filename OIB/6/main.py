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
    pattern = r'''
        ("(?:\\.|[^"\\])*"|'(?:\\.|[^'\\])*')  # строки в кавычках
        |(//.*?$)                              # однострочные комментарии //
        |(/\*.*?\*/)                           # многострочные комментарии /* */
    '''
    
    def replacer(match):
        if match.group(1): 
            return match.group(1)
        else:
            return ''

    # re.VERBOSE — для читаемости, re.DOTALL — чтобы . включал \n, re.MULTILINE — для ^ и $ построчно
    return re.sub(pattern, replacer, code, flags=re.VERBOSE | re.DOTALL | re.MULTILINE)

def remove_spaces(text):
    exceptions = ["int", "struct", "include", "define", "float",
                  "double", "long", "char"]
    
    words_pattern = r'\b(?:' + '|'.join(map(re.escape, exceptions)) + r')\b'
    
    # Основное регулярное выражение:
    # 1. Строки в кавычках (полностью сохраняем)
    # 2. Директивы препроцессора (сохраняем полностью)
    # 3. Слова из списка (сохраняем с одним пробелом после)
    # 4. Все остальные пробелы (удаляем)
    pattern = rf'''
        (                          # Группа 1: строки в кавычках
            "(?:\\.|[^"\\])*"      # Двойные кавычки
            |'(?:\\.|[^'\\])*'     # Или одинарные
        )
        |(^\s*#.*$)                # Группа 2: директивы препроцессора
        |({words_pattern})(\s+)    # Группа 3: слово из списка, Группа 4: пробелы после
        |(\s+)                     # Группа 5: все остальные пробелы
    '''
    
    def replacer(match):
        if match.group(1):          # Строка в кавычках
            return match.group(1)
        elif match.group(2):        # Директива препроцессора
            return match.group(2)
        elif match.group(3):        # Слово из списка
            return match.group(3) + ' '  # Сохраняем слово + один пробел
        else:                       # Все остальные пробелы
            return ''
    
    # Обрабатываем каждую строку отдельно
    lines = text.split('\n')
    cleaned_lines = []
    
    for line in lines:
        if line.lstrip().startswith('#'):
            # Для директив препроцессора сохраняем строку как есть
            cleaned_lines.append(line)
        else:
            # Обрабатываем остальные строки
            cleaned_line = re.sub(
                pattern,
                replacer,
                line,
                flags=re.VERBOSE | re.DOTALL
            )
            cleaned_lines.append(cleaned_line)
    
    return '\n'.join(cleaned_lines)


def main():
    file = open("test.c", "r").read()
    file = remove_comments(file)
    print(remove_spaces(file))

main()