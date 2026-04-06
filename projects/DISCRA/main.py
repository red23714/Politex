import sys

with open("input_9.in", "r") as f:
    lines = f.read().splitlines()

first_line = list(map(int, lines[0].split()))
n = first_line[0]
variant = first_line[1]

start_vertex = first_line[2] - 1 if variant == 2 else None

matrix = []
for line in lines[1:]:
    matrix.append(list(map(int, line.split())))

# Подготовка списка рёбер для Краскала
edges = []
for i in range(n):
    for j in range(i + 1, n):
        if matrix[i][j] != 0:
            edges.append((matrix[i][j], i, j))


def kruskal():
    parent = list(range(n))
    rank = [0] * n

    def find(u):
        while parent[u] != u:
            parent[u] = parent[parent[u]]
            u = parent[u]
        return u

    def union(u, v):
        u_root = find(u)
        v_root = find(v)
        if u_root == v_root:
            return False
        if rank[u_root] < rank[v_root]:
            parent[u_root] = v_root
        else:
            parent[v_root] = u_root
            if rank[u_root] == rank[v_root]:
                rank[u_root] += 1
        return True

    mst_weight = 0
    mst_edges = []
    mst_matrix = [[0] * n for _ in range(n)]

    for w, u, v in sorted(edges, key=lambda x: (x[0], x[1], x[2])):
        if union(u, v):
            mst_weight += w
            mst_edges.append((u, v))
            mst_matrix[u][v] = mst_matrix[v][u] = 1

    return mst_weight, mst_matrix, mst_edges


def prim(start):
    in_mst = [False] * n
    key = [float("inf")] * n
    parent = [-1] * n
    key[start] = 0

    mst_weight = 0
    mst_edges = []
    mst_matrix = [[0] * n for _ in range(n)]

    for _ in range(n):
        u = min((k for k in range(n) if not in_mst[k]), key=lambda x: key[x])
        in_mst[u] = True

        if parent[u] != -1:
            v = parent[u]
            mst_edges.append((min(u, v), max(u, v)))
            mst_matrix[u][v] = mst_matrix[v][u] = 1
            mst_weight += matrix[u][v]

        # важно: порядок по индексам сохраняем
        for v in range(n):
            if matrix[u][v] != 0 and not in_mst[v] and matrix[u][v] < key[v]:
                key[v] = matrix[u][v]
                parent[v] = u

    return mst_weight, mst_matrix, mst_edges


if variant == 1:
    total_weight, mst_matrix, mst_edges = kruskal()
else:
    total_weight, mst_matrix, mst_edges = prim(start_vertex)

with open("input_1.out", "w") as f:
    f.write(str(total_weight) + "\n")
    for row in mst_matrix:
        f.write(", ".join(map(str, row)) + "\n")
    f.write(" ".join(f"({a + 1}, {b + 1})" for a, b in mst_edges))
