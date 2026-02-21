#abc446e
import sys
from collections import defaultdict
from collections import deque
import heapq
import math
# from sortedcontainers import SortedList, SortedDict, SortedSet
try:
    import pypyjit
    pypyjit.set_param('max_unroll_recursion=-1')
except ImportError:
    pass
sys.setrecursionlimit(10**8)
sys.set_int_max_str_digits(0)

def input(): return (sys.stdin.readline()).rstrip()

M, A, B = map(int,input().split())

graph = dict()

for i in range(M):
    for j in range(M):
        graph[(i,j)] = (j, (A*j+B*i)%M)

reachable = [[False for _ in range(M)] for _ in range(M)]
visited = set()

def dfs(cur):
    if cur in visited:
        return
    visited.add(cur)
    if cur[0] == 0 or cur[1] == 0:
        reachable[cur[0]][cur[1]] = True
        return
    if cur in graph:
        if not graph[cur] in visited:
            dfs(graph[cur])
        reachable[cur[0]][cur[1]] = reachable[graph[cur][0]][graph[cur][1]]
    else:
        reachable[cur[0]][cur[1]] = False

for i in range(M):
    for j in range(M):
        if not (i,j) in visited:
            dfs((i,j))

ans = 0
for i in range(M):
    for j in range(M):
        if not reachable[i][j]:
            ans += 1

print(ans)