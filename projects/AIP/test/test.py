import random

test_count = 200
output = []
input = []

def generate_test():
    buff = ""

    for _ in range(0, test_count):
        n = random.randint(0, 20)
        buff += str(n)

        for _ in range(0, n):
            x = random.randint(0, 18446744073709551615)
        
            if x % 3 == 0:
                x = random.randint(0, 4294967296)
                x *= x

            buff += ' ' + str(x)

        input.append(buff)
        buff = ""

def check_test():
    for i in range(0, test_count):
        buff = input[i].split(' ')
        max_square = -1
        index = -1
        buff = [ int(x) for x in buff ]
        for j in range(1, buff[0] + 1):
            if(is_square(buff[j]) and buff[j] > max_square):
                index = j - 1
                max_square = buff[j]
        output.append(index)


def is_square(apositiveint):
    x = apositiveint // 2
    seen = set([x])

    while x * x != apositiveint:
        x = (x + (apositiveint // x)) // 2
        if x in seen: return False
        seen.add(x)
    return True