import random
from math import gcd

def jacobi_symbol(a, n):
    a %= n
    result = 1
    while a != 0:
        while a % 2 == 0:
            a //= 2
            if n % 8 in (3, 5):
                result = -result
        a, n = n, a
        if a % 4 == 3 and n % 4 == 3:
            result = -result
        a %= n
    return result if n == 1 else 0

def test_ferma(n, a):
    g = gcd(a, n)
    if g != 1:
        return False, f"НОД(a, n) = {g} != 1"
    r = pow(a, n - 1, n)
    if r == 1:
        return True, f"a^(n-1) mod n = 1"
    return False, f"a^(n-1) mod n = {r} != 1 (нарушено a^(n-1) = 1 (mod n))"

def test_soloveya(n, a):
    g = gcd(a, n)
    if g != 1:
        return False, f"НОД(a, n) = {g} != 1"
    r = pow(a, (n - 1) // 2, n)
    if r != 1 and r != n - 1:
        return False, f"a^((n-1)/2) mod n = {r}, это не 1 и не n-1"
    s = jacobi_symbol(a, n)
    if r != s % n:
        return False, f"a^((n-1)/2) mod n = {r}, символ Якоби (a/n) = {s}, не сравнимы по модулю n"
    return True, f"a^((n-1)/2) mod n = {r} = (a/n) = {s} (mod n)"

def test_miller_rabin(n, a):
    s, r = 0, n - 1
    while r % 2 == 0:
        s += 1
        r //= 2
    y = pow(a, r, n)
    if y == 1 or y == n - 1:
        return True, f"n-1 = 2^{s} * r; a^r mod n = {'1' if y == 1 else 'n-1'}"
    j = 1
    while j <= s - 1 and y != n - 1:
        y = (y * y) % n
        if y == 1:
            return False, (f"n-1 = 2^{s} * r; на шаге j={j} получено a^(2^{j} r) = 1, "
                           f"а предыдущее значение не было ±1 (нетривиальный корень из 1)")
        j += 1
    if y != n - 1:
        return False, f"n-1 = 2^{s} * r; ни a^r, ни a^(2^j r), j=0..{s-1}, не равны ±1 (mod n)"
    return True, f"n-1 = 2^{s} * r; a^(2^{j-1} r) = n-1 (mod n)"

TESTS = [("Ферма", test_ferma),
         ("Соловэя-Штрассена", test_soloveya),
         ("Рабина-Миллера", test_miller_rabin)]

def short(x, w=60):
    s = str(x)
    return s if len(s) <= w else s[:25] + "..." + s[-25:]

def report(n, tries=30):
    print("=" * 78)
    print(f"n = {n}  ({len(str(n))} зн.)")
    if n % 2 == 0:
        print("n чётное - составное"); return
    for name, test in TESTS:
        random.seed(1)
        bases = [random.randint(2, n - 2) for _ in range(tries)]
        witness = None
        passed = []
        for a in bases:
            ok, info = test(n, a)
            if ok:
                if len(passed) < 5:
                    passed.append((a, info))
            elif witness is None:
                witness = (a, info)
        print(f"\n-- Тест {name} --")
        if witness:
            print("Результат: составное")
            print(f"  основание a = {witness[0]}")
            print(f"  нарушено: {witness[1]}")
        else:
            print(f"Результат: вероятно простое ({tries} оснований)")
            for a, info in passed:
                print(f"  a = {short(a)}: {info if len(info) < 200 else short(info,120)}")

def is_prime(n):
    if n < 2: return False
    for p in (2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37):
        if n % p == 0: return n == p
    return all(test_miller_rabin(n, a)[0] for a in (2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37))

def chernick(count=3, start_k=10**9):
    res, k = [], start_k
    while len(res) < count:
        if is_prime(6*k+1) and is_prime(12*k+1) and is_prime(18*k+1):
            res.append((k, (6*k+1, 12*k+1, 18*k+1)))
        k += 1
    return res

def carmichael_report():
    for k, (p, q, r) in chernick():
        n = p * q * r
        print("\n" + "=" * 78)
        print(f"k = {k}\nn = {n}  ({len(str(n))} зн.)")
        print(f"Разложение: {p} * {q} * {r}")
        print(f"Проверка Корселта: (n-1) mod (p-1),(q-1),(r-1) = "
              f"{(n-1)%(p-1)}, {(n-1)%(q-1)}, {(n-1)%(r-1)}")
        for name, test in TESTS:
            random.seed(2)
            yes = no = None
            for _ in range(10000):
                a = random.randint(2, n - 2)
                ok, info = test(n, a)
                if ok and yes is None: yes = (a, info)
                if not ok and no is None: no = (a, info)
                if yes and no: break
            if name == "Ферма":              # свидетель с НОД > 1
                no = (p, test(n, p)[1])
            print(f"\n-- {name} --")
            if yes: print(f"  'простое':   a = {yes[0]}\n      {short(yes[1],110)}")
            else:   print("  'простое': основание не найдено за 10000 попыток")
            if no:  print(f"  'составное': a = {no[0]}\n      {no[1]}")

if __name__ == "__main__":
    for n in (13945754999792127277,
              1910392819281856753342362664981670449081,
              230868847099016479369241273560654413619,
              10559821643272010452261027279670030138425095408945904756634125771064441932046413):
        report(n)
    carmichael_report()
