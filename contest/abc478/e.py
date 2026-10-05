#abc478e
import sys
from collections import defaultdict
from collections import deque
import heapq
import math
from sortedcontainers import SortedList, SortedDict, SortedSet
try:
    import pypyjit
    pypyjit.set_param('max_unroll_recursion=-1')
except ImportError:
    pass
sys.setrecursionlimit(10**6)
sys.set_int_max_str_digits(0)

def input(): return (sys.stdin.readline()).rstrip()

N,Q = map(int,input().split())

# 小さいもの -> 大きいもの への有向グラフ
graph = defaultdict(list)
rev_graph = defaultdict(list)

for _ in range(Q):
    t,u,v = map(int,input().split())
    u -= 1
    v -= 1
    graph[u].append((v,t))
    rev_graph[v].append((u,t))

visited = [False]*N

ans = [0]*N

# 強連結成分分解(SCC): グラフGに対するSCCを行う
# 入力: <N>: 頂点サイズ, <G>: 順方向の有向グラフ, <RG>: 逆方向の有向グラフ
# 出力: (<ラベル数>, <各頂点のラベル番号>)
def scc(N, G, RG):
    order = []
    used = [0]*N
    group = [None]*N
    def dfs(s):
        used[s] = 1
        for t in G[s]:
            if not used[t[0]]:
                dfs(t[0])
        order.append(s)
    def rdfs(s, col):
        group[s] = col
        used[s] = 1
        for t in RG[s]:
            if not used[t[0]]:
                rdfs(t[0], col)
    for i in range(N):
        if not used[i]:
            dfs(i)
    used = [0]*N
    label = 0
    for s in reversed(order):
        if not used[s]:
            rdfs(s, label)
            label += 1
    return label, group

lb, gp = scc(Q, graph, rev_graph)

minimum = 0

def dfs(pos):
    global minimum
    my_score = ans[pos]
    for bigger_pos, type in graph[pos]:
        if type == 0:
            if not visited[bigger_pos]:
                ans[bigger_pos] = my_score
                visited[bigger_pos] = True
                dfs(bigger_pos)
        if type == 1:
            if gp[bigger_pos] == gp[pos]:
                print("No")
                exit()
            if not visited[bigger_pos]:
                ans[bigger_pos] = my_score+1
                visited[bigger_pos] = True
                dfs(bigger_pos)

    for smaller_pos, type in rev_graph[pos]:
        if type == 0:
            if not visited[smaller_pos]:
                ans[smaller_pos] = my_score
                visited[smaller_pos] = True
                dfs(smaller_pos)
        if type == 1:
            if gp[smaller_pos] == gp[pos]:
                print("No")
                exit()
            if not visited[smaller_pos]:
                ans[smaller_pos] = my_score-1
                if my_score-1 < minimum:
                    minimum = my_score-1
                visited[smaller_pos] = True
                dfs(smaller_pos)

for i in range(N):
    if not visited[i]:
        visited[i] = True
        dfs(i)

maximum = 1
for i in range(N):
    ans[i] += -1*minimum + 1
    if ans[i] > maximum:
        maximum = ans[i]

if maximum > N:
    print("No")
    exit()

print(*ans)