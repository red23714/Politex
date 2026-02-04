import sympy as sp
import re
from playwright.sync_api import sync_playwright
import matplotlib.pyplot as plt
import io
from PIL import Image
import numpy as np
import base64

# ================================================================
# Символы и константа
# ================================================================
x, y = sp.symbols('x y')
C = sp.Symbol('C')

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

# ================================================================
# ======================= Общие функции =========================
# ================================================================
def parse_equation(eq_str: str) -> sp.Expr:
    """
    Преобразует строку уравнения в символьное выражение F(x, y):
    поддерживается формат dy/dx = ... или f(y)*dy = g(x)*dx
    """
    eq_str = eq_str.replace(' ', '')
    if 'dy/dx' in eq_str:
        rhs = sp.sympify(eq_str.split('=')[1], locals={'x': x, 'y': y})
        return sp.simplify(rhs)
    elif '*dy' in eq_str and '*dx' in eq_str and '=' in eq_str:
        left, right = eq_str.split('=')
        f_y = sp.sympify(left.replace('*dy', ''), locals={'x': x, 'y': y})
        g_x = sp.sympify(right.replace('*dx', ''), locals={'x': x, 'y': y})
        return sp.simplify(g_x / f_y)
    else:
        raise ValueError("Неверный формат. Используй 'dy/dx = ...' или 'f(y)*dy = g(x)*dx'.")


def integrate(expr: sp.Expr, var: sp.Symbol) -> sp.Expr:
    """Интегрирование выражения"""
    return sp.integrate(expr, var)


def simplify(expr: sp.Expr) -> sp.Expr:
    """Упрощение выражения"""
    return sp.simplify(expr)


# ================================================================
# ======================= Проверки типов ========================
# ================================================================
def is_separable(eq_str: str) -> tuple[bool, sp.Expr, sp.Expr]:
    """
    Проверяет, является ли уравнение разделяющимся.
    Возвращает (True, f(x), g(y)) или (False, None, None)
    """
    try:
        F = parse_equation(eq_str)
        separated = sp.separatevars(F, symbols=(x, y), dict=True)
        if separated and x in separated and y in separated:
            return True, separated[x], separated[y]

        N, D = sp.together(F).as_numer_denom()
        N_syms = {s for s in N.free_symbols if s in (x, y)}
        D_syms = {s for s in D.free_symbols if s in (x, y)}
        if N_syms <= {x} and D_syms <= {y}:
            return True, simplify(N), simplify(1/D)
        if N_syms <= {y} and D_syms <= {x}:
            return True, simplify(1/D), simplify(N)

    except Exception:
        pass

    return False, None, None


def is_homogeneous(eq_str: str) -> tuple[bool, str]:
    """
    Проверяет, является ли уравнение однородным.
    Возвращает (True, 'y/x' или 'x/y') или (False, None)
    """
    try:
        F = parse_equation(eq_str)
        z = sp.Symbol('z')
        if F.subs(y, z*x).free_symbols == {z}:
            return True, 'y/x'
        if F.subs(x, z*y).free_symbols == {z}:
            return True, 'x/y'
    except Exception:
        pass
    return False, None


def is_linear(eq_str: str) -> tuple[bool, sp.Expr, sp.Expr]:
    """
    Проверяет, является ли уравнение линейным первого порядка
    dy/dx + P(x)*y = f(x)
    Возвращает (True, P, f) или (False, None, None)
    """
    try:
        F = parse_equation(eq_str)
        A = sp.diff(F, y)
        if simplify(sp.diff(A, y)) != 0:
            return False, None, None
        B = simplify(F - A*y)
        if A.free_symbols <= {x} and B.free_symbols <= {x}:
            return True, A, B
    except Exception:
        pass
    return False, None, None


def is_bernoulli(eq_str: str) -> tuple[bool, sp.Expr, sp.Expr, sp.Rational]:
    """
    Проверяет, является ли уравнение уравнением Бернулли
    dy/dx + P*y = Q*y^n
    Возвращает (True, P, Q, n) или (False, None, None, None)
    """
    try:
        eq_str = eq_str.replace(' ', '')
        if '=' not in eq_str:
            return False, None, None, None
        left, right = eq_str.split('=')
        if 'dy/dx' not in left:
            return False, None, None, None

        # Коэффициент P
        left_part = left.replace('dy/dx','')
        if left_part in ['', '+']:
            P = 0
        elif '*y' in left_part:
            P = simplify(sp.sympify(left_part.replace('*y',''), locals={'x':x}))
        else:
            return False, None, None, None

        rhs = sp.sympify(right, locals={'x': x, 'y': y})
        if not rhs.has(y):
            return False, None, None, None

        # Определение n
        poly_y = sp.degree(rhs, y)
        if poly_y is not None:
            n = sp.Rational(poly_y)
            Q = simplify(rhs / (y**n))
            if Q.free_symbols <= {x}:
                return True, P, Q, n

        # Пробуем диапазон возможных степеней
        for n_try in range(-3,6):
            test = simplify(rhs / (y**n_try))
            if test.free_symbols <= {x}:
                return True, P, test, sp.Rational(n_try)

    except Exception:
        pass

    return False, None, None, None


def is_full(eq_str: str) -> bool:
    """Проверяет, является ли уравнение полным M*dx + N*dy = 0"""
    return eq_str.replace(' ', '').endswith('=0')


# ================================================================
# ======================= Решения ===============================
# ================================================================
def solve_bernoulli_eq(eq_str: str) -> sp.Eq:
    """
    Решение уравнения Бернулли dy/dx + P*y = Q*y^n пошагово:
    1️⃣ Делим на y^n
    2️⃣ Замена z = 1 / y^(n-1)
    3️⃣ Получаем линейное уравнение для z
    4️⃣ Решаем линейное уравнение
    5️⃣ Возвращаемся к y
    """
    is_b, P, Q, n = is_bernoulli(eq_str)
    if not is_b:
        raise ValueError("Не является уравнением Бернулли.")

    print(f"\n✅ Уравнение Бернулли: dy/dx + ({sp.pretty(P)})*y = ({sp.pretty(Q)})*y^{n}")

    # 1️⃣ Делим уравнение на y^n
    print("\n1️⃣ Делим всё на y^n:")
    print(f"1/y^{n} * dy/dx + P*y^{1-n} = Q")

    # 2️⃣ Замена z = 1 / y^(n-1)
    z = sp.Symbol('z', real=True)
    print("\n2️⃣ Замена переменной: z = 1 / y^(n-1)")
    print("dz/dx = (1 - n) * y^(-n) * dy/dx")

    # 3️⃣ Линейное уравнение для z
    Pz = simplify((1-n)*P)
    Qz = simplify((1-n)*Q)
    print(f"\n3️⃣ Получаем линейное уравнение для z:\n dz/dx + ({sp.pretty(Pz)})*z = {sp.pretty(Qz)}")

    # 4️⃣ Решаем линейное уравнение для z
    μ = sp.exp(integrate(Pz, x))
    z_expr = simplify((integrate(μ*Qz, x)+C)/μ)
    print(f"\nИнтегрирующий множитель μ(x) = e^∫Pz dx = \n{sp.pretty(μ)}")
    print(f"Решение для z: z = {sp.pretty(z_expr)}")

    # 5️⃣ Возвращаемся к y
    y_expr = simplify((1/z_expr)**(1/(n-1)))
    print(f"\n5️⃣ Подставляем обратно z = 1 / y^(n-1)")
    print(f"📘 Итоговое решение: y = \n{sp.pretty(y_expr)}")

    return sp.Eq(y, y_expr)


def solve_linear_eq(eq_str: str) -> sp.Eq:
    """
    Решение линейного уравнения dy/dx + P*y = f(x) пошагово:
    1️⃣ Находим интегрирующий множитель μ = exp(∫P dx)
    2️⃣ Умножаем уравнение на μ и интегрируем
    3️⃣ Выражаем y
    """
    is_lin, P, f = is_linear(eq_str)
    if not is_lin:
        raise ValueError("Не является линейным уравнением.")

    print(f"\n✅ Линейное уравнение: dy/dx + ({sp.pretty(P)})*y = \n{sp.pretty(f)}")

    # 1️⃣ Интегрирующий множитель
    μ = sp.exp(integrate(P, x))
    print(f"\n1️⃣ Интегрирующий множитель: μ(x) = exp(∫P dx) = \n{sp.pretty(μ)}")

    # 2️⃣ Умножаем на μ и интегрируем
    y_expr = simplify((integrate(μ*f, x)+C)/μ)
    print(f"\n2️⃣ Интегрируем: ∫(μ*f) dx / μ = \n{sp.pretty(y_expr)}")

    # 3️⃣ Решение
    print(f"\n📗 Итоговое решение: y = \n{sp.pretty(y_expr)}")
    return sp.Eq(y, y_expr)


def solve_separable_eq(eq_str: str) -> sp.Eq:
    sep, fx, fy = is_separable(eq_str)
    if not sep:
        raise ValueError("Не является разделяющимся.")
    print(f"\n✅ Разделяющееся уравнение")
    print(f"F(x)*G(y): f(x) = \n{sp.pretty(fx)}\n g(y) = \n{sp.pretty(fy)}")
    left = integrate(1/fy, y)
    right = integrate(fx, x)
    print(f"Интегрируем: ∫(1/g(y)) dy = \n{sp.pretty(left)}\n ∫f(x) dx = \n{sp.pretty(right)}")
    sol = sp.Eq(left, right + C)
    print(f"📗 Итоговое решение: \n{sp.pretty(sol)}")
    return sol


def solve_homogeneous_eq(eq_str: str) -> sp.Eq:
    F = parse_equation(eq_str)
    is_hom, form = is_homogeneous(eq_str)
    if not is_hom:
        raise ValueError("Не является однородным.")
    print(f"\n✅ Однородное уравнение ({form})")
    u = sp.Symbol('u', real=True)
    f_sub = F.subs(y, u*x)
    print(f"Подставляем y = u*x: F -> \n{sp.pretty(f_sub)}")
    left = integrate(1/(f_sub - u), u)
    right = integrate(1/x, x)
    print(f"Интегрируем: ∫1/(F - u) du = \n{sp.pretty(left)}\n ∫1/x dx = {sp.pretty(right)}")
    sol = sp.Eq(left, right + C).subs(u, y/x)
    print(f"Возвращаемся к y: y = u*x -> y/x = u")
    print(f"📗 Итоговое решение: \n{sp.pretty(sol)}")
    return sol



def extract_M_N(eq_str: str) -> tuple[sp.Expr, sp.Expr]:
    eq = eq_str.replace(' ','').replace('=0','')
    pattern = re.compile(r'(.+?)\*dx\+?\(?(.+?)\)?\*dy')
    match = pattern.match(eq)
    if not match:
        pattern = re.compile(r'(.+?)\*dy\+?\(?(.+?)\)?\*dx')
        match = pattern.match(eq)
        if not match:
            raise ValueError("Не удалось распознать dx и dy")
        N_str, M_str = match.groups()
    else:
        M_str, N_str = match.groups()
    return sp.sympify(M_str), sp.sympify(N_str)


def solve_full_eq(eq_str: str) -> sp.Eq:
    M, N = extract_M_N(eq_str)
    if not simplify(sp.diff(M, y) - sp.diff(N, x)) == 0:
        raise ValueError("Не является полным уравнением.")
    Fx = integrate(M, x)
    g_y = integrate(N - sp.diff(Fx, y), y)
    sol = sp.Eq(Fx + g_y, C)
    print(f"\n✅ Полное уравнение\nРешение:\n{sp.pretty(sol)}")
    return sol


# ================================================================
# =================== Автоопределение и решение =================
# ================================================================
def solve_equation(eq_str: str) -> sp.Eq | None:
    print(f"\n>>> {eq_str}")
    try:
        if is_full(eq_str):
            return solve_full_eq(eq_str)
        elif is_homogeneous(eq_str)[0]:
            return solve_homogeneous_eq(eq_str)
        elif is_linear(eq_str)[0]:
            return solve_linear_eq(eq_str)
        elif is_bernoulli(eq_str)[0]:
            return solve_bernoulli_eq(eq_str)
        elif is_separable(eq_str)[0]:
            return solve_separable_eq(eq_str)
        else:
            print("Тип уравнения не распознан.")
    except Exception as e:
        print("Ошибка:", e)



# ================================================================
# Примеры
# ================================================================
if __name__ == "__main__":
    input_text = input("Введите выражение (или 'q' для выхода): ")

    with sync_playwright() as p:
        browser = p.chromium.launch(headless=True)
        page = browser.new_page()

        coded_string = '''aHR0cHM6Ly9tYXRoZGYuY29tL2RpZi9ydQ=='''

        page.goto(base64.b64decode(coded_string).decode('ascii'))

        while input_text != 'q':
            page.wait_for_selector("#input-expression")
            page.fill("#input-expression", input_text)

            page.click("#solve")

            page.wait_for_selector("#math-canvas", timeout=20000)

            steps = page.query_selector_all(".step")

            selected_equations = extract_equations_from_steps(steps)
            
            if selected_equations:
                selected_equations = [selected_equations[-1]]
                output_filename = f"solution_{input_text.replace('/', '_')}.png"
                combined_image = render_all_steps(
                    selected_equations, 
                    fontsize=20, 
                    output_filename=output_filename,
                    dpi=150
                )
                
                if combined_image:
                    plt.figure(figsize=(10, len(selected_equations) * 2))
                    plt.imshow(np.array(combined_image))
                    plt.axis('off')
                    plt.tight_layout()
                    plt.show()
                else:
                    print("❌ Не удалось создать объединенное изображение")
            
            input_text = input("Введите следующее выражение (или 'q' для выхода): ")

        browser.close()
