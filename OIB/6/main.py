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
        ("(?:\\.|[^"\\])*")|          # строки в двойных кавычках
        ('(?:\\.|[^'\\])*')|          # строки в одинарных кавычках
        (//[^\n]*|/\*.*?\*/)          # комментарии
    '''
    
    def replacer(match):
        if match.group(1): 
            return match.group(1)
        else:
            return ''

    return re.sub(pattern, replacer, code, flags=re.VERBOSE | re.DOTALL)

def remove_spaces(code):
    c_preproc = [
        # Директивы препроцессора
        '#include', '#define', '#undef', '#if', '#ifdef', '#ifndef',
        '#else', '#elif', '#endif', '#line', '#error', '#pragma',
        '#using', '#import'
    ]

def generate_variable():
    return "asdf"

def replace_variables(code):
    c_keywords = [
        "auto", "break", "case", "char", "const", "continue", "default", "do", 
        "double", "else", "enum", "extern", "float", "for", "goto", "if", 
        "inline", "int", "long", "register", "restrict", "return", "short", 
        "signed", "sizeof", "static", "struct", "switch", "typedef", "union", 
        "unsigned", "void", "volatile", "while"
    ]

    pattern = re.compile(
        r'''
        ("(?:\\.|[^"\\])*")       # строки в кавычках
        | (<[^>]*>)               # содержимое угловых скобок
        | (\b[a-zA-Z_]\w*\()      # слова со скобкой (например "func(")
        | (\b[a-zA-Z_]\w*\b)      # обычные слова
        | (\b\d+\b)               # числа
        ''',
        re.VERBOSE
    )
    
    tokens = []
    for match in pattern.finditer(code):
        # Выбираем первую непустую группу
        token = next((group for group in match.groups() if group), None)
        if token:
            tokens.append(token)

    data_types = ["char", "float", "int", "long", "short", "double"]
    variables = {}
    previous = ""

    def replace_var(code, word):
        new_name = generate_variable()
        pattern = r'\b' + re.escape(word) + r'\b'
        variables.update({word: new_name})
        return re.sub(pattern, new_name, code)

    for word in tokens:
        if word not in data_types:
            if previous == "struct":
                data_types.append(word)
                code = replace_var(code, word)
            elif previous not in data_types:
                data_types.append(word)
            else:
                if word not in variables.keys():
                    code = replace_var(code, word)

        previous = word

    print(code)

def main():
    file = open("test.c", "r").read()
    file = remove_comments(file)
    
    replace_variables(file)

main()