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
