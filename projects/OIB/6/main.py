import configparser
import re
import random
import string

c_words = ["struct", "typedef", "unsigned", "signed", "enum", "union", "const", "return"]
data_types = ["char", "float", "int", "long", "short", "double", "size_t", "void", "define"]

def load_config(config_file):
    config = configparser.ConfigParser()
    config.read(config_file)
    return {
        'input_file': config.get('DEFAULT', 'input_file', fallback='test.c'),
        'output_file': config.get('DEFAULT', 'output_file', fallback='compressed.c'),
        'remove_comments': config.getboolean('DEFAULT', 'remove_comments', fallback=True),
        'remove_spaces': config.getboolean('DEFAULT', 'remove_spaces', fallback=True),
        'replace_variables': config.getboolean('DEFAULT', 'replace_variables', fallback=True),
        'shuffle_functions': config.getboolean('DEFAULT', 'shuffle_functions', fallback=True),
        'name_prefix': config.get('DEFAULT', 'name_prefix', fallback='x_'),
        'add_garbage': config.getboolean('DEFAULT', 'add_garbage', fallback=False),
        'garbage_functions': config.getint('DEFAULT', 'garbage_functions', fallback=5),
        'garbage_variables': config.getint('DEFAULT', 'garbage_variables', fallback=5),
        'garbage_loops': config.getint('DEFAULT', 'garbage_loops', fallback=5),
        'garbage_iterations': config.getint('DEFAULT', 'garbage_iterations', fallback=10)
    }

def remove_comments(code):
    pattern = r'''
        ("(?:\\.|[^"\\])*")|          # строки в двойных кавычках
        ('(?:\\.|[^'\\])*')|          # строки в одинарных кавычках
        (//[^\n]*|/\*.*?\*/)          # комментарии
    '''
    
    def replacer(match):
        if match.group(1): return match.group(1)
        if match.group(2): return match.group(2)
        return ''

    return re.sub(pattern, replacer, code, flags=re.VERBOSE | re.DOTALL)

def remove_spaces(code):
    lines = code.split('\n')
    result = []
    for line in lines:
        line = line.strip()
        if line.startswith('#'):
            result.append(line + '\n')
        else:
            if line:
                result.append(line.rstrip(' \t') + ' ')
    
    code = ''.join(result)

    all_keywords = [re.escape(word) for word in c_words + data_types]
    keyword_pattern = r'(?:\b(?:' + '|'.join(all_keywords) + r')\b\s*)+'
    
    # Корректно экранированные шаблоны
    pattern = fr'''
        (".*?"|'.*?')               # 1. Строки в кавычках
        |                           # ИЛИ
        (//.*?$|/\*.*?\*/)          # 2. Комментарии
        |                           # ИЛИ
        ({keyword_pattern}          # 3. Последовательность ключевых слов
            (?:\*\s*)*              # возможные указатели
            \w+                     # идентификатор
        )
        |                           # ИЛИ
        ([ ]+)                      # 4. Обычные пробелы
    '''
    
    def replacer(m):
        if m.group(1): return m.group(1)  # Сохраняем строки
        if m.group(2): return m.group(2)  # Сохраняем комментарии
        if m.group(3): return m.group(3)  # Сохраняем ключевые слова с пробелами
        return ' ' if m.group(4) else ''  # Сжимаем пробелы до одного
    
    # Применяем замену
    code = re.sub(pattern, replacer, code, flags=re.VERBOSE | re.DOTALL | re.MULTILINE)
    
    # Дополнительная очистка пробелов вокруг операторов (с корректным экранированием)
    operator_pattern = r'''
        [ ]*                       # Любые пробелы
        (
            [=+\-/*%&|^<>!]*=     # Операторы присваивания
            |\+\+ |--             # Инкремент/декремент
            |&& |\|\|             # Логические операторы
            |-> |\.               # Операторы доступа
            |[,;:()\[\]{}]       # Пунктуация
        )
        [ ]*                       # Любые пробелы
    '''
    code = re.sub(operator_pattern, r'\1', code, flags=re.VERBOSE)
    
    return code

def garbage_var(config):
    var_type = random.choice(["int", "double", "float", "char"])
    var_name = generate_variable(config)
    var_value = random.choice([
        f'\'{random.choice(string.ascii_lowercase)}\'',
        f'{random.randint(0, 100)}',
        f'{random.randint(100, 200) / 100 }'
    ])

    return f"\n{var_type} {var_name} = {var_value};"

def garbage_loop(config):
    var_name = generate_variable(config)
    iterations = config["garbage_iterations"]
    loop_type = random.choice(['for', 'while', 'do'])

    if loop_type == 'for':
        return f"\nfor(int {var_name} = 0; {var_name} < {iterations}; {var_name}++) {{}}\n"
    elif loop_type == 'while':
        return f"\nint {var_name} = 0; \nwhile({var_name} < {iterations}){{ {var_name}++; }}\n"
    else:
        return f"\nint {var_name} = 0; \ndo{{{var_name}++;}}while({var_name} < {iterations});\n"
    
def garbage_func(config):
    func_name = generate_variable(config)
    func_type = random.choice(["int", "double", "void"])
    var_count = config["garbage_variables"]
    loop_count = config["garbage_loops"]

    func_vars = ""
    for _ in range(0, var_count):
        func_vars += garbage_var(config)
    
    func_loops = ""
    for _ in range(0, loop_count):
        func_loops += garbage_loop(config)

    func_return = ""
    if func_type != "void":
        func_return = f"return {random.randint(1, 100)};"

    return f"\n{func_type} {func_name} () \n{{ {func_vars} {func_loops} {func_return} }}\n"


def add_garbage(code, config):
    func = find_function(code)
    parts = []

    parts.append(code[:func[0][1]])

    for _ in range(0, config["garbage_functions"]):
        parts.append(garbage_func(config))

    for i in func:
        parts.append(code[i[1]:i[2]])

    new_code = ""
    for part in parts:
        new_code += part

    return new_code

def find_function(code):
    functions = []
    stack = 0
    func_name = None
    func_start = None
    i = 0
    n = len(code)
    
    while i < n:
        # Ищем начало объявления функции
        if stack == 0:
            match = re.match(
                r'(?:[\w\s*]+)\s+'      # возвращаемый тип
                r'(\w+)\s*'             # имя функции (группа 1)
                r'\([^)]*\)\s*'         # параметры
                r'(?:{[^{}]*})?',       # возможное тело для однострочных функций
                code[i:i+100]           # ограничиваем длину проверки
            )
            if match:
                func_name = match.group(1)
                func_start = i
                while i < n and code[i] != '{':
                    i += 1
                if i >= n:
                    break
                stack = 1
                i += 1
                continue
        
        if stack > 0:
            if code[i] == '{':
                stack += 1
            elif code[i] == '}':
                stack -= 1
                if stack == 0 and func_name:
                    functions.append((func_name, func_start, i + 1))
                    func_name = None
                    func_start = None
        
        i += 1
    
    return functions


def shuffle_functions(code):
    funcs = find_function(code)

    parts = []

    prev = 0
    for func in funcs:
        if prev != func[1]: parts.append([False, code[prev:func[1]]])
        parts.append([True, code[func[1]:func[2]]])
        prev = func[2]
    
    if prev < len(code):
        parts.append([False, code[prev:]])

    to_shuffle = [part for part in parts if part[0]]
    random.shuffle(to_shuffle)

    result = []
    shuffle_idx = 0
    structs = 1
    for part in parts:
        if part[0]:
            result.append(to_shuffle[shuffle_idx][1])
            shuffle_idx += 1
        else:
            result.append(part[1])
            if "struct" in part[1] and len(result) > 1:
                temp = result[structs]
                result[structs] = part[1]
                result[len(result) - 1] = temp
                structs += 1

    
    return ''.join(result)

def generate_variable(config):
    var = config["name_prefix"] + ''.join(random.choice(string.ascii_letters) for _ in range(10))
    return var

def replace_variables(code, config):

    code = remove_comments(code)

    replacements = {}
    
    # Запомнить все строковые литералы и заменить их временными метками
    string_literals = []
    def replace_string(match):
        string_literals.append(match.group(0))
        return f'__STRING_LITERAL_{len(string_literals)-1}__'
    
    code_without_strings = re.sub(r'"(?:\\.|[^"\\])*"', replace_string, code)
    
    type_pattern = r'\b(?:unsigned|const|static|extern|auto)\b'
    ptr_pattern = r'\*+\s*'
    identifier_pattern = r'[a-zA-Z_][a-zA-Z0-9_]*'
    
    # Шаблон для структур
    struct_pattern = re.compile(
        rf'\bstruct\s+({identifier_pattern})\s*'
        rf'(?:\{{\s*[^}}]*\}})?'
        rf'\s*;?'
    )
    
    # Шаблон для объявлений переменных и функций
    declaration_pattern = re.compile(
        rf'(?:{type_pattern}\s+)*'
        rf'(?:{type_pattern}\s+)*'
        rf'(?:{identifier_pattern}\s+)+'
        rf'(?:{ptr_pattern})?'
        rf'({identifier_pattern})'
        rf'(?:\s*\[[^\]]*\])?'
        rf'(?:\s*=\s*[^;,]*)?'
        rf'(?:\s*\([^)]*\))?'
    )
    
    # Обрабатываем структуры
    structs = []
    for match in struct_pattern.finditer(code_without_strings):
        struct_name = match.group(1)
        if struct_name not in replacements:
            replacements[struct_name] = generate_variable(config)
            structs.append(struct_name)
            data_types.append(struct_name)
    
    # Обрабатываем другие объявления
    for match in declaration_pattern.finditer(code_without_strings):
        original_name = match.group(1)
        
        if (original_name not in replacements and original_name != "main" and
            not re.match(r'\b(?:int|char|float|double|void|short|long|signed|unsigned|const|static|extern|auto|struct|union|enum|typedef)\b', original_name)):
            replacements[original_name] = generate_variable(config)
    
    for original, new in replacements.items():
        code_without_strings = re.sub(rf'\b{original}\b', new, code_without_strings)
    
    def restore_strings(match):
        index = int(match.group(1))
        return string_literals[index]
    
    final_code = re.sub(r'__STRING_LITERAL_(\d+)__', restore_strings, code_without_strings)
    
    return final_code

def main():
    config = load_config("obfuscator.cfg")
    file = open(config["input_file"], "r")
    code = file.read()
    if config["remove_comments"]: code = remove_comments(code)
    if config["replace_variables"]: code = replace_variables(code, config)
    if config["shuffle_functions"]: code = shuffle_functions(code)
    if config["add_garbage"]: code = add_garbage(code, config)
    if config["remove_spaces"]: code = remove_spaces(code)

    new_file = open(config["output_file"], "w")
    new_file.write(code)

main()