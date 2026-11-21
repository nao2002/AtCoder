#abc485e
# Source: https://github.com/nao2002/AtCoder
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

N,M = map(int,input().split())
graph = defaultdict(list)

for i in range(M):
    U,V,C = map(int,input().split())
    graph[U].append((V,C))

visited = {}

queue = [(1,0)]
visited[1] = [0,0]

idx = 0
last_dist = 0
lazy = set()
while idx < len(queue):
    pos, dist = queue[idx]
    idx += 1

    if pos == N:
        print(dist)
        exit()

    if last_dist != dist:
        for nxt in lazy:
            visited[nxt][0] = 0
        lazy.clear()
        last_dist = dist

    for nxt,color in graph[pos]:
        if nxt in visited:
            if visited[nxt][0] == 0 or visited[nxt][0] == color:
                continue
            if visited[nxt][1] == dist+1 and visited[pos][0] != color:
                visited[nxt][0] = 0
                continue
            elif visited[nxt][1] != dist+1 and visited[pos][0] != color:
                if visited[nxt][1] == dist:
                    lazy.add(nxt)
                else:
                    visited[nxt][0] = 0
                queue.append((nxt, dist+1))
                continue
        elif visited[pos][0] != color:
            visited[nxt] = [color,dist+1]
            queue.append((nxt, dist+1))

print(-1)



"""
4 6
1 2 1
1 3 2
3 2 2
2 3 3
2 4 1
3 4 2

"""