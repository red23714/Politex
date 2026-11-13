import random
import math
import time
import sys

sys.set_int_max_str_digits(1000000)  # для печати больших чисел P_i

# ==================================================================
# расширенный бинарный алгоритм Евклида (из лабораторной работы №1)
# ==================================================================
def gcd(a, b):
    g = 1

    while a % 2 == 0 and b % 2 == 0:
        a = a // 2
        b = b // 2
        g = 2 * g

    u = a
    v = b
    A = 1
    B = 0
    C = 0
    D = 1

    while u != 0:
        while u % 2 == 0:
            u = u // 2
            if A % 2 == 0 and B % 2 == 0:
                A = A // 2
                B = B // 2
            else:
                A = (A + b) // 2
                B = (B - a) // 2

        while v % 2 == 0:
            v = v // 2

            if C % 2 == 0 and D % 2 == 0:
                C = C // 2
                D = D // 2
            else:
                C = (C + b) // 2
                D = (D - a) // 2

        if u >= v:
            u = u - v
            A = A - C
            B = B - D
        else:
            v = v - u
            C = C - A
            D = D - B

    d = g * v

    return d

# ==================================================================
# вспомогательные функции
# ==================================================================
def primes_up_to(B):
    # решето Эратосфена: все простые числа <= B
    sieve = [True] * (B + 1)
    sieve[0] = sieve[1] = False
    for i in range(2, int(B ** 0.5) + 1):
        if sieve[i]:
            for j in range(i * i, B + 1, i):
                sieve[j] = False
    return [i for i in range(2, B + 1) if sieve[i]]

def legendre_sym(a, p):
    # символ Лежандра (a/p) по квадратичному закону взаимности
    a = a % p
    t = 1
    while a != 0:
        while a % 2 == 0:
            a = a // 2
            if p % 8 in (3, 5):
                t = -t
        a, p = p, a
        if a % 4 == 3 and p % 4 == 3:
            t = -t
        a = a % p
    if p == 1:
        return t
    return 0

def factor_base(n, B):
    # факторная база B = {-1, p_1, ..., p_h}: простые p <= B, по модулю
    # которых n является квадратичным вычетом, то есть (n/p) = 1
    base = [-1]
    for p in primes_up_to(B):
        if p == 2 or legendre_sym(n, p) == 1:
            base.append(p)
    return base

def is_B_smooth(x, base):
    # проверка B-гладкости числа x и построение вектора показателей e;
    # если x не гладкое, возвращается None
    v = [0] * len(base)
    for i in range(len(base)):
        p = base[i]
        if p == -1:
            while x < 0:
                x = -x
                v[i] = v[i] + 1
        else:
            while x % p == 0:
                x = x // p
                v[i] = v[i] + 1
    if x == 1:
        return v
    return None

def sqrt_mod(a, p):
    # решение сравнения x^2 = a (mod p) (алгоритм Тонелли-Шенкса)
    a = a % p
    if p == 2:
        return 1
    if pow(a, (p - 1) // 2, p) != 1:
        return None
    if p % 4 == 3:
        return pow(a, (p + 1) // 4, p)
    # общий случай Тонелли-Шенкса
    q = p - 1
    s = 0
    while q % 2 == 0:
        q = q // 2
        s = s + 1
    z = 2
    while pow(z, (p - 1) // 2, p) != p - 1:
        z = z + 1
    m = s
    c = pow(z, q, p)
    t = pow(a, q, p)
    R = pow(a, (q + 1) // 2, p)
    while t != 1:
        i = 0
        t2 = t
        while t2 != 1:
            t2 = t2 * t2 % p
            i = i + 1
        b = pow(c, 1 << (m - i - 1), p)
        m = i
        c = b * b % p
        t = t * c % p
        R = R * b % p
    return R

# ==================================================================
# поиск линейной зависимости методом Гаусса над F2 (шаг 3 алгоритма 6.3);
# векторы задаются битовыми масками, к каждому приписывается единичный
# столбец с номером строки, по нулевой строке восстанавливается множество K;
# возвращает до count независимых нулевых строк (зависимостей)
# ==================================================================
def find_dependency(vectors, k, count=3):
    m = len(vectors)
    rows = []
    for i in range(m):
        rows.append(vectors[i] | (1 << (k + i)))

    r = 0
    for c in range(k):
        # ищем строку с единицей в столбце c
        sel = -1
        for i in range(r, m):
            if (rows[i] >> c) & 1 == 1:
                sel = i
                break
        if sel == -1:
            continue
        rows[r], rows[sel] = rows[sel], rows[r]
        # гауссово исключение: обнуляем столбец c во всех остальных строках
        for i in range(m):
            if i != r and (rows[i] >> c) & 1 == 1:
                rows[i] = rows[i] ^ rows[r]
        r = r + 1

    # нулевые строки дают искомые множества K
    deps = []
    for i in range(m):
        vec = rows[i] & ((1 << k) - 1)
        idx = rows[i] >> k
        if vec == 0 and idx != 0:
            # из маски делаем список номеров строк
            K = []
            for j in range(k + m):
                if (idx >> j) & 1 == 1:
                    K.append(j)
            deps.append(K)
            if len(deps) >= count:
                break
    return deps

# ==================================================================
# ро-метод Полларда (алгоритм 6.1)
# ==================================================================
def pollard_rho(n, c, f, max_iter=100000000):
    # алгоритм Флойда: a движется с удвоенной скоростью
    a = c
    b = c
    it = 0
    history = []

    while True:
        a = f(a) % n
        b = f(f(b) % n) % n
        d = gcd(abs(a - b), n)
        it = it + 1
        history.append((a, b, d))

        if it <= 5 or it % 1000 == 0:
            print("i =", it, " a =", a, " b =", b, " d =", d)

        if 1 < d < n:
            print("делитель найден:", d)
            return d, it, history
        if d == n:
            print("d = n, делитель не найден")
            return None, it, history

def run_rho(n, c, f):
    print("=== ро-метод Полларда: n =", n, " c =", c, " f(x) = x^2 + 1 ===")
    t0 = time.time()
    d, it, history = pollard_rho(n, c, f)
    t = time.time() - t0
    print("число итераций:", it)
    print("время работы:", round(t, 2), "сек")
    print("первые 5 значений (i, a, b, НОД(a-b, n)):")
    for i in range(min(5, len(history))):
        print("  ", i + 1, history[i])
    print("последние 5 значений (i, a, b, НОД(a-b, n)):")
    for i in range(max(0, len(history) - 5), len(history)):
        print("  ", i + 1, history[i])
    return d, it, t

# ==================================================================
# (p-1)-метод Полларда (алгоритм 6.2)
# ==================================================================
def pollard_p_minus_1(n, base):
    # показатели l_i = [ln n / ln p_i]
    a = 2
    print("=== (p-1)-метод Полларда: n =", n, "===")
    print("размер базы:", len(base), "последний элемент:", base[-1])

    ls = []
    for p in base:
        l = int(math.log(n) // math.log(p))
        ls.append(l)
        a = pow(a, p ** l, n)

    d = gcd(a - 1, n)

    print("основание a =", a)
    print("НОД(a - 1, n) =", d)
    if 1 < d < n:
        print("делитель найден:", d)
        return a, d, ls
    print("делитель не найден")
    return a, d, ls

def run_p_minus_1(n, B):
    base = primes_up_to(B)
    t0 = time.time()
    a, d, ls = pollard_p_minus_1(n, base)
    t = time.time() - t0
    print("время работы:", round(t, 2), "сек")
    print("значения l_i (уникальные):", sorted(set(ls)))
    return a, d, t

# ==================================================================
# шаги 4-5 алгоритма 6.3, общие для CFRAC и QS:
# по множеству K считаем s, t и делитель
# ==================================================================
def finish_dixon(n, K, s_list, exp_vectors, base):
    k = len(base)
    s = 1
    beta = [0] * k
    for i in K:
        s = s * s_list[i] % n
        for j in range(k):
            beta[j] = beta[j] + exp_vectors[i][j]

    print("множество K (номера строк):", K)
    print("суммарный вектор beta:", beta)

    # t = произведение p_j^(beta_j / 2)
    t = 1
    for j in range(k):
        if base[j] != -1 and beta[j] > 0:
            t = t * pow(base[j], beta[j] // 2, n) % n

    d1 = gcd(abs(s - t), n)
    d2 = gcd(s + t, n)
    print("s =", s)
    print("t =", t)
    print("s^2 mod n =", s * s % n)
    print("t^2 mod n =", t * t % n)
    print("НОД(s - t, n) =", d1)
    print("НОД(s + t, n) =", d2)

    if 1 < d1 < n:
        print("делитель:", d1)
        return d1
    if 1 < d2 < n:
        print("делитель:", d2)
        return d2
    print("s = ±t (mod n), тривиальная зависимость")
    return None

# ==================================================================
# метод непрерывных дробей (алгоритм 6.3 + раздел 6.4.1)
# ==================================================================
def CFRAC(n, B, max_iter=100000000):
    print("=== метод непрерывных дробей: n =", n, " B =", B, "===")
    t0 = time.time()

    # шаг 1: база разложения
    base = factor_base(n, B)
    k = len(base)
    print("факторная база:", k, "элементов, последний:", base[-1])
    print("база:", base)

    # рекуррентные формулы для разложения sqrt(n) в непрерывную дробь:
    # P_0 = 0, Q_0 = 1, a_i = [(P_i + sqrt(n)) / Q_i]
    # числители подходящих дробей: P_i = a_i * P_{i-1} + P_{i-2}
    P = 0
    Q = 1
    Pm1 = 1  # P_{-1}
    Pm2 = 0  # P_{-2}

    s_list = []        # числители P_i
    exp_vectors = []   # векторы показателей
    mods = []          # значения P_i^2 mod n

    for i in range(max_iter):
        # очередное неполное частное и числитель подходящей дроби
        a_i = (math.isqrt(n) + P) // Q
        Pi = a_i * Pm1 + Pm2
        Pm2 = Pm1
        Pm1 = Pi

        # рекуррентные формулы для P_{i+1}, Q_{i+1}
        P_next = a_i * Q - P
        Q_next = (n - P_next * P_next) // Q

        # P_i^2 mod n = |P_i^2 - n*Q_i^2| < 2*sqrt(n) (следствие теоремы 6.2),
        # при этом P_i^2 - n*Q_i^2 = (-1)^(i+1) * Q_{i+1}
        if i % 2 == 1:
            val = Q_next
        else:
            val = n - Q_next

        # проверка B-гладкости
        v = is_B_smooth(val, base)
        if v is not None:
            s_list.append(Pi)
            mods.append(val)
            exp_vectors.append(v)
            if len(s_list) <= 5 or len(s_list) % 100 == 0:
                print("гладкое №", len(s_list), ": i =", i, " P_i =", Pi,
                      " P_i^2 mod n =", val)

        P = P_next
        Q = Q_next

        # нужно h+2 гладких чисел
        if len(s_list) >= k + 2:
            break

    print("найдено гладких:", len(s_list), "нужно:", k + 2)
    print("время:", round(time.time() - t0, 2), "сек")

    print("первые 5 значений P_i и (P_i)^2 mod n:")
    for i in range(min(5, len(s_list))):
        print("   P_i =", s_list[i], "  (P_i)^2 mod n =", mods[i])
        print("   вектор:", exp_vectors[i])
    print("последние 5 значений P_i и (P_i)^2 mod n:")
    for i in range(max(0, len(s_list) - 5), len(s_list)):
        print("   P_i =", s_list[i], "  (P_i)^2 mod n =", mods[i])
        print("   вектор:", exp_vectors[i])

    if len(s_list) < k + 2:
        print("гладких чисел недостаточно")
        return None

    # шаг 3: векторы показателей по модулю 2 -> поиск зависимости
    masks = []
    for v in exp_vectors:
        mask = 0
        for j in range(k):
            if v[j] % 2 == 1:
                mask = mask | (1 << j)
        masks.append(mask)

    t1 = time.time()
    deps = find_dependency(masks[:k + 1], k)
    print("поиск зависимости занял", round(time.time() - t1, 2), "сек")
    if len(deps) == 0:
        print("зависимость не найдена")
        return None

    # шаги 4-5: перебираем найденные зависимости, пока не получим нетривиальный
    # делитель (первая может оказаться тривиальной: s = ±t (mod n))
    for K in deps:
        d = finish_dixon(n, K, s_list, exp_vectors, base)
        if d is not None:
            print("полное время работы:", round(time.time() - t0, 2), "сек")
            return d
        print("зависимость оказалась тривиальной, пробуем следующую")

    print("полное время работы:", round(time.time() - t0, 2), "сек")
    return None

# ==================================================================
# метод квадратичного решета (алгоритм 6.3 + раздел 6.4.2)
# ==================================================================
def QS(n, B, M):
    # f(x) = (x + [sqrt(n)])^2 - n, просеиваем x из диапазона |x| < M
    print("=== метод квадратичного решета: n =", n, " B =", B, " M =", M, "===")
    t0 = time.time()

    # шаг 1: база разложения
    base = factor_base(n, B)
    k = len(base)
    print("факторная база:", k, "элементов, последний:", base[-1])
    print("база:", base)

    r = math.isqrt(n)
    print("[sqrt(n)] =", r, ", диапазон x: (-", M, ", ", M, ")", sep="")

    # просеивание: для каждого p базы решаем сравнение (6.1):
    # (x + r)^2 = n (mod p), получаем два корня x = x_j - r (mod p)
    # и отмечаем в массиве все такие x
    log_sieve = [0.0] * (2 * M + 1)
    for p in base:
        if p == -1:
            continue
        lp = math.log(p)
        x1 = sqrt_mod(n, p)
        if x1 is None:
            continue
        x2 = (p - x1) % p
        for xr in {x1, x2}:
            a0 = (xr - r) % p
            start = (a0 + M) % p
            while start < 2 * M + 1:
                log_sieve[start] = log_sieve[start] + lp
                start = start + p

    print("просеивание завершено за", round(time.time() - t0, 2), "сек")

    # кандидаты: log f(x) ~ log(2 r M); так как решето учитывает каждое
    # простое один раз, а показатель степени в разложении f(x) может быть
    # больше 1, берём запас побольше и всех кандидатов проверяем
    # пробными делениями по факторной базе
    target = math.log(2 * r * M) - 2.5 * math.log(B)

    s_list = []        # значения s_i = x + r
    xs = []            # значения x
    exp_vectors = []   # векторы показателей
    mods = []          # значения f(x)

    for idx in range(2 * M + 1):
        if log_sieve[idx] > target - math.log(B) / 2:
            x = idx - M
            si = x + r
            fx = si * si - n
            v = is_B_smooth(fx, base)
            if v is not None:
                s_list.append(si)
                xs.append(x)
                mods.append(fx)
                exp_vectors.append(v)

    print("найдено гладких:", len(s_list), "нужно:", k + 2)
    print("время на просеивание и проверку:", round(time.time() - t0, 2), "сек")

    print("первые 5 значений x и f(x):")
    for i in range(min(5, len(s_list))):
        print("   x =", xs[i], "  f(x) =", mods[i])
        print("   вектор:", exp_vectors[i])
    print("последние 5 значений x и f(x):")
    for i in range(max(0, len(s_list) - 5), len(s_list)):
        print("   x =", xs[i], "  f(x) =", mods[i])
        print("   вектор:", exp_vectors[i])

    if len(s_list) < k + 2:
        print("гладких чисел недостаточно")
        return None

    # шаг 3: векторы показателей по модулю 2 -> поиск зависимости
    masks = []
    for v in exp_vectors:
        mask = 0
        for j in range(k):
            if v[j] % 2 == 1:
                mask = mask | (1 << j)
        masks.append(mask)

    t1 = time.time()
    deps = find_dependency(masks[:k + 1], k)
    print("поиск зависимости занял", round(time.time() - t1, 2), "сек")
    if len(deps) == 0:
        print("зависимость не найдена")
        return None

    # шаги 4-5; s_i = x_i + [sqrt(n)]; перебираем зависимости, пока
    # не получим нетривиальный делитель
    for K in deps:
        d = finish_dixon(n, K, s_list, exp_vectors, base)
        if d is not None:
            print("полное время работы:", round(time.time() - t0, 2), "сек")
            return d
        print("зависимость оказалась тривиальной, пробуем следующую")

    print("полное время работы:", round(time.time() - t0, 2), "сек")
    return None

# ==================================================================
# числа варианта
# ==================================================================
N1 = 7725862768907051557
N2 = 434154551935578441586027428350531478331
N3 = 10854380007467828036094467779670889360795162191206049015516035227184511774880289

if __name__ == "__main__":
    pass
