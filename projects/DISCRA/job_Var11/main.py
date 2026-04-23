"""
Решение задач сетевого планирования:
  Вариант 5 — Задача об узких местах (Maximin / Widest Path)
  Вариант 6 — Задача сетевого планирования (CPM, метод критического пути)

Использование:
    python network_solver.py <входной_файл.in>
Выходной файл создаётся автоматически с тем же именем, но расширением .out
"""

import sys
import os
from collections import deque

INF = float("inf")


# ─────────────────────────────────────────────────────────────
#  ВАРИАНТ 5 — Задача об узких местах
# ─────────────────────────────────────────────────────────────


def parse_v5(lines):
    """Разбор входных данных варианта 5."""
    parts = lines[0].split()
    n = int(parts[0])
    s = int(parts[2]) - 1  # 0-индексация

    W = []
    for i in range(1, n + 1):
        row_parts = lines[i].split()
        row = []
        for v in row_parts:
            if v == "*":
                row.append(None)
            else:
                row.append(float(v))
        W.append(row)
    return n, s, W


def solve_v5(n, s, W):
    """
    Алгоритм Дейкстры для задачи об узких местах (maximin / widest path).
    На каждой итерации извлекаем необработанную вершину с максимальным D[u],
    релаксируем её исходящие дуги.
    Итерация = один шаг извлечения вершины из очереди.
    """
    import heapq

    D = [-INF] * n
    P = [s] * n  # все изначально указывают на стартовую вершину
    D[s] = INF
    visited = [False] * n

    # max-heap через отрицание
    pq = [(-INF, s)]  # (-D[v], v)
    iterations = []

    while pq:
        neg_d, u = heapq.heappop(pq)
        if visited[u]:
            continue
        visited[u] = True

        for v in range(n):
            if u == v:
                continue
            w = W[u][v]
            if w is None or w == 0:
                continue
            width = min(D[u], w)
            if width > D[v]:
                D[v] = width
                P[v] = u
                heapq.heappush(pq, (-width, v))

        iterations.append((D[:], P[:]))

    # Оставляем только первые n-1 итераций (без последней если вершина тупиковая)
    return iterations[: n - 1]


def format_val_d(v):
    if v == INF:
        return "*"
    if v == -INF:
        return "-"
    # Целое или дробное
    return str(int(v)) if v == int(v) else str(v)


def format_val_p(v):
    return "-" if v == -1 else str(v + 1)


def write_v5(iterations):
    lines = []
    for i, (D, P) in enumerate(iterations, start=1):
        lines.append(str(i))
        lines.append("D: " + " ".join(format_val_d(v) for v in D))
        lines.append("P: " + " ".join(format_val_p(v) for v in P))
    return "\n".join(lines)


# ─────────────────────────────────────────────────────────────
#  ВАРИАНТ 6 — Сетевое планирование (CPM)
# ─────────────────────────────────────────────────────────────


def parse_v6(lines):
    """Разбор входных данных варианта 6."""
    parts = lines[0].split()
    n = int(parts[0])
    T = int(parts[2])  # заданный план

    jobs = []
    for line in lines[1:]:
        if not line.strip():
            continue
        p = line.split()
        job_id = int(p[0])
        duration = float(p[1])
        preds = [] if p[2] == "*" else list(map(int, p[2:]))
        jobs.append({"id": job_id, "dur": duration, "preds": preds})
    return n, T, jobs


def topological_sort(jobs):
    """Топологическая сортировка (алгоритм Кана)."""
    id_map = {j["id"]: j for j in jobs}
    in_deg = {j["id"]: len(j["preds"]) for j in jobs}
    adj = {j["id"]: [] for j in jobs}

    for j in jobs:
        for p in j["preds"]:
            adj[p].append(j["id"])

    queue = deque(jid for jid, deg in in_deg.items() if deg == 0)
    order = []

    while queue:
        cur = queue.popleft()
        order.append(cur)
        for nxt in adj[cur]:
            in_deg[nxt] -= 1
            if in_deg[nxt] == 0:
                queue.append(nxt)

    if len(order) != len(jobs):
        raise ValueError("Граф содержит цикл — задача некорректна")

    return order, adj, id_map


def solve_v6(jobs, T):
    """
    Метод критического пути (CPM).
    Прямой проход  → ранние сроки  (ebeg, efin)
    Обратный проход → поздние сроки (lbeg, lfin)
    Резерв = lfin - efin
    """
    order, adj, id_map = topological_sort(jobs)

    # ─ Прямой проход ─
    ebeg, efin = {}, {}
    for jid in order:
        j = id_map[jid]
        ebeg[jid] = max((efin[p] for p in j["preds"]), default=0)
        efin[jid] = ebeg[jid] + j["dur"]

    # Длительность критического пути
    T_crit = max(efin[j["id"]] for j in jobs)
    T_used = T  # заданный план (используется для обратного прохода)

    # ─ Обратный проход ─
    lfin, lbeg = {}, {}
    for jid in reversed(order):
        succs = adj[jid]
        lfin[jid] = min((lbeg[s] for s in succs), default=float(T_used))
        lbeg[jid] = lfin[jid] - id_map[jid]["dur"]

    # ─ Резервы ─
    results = []
    for j in sorted(jobs, key=lambda x: x["id"]):
        jid = j["id"]
        res = lfin[jid] - efin[jid]
        results.append(
            {
                "id": jid,
                "ebeg": ebeg[jid],
                "efin": efin[jid],
                "lbeg": lbeg[jid],
                "lfin": lfin[jid],
                "reserve": res,
            }
        )

    return results, T_crit


def fmt(v):
    """Форматирование числа: целое без .0, дробное как есть."""
    return str(int(v)) if v == int(v) else str(v)


def write_v6(results, T_crit):
    lines = [fmt(T_crit)]
    for r in results:
        lines.append(
            f"{r['id']}: {fmt(r['ebeg'])}, {fmt(r['efin'])}, "
            f"{fmt(r['lbeg'])}, {fmt(r['lfin'])}, {fmt(r['reserve'])}"
        )
    return "\n".join(lines)


# ─────────────────────────────────────────────────────────────
#  ВСПОМОГАТЕЛЬНЫЙ ВЫВОД В КОНСОЛЬ
# ─────────────────────────────────────────────────────────────


def print_v5(iterations, n, s):
    print(f"\n{'=' * 50}")
    print(f"  ВАРИАНТ 5 — Задача об узких местах")
    print(f"  Граф: {n} вершин, стартовая вершина: {s + 1}")
    print(f"{'=' * 50}")
    for i, (D, P) in enumerate(iterations, start=1):
        print(f"\n  Итерация {i}:")
        print(
            f"  D: {' '.join(f'[{k + 1}]={format_val_d(v)}' for k, v in enumerate(D))}"
        )
        print(
            f"  P: {' '.join(f'[{k + 1}]={format_val_p(v)}' for k, v in enumerate(P))}"
        )


def print_v6(results, T_crit, T_plan):
    print(f"\n{'=' * 50}")
    print(f"  ВАРИАНТ 6 — Сетевое планирование (CPM)")
    print(
        f"  Оптимальный план: {fmt(T_crit)}"
        + (f"  (заданный: {T_plan})" if T_plan != T_crit else "")
    )
    print(f"{'=' * 50}")
    print(
        f"\n  {'№':>4}  {'ebeg':>6}  {'efin':>6}  {'lbeg':>6}  {'lfin':>6}  {'резерв':>7}  критич."
    )
    print(f"  {'-' * 55}")
    for r in results:
        crit = "  ●" if r["reserve"] == 0 else ""
        print(
            f"  {r['id']:>4}  {fmt(r['ebeg']):>6}  {fmt(r['efin']):>6}  "
            f"{fmt(r['lbeg']):>6}  {fmt(r['lfin']):>6}  {fmt(r['reserve']):>7}{crit}"
        )


# ─────────────────────────────────────────────────────────────
#  ТОЧКА ВХОДА
# ─────────────────────────────────────────────────────────────


def main():
    if len(sys.argv) < 2:
        print("Использование: python network_solver.py <входной_файл.in>")
        sys.exit(1)

    in_path = sys.argv[1]
    if not os.path.isfile(in_path):
        print(f"Ошибка: файл '{in_path}' не найден")
        sys.exit(1)

    # Формируем имя выходного файла
    base, _ = os.path.splitext(in_path)
    out_path = base + "1.out"

    with open(in_path, encoding="utf-8") as f:
        raw = f.read()

    lines = [l.strip() for l in raw.strip().splitlines() if l.strip()]

    # Определяем вариант из первой строки
    header = lines[0].split()
    variant = int(header[1])

    if variant == 5:
        n, s, W = parse_v5(lines)
        iterations = solve_v5(n, s, W)
        print_v5(iterations, n, s)
        output = write_v5(iterations)

    elif variant == 6:
        n, T, jobs = parse_v6(lines)
        results, T_crit = solve_v6(jobs, T)
        print_v6(results, T_crit, T)
        output = write_v6(results, T_crit)

    else:
        print(f"Ошибка: неизвестный вариант {variant} (ожидается 5 или 6)")
        sys.exit(1)

    with open(out_path, "w", encoding="utf-8") as f:
        f.write(output)

    print(f"\n  Результат записан в: {out_path}\n")


if __name__ == "__main__":
    main()

