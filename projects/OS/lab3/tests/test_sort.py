import random
import sys

threads = [1, 2, 4, 12, 25, 63]
count = [50000, 5000000, 10000000]


def main():
    print("Генератор input.txt для qsort / msort")

    rng = random.Random()
    CHUNK = 200_000

    for elem in count:
        for thread in threads:
            with open(f"input_{elem}_{thread}.txt", "w") as f:
                T = thread
                N = elem
                mode = "1"
                f.write(f"{T}\n {N}\n")
                written = 0
                first = True
                while written < N:
                    size = min(CHUNK, N - written)
                    if mode == "1":
                        nums = [rng.randint(-(10**9), 10**9) for _ in range(size)]
                    elif mode == "2":
                        start = N - written
                        nums = list(range(start, start - size, -1))
                    else:
                        nums = [rng.randint(1, 100) for _ in range(size)]
                    f.write(("" if first else " ") + " ".join(map(str, nums)))
                    first = False
                    written += size
                f.write("\n")

    print(f"\nГотово! Создан input.txt  (T={T}, N={N})")


if __name__ == "__main__":
    main()
