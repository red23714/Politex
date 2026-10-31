import os
import pefile

def extract_resources(exe_path, output_dir):
    """
    Извлекает все ресурсы из exe-файла и сохраняет их в указанную директорию.
    """
    try:
        # Загружаем PE-файл
        pe = pefile.PE(exe_path)
    except Exception as e:
        print(f"Ошибка при открытии файла: {e}")
        return

    # Проверяем наличие директории ресурсов
    if not hasattr(pe, 'DIRECTORY_ENTRY_RESOURCE'):
        print("В данном файле нет секции ресурсов (.rsrc).")
        return

    os.makedirs(output_dir, exist_ok=True)
    print(f"Начало извлечения ресурсов в папку: {output_dir}")

    # Обходим дерево ресурсов: Тип -> Имя/ID -> Язык
    for resource_type in pe.DIRECTORY_ENTRY_RESOURCE.entries:
        # Определяем тип ресурса (строка или числовой ID)
        if resource_type.name is not None:
            type_name = str(resource_type.name)
        else:
            # Превращаем стандартные ID в читаемые названия (опционально)
            type_name = str(resource_type.id)

        if hasattr(resource_type, 'directory'):
            for resource_id in resource_type.directory.entries:
                if resource_id.name is not None:
                    res_name = str(resource_id.name)
                else:
                    res_name = str(resource_id.id)

                if hasattr(resource_id, 'directory'):
                    for resource_lang in resource_id.directory.entries:
                        try:
                            # Получаем данные ресурса
                            data_rva = resource_lang.data.struct.OffsetToData
                            size = resource_lang.data.struct.Size
                            
                            # Читаем байты из структуры
                            data = pe.get_memory_mapped_image()[data_rva:data_rva + size]

                            # Формируем имя файла и путь для сохранения
                            lang_id = resource_lang.id
                            filename = f"res_{res_name}_lang_{lang_id}.bin"
                            
                            # Создаем подпапку для каждого типа ресурса
                            type_dir = os.path.join(output_dir, type_name)
                            os.makedirs(type_dir, exist_ok=True)
                            
                            file_path = os.path.join(type_dir, filename)

                            # Записываем данные в файл
                            with open(file_path, 'wb') as f:
                                f.write(data)
                            
                            print(f"[+] Извлечен: {type_name} -> {filename}")
                        except Exception as res_err:
                            print(f"[-] Ошибка при чтении ресурса: {res_err}")

    print("Извлечение завершено.")

if __name__ == "__main__":
    # Укажите путь к вашему EXE и папку назначения
    target_exe = "./malware/fff9abf1ccd7d7821f467af318f3782a7a9fc69dbd15794d52aa2aaa40eac720" 
    target_output = "extracted_resources_2"
    
    if os.path.exists(target_exe):
        extract_resources(target_exe, target_output)
    else:
        print(f"Файл {target_exe} не найден. Укажите корректный путь.")
