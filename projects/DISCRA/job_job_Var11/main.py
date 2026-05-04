import sys
from collections import deque


def parse_input(filename):
    with open(filename) as f:
        lines = [l.strip() for l in f if l.strip()]
    first = lines[0].split()
    n, variant = int(first[0]), int(first[1])
    cap = []
    for i in range(1, n + 1):
        row = lines[i].split()
        cap.append([0 if x == "*" else int(x) for x in row])
    return n, variant, cap


# ─── graph helpers ────────────────────────────────────────────────────────────


def build_residual(n, cap):
    """Returns adjacency list of edges as (to, cap, rev_index) using standard
    Dinic edge list format: graph[u] = list of [to, cap, rev]"""
    graph = [[] for _ in range(n)]
    for u in range(n):
        for v in range(n):
            if cap[u][v] > 0:
                graph[u].append([v, cap[u][v], len(graph[v])])
                graph[v].append([u, 0, len(graph[u]) - 1])
    return graph


def find_sources_sinks(n, cap):
    has_in = [False] * n
    has_out = [False] * n
    for u in range(n):
        for v in range(n):
            if cap[u][v] > 0:
                has_out[u] = True
                has_in[v] = True
    sources = [i for i in range(n) if not has_in[i]]
    sinks = [i for i in range(n) if not has_out[i]]
    return sources, sinks


def add_super_nodes(graph, n, sources, sinks):
    """Add super-source (n) and super-sink (n+1) with infinite capacity edges.
    Returns new total size."""
    INF = 10**9
    S = n  # super-source index
    T = n + 1  # super-sink index
    graph.append([])  # S
    graph.append([])  # T

    for s in sources:
        graph[S].append([s, INF, len(graph[s])])
        graph[s].append([S, 0, len(graph[S]) - 1])

    for t in sinks:
        graph[t].append([T, INF, len(graph[T])])
        graph[T].append([t, 0, len(graph[t]) - 1])

    return S, T


# ─── Edmonds-Karp ─────────────────────────────────────────────────────────────


def bfs_path(graph, S, T):
    """BFS: returns parent[(node)] = (prev_node, edge_index) or None."""
    n = len(graph)
    visited = [False] * n
    visited[S] = True
    parent = [None] * n
    q = deque([S])
    while q:
        u = q.popleft()
        if u == T:
            break
        for idx, (v, cap, _) in enumerate(graph[u]):
            if not visited[v] and cap > 0:
                visited[v] = True
                parent[v] = (u, idx)
                q.append(v)
    if not visited[T]:
        return None
    # reconstruct path as list of (u, edge_idx)
    path = []
    cur = T
    while cur != S:
        u, idx = parent[cur]
        path.append((u, idx))
        cur = u
    path.reverse()
    return path


def edmonds_karp(graph, S, T):
    iterations = []
    while True:
        path = bfs_path(graph, S, T)
        if path is None:
            break
        # find bottleneck
        bn = min(graph[u][idx][1] for u, idx in path)
        # augment
        for u, idx in path:
            v = graph[u][idx][0]
            rev = graph[u][idx][2]
            graph[u][idx][1] -= bn
            graph[v][rev][1] += bn
        iterations.append(bn)
    return iterations


# ─── Dinic ────────────────────────────────────────────────────────────────────


def bfs_level(graph, S, T):
    n = len(graph)
    level = [-1] * n
    level[S] = 0
    q = deque([S])
    while q:
        u = q.popleft()
        for v, cap, _ in graph[u]:
            if cap > 0 and level[v] < 0:
                level[v] = level[u] + 1
                q.append(v)
    return level if level[T] >= 0 else None


def dfs_blocking(graph, level, iter_, u, T, pushed):
    if u == T:
        return pushed
    while iter_[u] < len(graph[u]):
        v, cap, rev = graph[u][iter_[u]]
        if cap > 0 and level[v] == level[u] + 1:
            d = dfs_blocking(graph, level, iter_, v, T, min(pushed, cap))
            if d > 0:
                graph[u][iter_[u]][1] -= d
                graph[v][rev][1] += d
                return d
        iter_[u] += 1
    return 0


def dinic(graph, S, T):
    INF = 10**9
    iterations = []
    while True:
        level = bfs_level(graph, S, T)
        if level is None:
            break
        iter_ = [0] * len(graph)
        phase_flow = 0
        while True:
            f = dfs_blocking(graph, level, iter_, S, T, INF)
            if f == 0:
                break
            phase_flow += f
        if phase_flow == 0:
            break
        iterations.append(phase_flow)
    return iterations


# ─── extract flow matrix ──────────────────────────────────────────────────────


def extract_flow(n_orig, cap_orig, graph):
    """Compare residual graph against original capacities to get flow matrix."""
    # Build a lookup: for original edge (u,v) with cap>0, find its residual cap
    # The flow on (u,v) = original_cap - residual_cap
    # We need to match original edges to graph edges.
    # Rebuild edge positions the same way build_residual did.
    flow = [[0] * n_orig for _ in range(n_orig)]
    edge_pos = {}  # (u,v) -> index in graph[u]
    # re-trace build order
    pos = [0] * n_orig
    for u in range(n_orig):
        for v in range(n_orig):
            if cap_orig[u][v] > 0:
                idx = pos[u]
                # find the right edge (they were appended in order)
                # count how many forward edges u has so far
                edge_pos[(u, v)] = idx
                pos[u] += 1  # but graph[u] has both fwd and back edges mixed
    # simpler: just re-walk graph edges in same order
    # We'll re-count per-node forward edge indices
    fwd_count = [0] * n_orig
    graph_idx = [0] * n_orig
    # We need to iterate graph[u] and identify forward vs back edges
    # Forward edges: original cap > 0, back edges: original cap == 0
    # Actually we built: for each (u,v) with cap>0:
    #   graph[u].append fwd edge, graph[v].append back edge
    # So we can just check if cap_orig[u][v] > 0 to identify fwd edges
    for u in range(n_orig):
        for edge in graph[u]:
            v, res_cap, _ = edge
            if v < n_orig and cap_orig[u][v] > 0:
                flow[u][v] = cap_orig[u][v] - res_cap
    return flow


# ─── main ─────────────────────────────────────────────────────────────────────


def solve(in_file, out_file):
    n, variant, cap_orig = parse_input(in_file)

    sources, sinks = find_sources_sinks(n, cap_orig)

    graph = build_residual(n, cap_orig)
    if len(sources) == 1 and len(sinks) == 1:
        S, T = sources[0], sinks[0]
    else:
        S, T = add_super_nodes(graph, n, sources, sinks)

    if variant == 1:
        iters = edmonds_karp(graph, S, T)
    else:
        iters = dinic(graph, S, T)

    flow_matrix = extract_flow(n, cap_orig, graph)
    total = sum(iters)

    # format sources/sinks as 1-based
    src_str = " ".join(str(s + 1) for s in sources)
    sink_str = " ".join(str(t + 1) for t in sinks)
    iter_str = ", ".join(str(x) for x in iters)

    lines = []
    lines.append(src_str + " ")
    lines.append(sink_str + " ")
    lines.append(iter_str)
    for row in flow_matrix:
        lines.append(" ".join(str(x) for x in row) + " ")
    lines.append(str(total))

    with open(out_file, "w") as f:
        f.write("\n".join(lines) + "\n")

    print("\n".join(lines))


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python maxflow.py <input_file>")
        sys.exit(1)
    in_file = sys.argv[1]
    out_file = in_file.rsplit(".", 1)[0] + "1.out"
    solve(in_file, out_file)
