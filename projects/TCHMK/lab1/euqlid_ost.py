def nod_min_abs(a, b):
    r0, r1 = a, b
    x0, x1 = 1, 0
    y0, y1 = 0, 1
    i = 1

    while True:
        fq = r0 // r1
        cand_low = r0 - fq * r1
        cand_high = cand_low - r1

        if abs(cand_low) < abs(cand_high):
            q, r_next = fq, cand_low
        elif abs(cand_low) > abs(cand_high):
            q, r_next = fq + 1, cand_high
        else:
            if cand_low > 0:
                q, r_next = fq, cand_low
            else:
                q, r_next = fq + 1, cand_high

        if r_next == 0:
            sign = 1 if r1 > 0 else -1
            d = sign * r1
            x = sign * x1
            y = sign * y1
            print(f"\nd = {d}, x = {x}, y = {y}")
            return x, y, d

        x_next = x0 - q * x1
        y_next = y0 - q * y1

        print(f"{i + 1:>3} {r_next:>8} {x_next:>6} {y_next:>6} {q:>6}")

        r0, r1 = r1, r_next
        x0, x1 = x1, x_next
        y0, y1 = y1, y_next
        i += 1


print("1.")
print(nod_min_abs(9190813042428527561, 3692656050301860119))

print("\n2.")
print(
    nod_min_abs(
        422070269743864020891557233196060131009,
        315970570002032382346428535803118840663,
    )
)

print("\n3.")
print(
    nod_min_abs(
        51270458771916610256956412768918119681281326451560924039920282880900791951281613,
        66258158090480637075800780925780649193407163093965839123475623634634432994618769,
    )
)
