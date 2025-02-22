import math

def factorial(n):
    result = 1
    for i in range(2, n + 1):
        result *= i
    return result

def main():
    i = 1
    current_sum = 0.0
    sum_old = 0.2

    x = float(input())

    while abs(current_sum - sum_old) > 0.01:
        sum_old = current_sum
        a = 1 if (i // 2) % 2 == 0 else -1
        c = factorial(i + 1)
        d = 1 << (i + 1)
        b_double = (-x) ** (2 * i + 3)

        current_sum += a * b_double / c / d
        print(c)
        i += 1

if __name__ == "__main__":
    main()

