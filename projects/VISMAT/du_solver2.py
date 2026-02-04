from playwright.sync_api import sync_playwright
import matplotlib.pyplot as plt
import io
from PIL import Image
import numpy as np

def safe_render_latex(latex_string, fontsize=20, dpi=100):
    try:
        fig = plt.figure(figsize=(0.01, 0.01), dpi=dpi)
        plt.text(0.5, 0.5, f'${latex_string}$', fontsize=fontsize, 
                ha='center', va='center')
        plt.axis('off')
        
        buf = io.BytesIO()
        plt.savefig(buf, format='png', bbox_inches='tight', 
                   pad_inches=0.1, dpi=dpi)
        buf.seek(0)
        
        img = Image.open(buf)
        
        plt.close(fig)
        return img
        
    except Exception:
        plt.close('all')
        return None

def combine_images_vertically(images, spacing=20, background='white'):
    if not images:
        return None
    
    total_height = sum(img.height for img in images) + spacing * (len(images) - 1)
    max_width = max(img.width for img in images) if images else 0
    
    combined = Image.new('RGB', (max_width, total_height), background)
    
    y_offset = 0
    for img in images:
        x_offset = (max_width - img.width) // 2
        combined.paste(img, (x_offset, y_offset))
        y_offset += img.height + spacing
    
    return combined

def render_all_steps(latex_strings, fontsize=20, output_filename=None, dpi=100):
    successful_images = []
    
    for _, latex_str in enumerate(latex_strings):        
        img = safe_render_latex(latex_str, fontsize, dpi)
        if img is not None:
            successful_images.append(img)
    
    combined_image = combine_images_vertically(successful_images)
    
    if output_filename and combined_image:
        combined_image.save(output_filename)
        print(f"📁 Изображение сохранено как: {output_filename}")
    
    return combined_image

def extract_equations_from_steps(steps):
    equations = []
    
    for i, step in enumerate(steps):
        ktx_elements = step.query_selector_all(".ktx")
        latex_strings = []
        
        for elem in ktx_elements:
            tex = elem.get_attribute("data-tex")
            if tex:
                latex_strings.append(tex)
        
        if latex_strings:
            if i == 0: 
                selected_equation = latex_strings[0]
            else: 
                selected_equation = latex_strings[-1]
            
            equations.append(selected_equation)
    
    return equations

# файл: input_math.py
from playwright.sync_api import sync_playwright
import matplotlib.pyplot as plt

def render_latex(latex_string, fontsize=20, filename=None):
    """
    Рендерит LaTeX строку и показывает её в окне.
    
    Параметры:
    - latex_string: str, LaTeX код (например, r"$\frac{a}{b}$")
    - fontsize: int, размер шрифта
    - filename: str, если указано, сохранит изображение в файл
    """
    plt.figure(figsize=(0.01, 0.01))  # размер фигуры минимальный, чтобы подогнать под текст
    plt.text(0.5, 0.5, latex_string, fontsize=fontsize, ha='center', va='center')
    plt.axis('off')  # убираем оси
    
    if filename:
        plt.savefig(filename, bbox_inches='tight', pad_inches=0.1)
    
    plt.show()

input_text = input()

with sync_playwright() as p:
    browser = p.chromium.launch(headless=True)
    page = browser.new_page()

    print("Открываю сайт mathdf.com...")
    page.goto("https://mathdf.com/dif/ru")

    # Вводим уравнение
    page.wait_for_selector("#input-expression")
    page.fill("#input-expression", input_text)
    print(f'Введено выражение: {input_text}')

    # Нажимаем "Решить"
    page.click("#solve")
    print("Нажата кнопка 'Решить'.")

    # Ждём появления блока с решением
    print("Ожидание загрузки блока #math-canvas...")
    page.wait_for_selector("#math-canvas", timeout=20000)

    # Получаем все элементы с классом .step
    steps = page.query_selector_all(".step")

    if len(steps) < 2:
        print("⚠️ Недостаточно шагов для извлечения предпоследнего.")
        browser.close()

    # Берём предпоследний шаг
    target_step = steps[-2]

    # Ищем элемент с решением (атрибут data-tex)
    solution_elem = target_step.query_selector(".ktx")

    if solution_elem:
        solution_tex = solution_elem.get_attribute("data-tex")

        render_latex(solution_tex)
    else:
        print("⚠️ Не удалось найти решение в предпоследнем шаге.")

    # Оставляем браузер открытым, чтобы посмотреть результат
    # browser.close()