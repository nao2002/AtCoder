#abc481d
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

N,M,S,T = map(int,input().split())

minimum = [None]*(N+1)

graph = defaultdict(set)
queue = []

for i in range(M):
    A,B = map(int,input().split())
    graph[A].add(B)
    if minimum[A] is None:
        minimum[A] = B
    elif B < minimum[A]:
        minimum[A] = B

costs = [-1]*(N+1)
costs[S] = 0

for nxt in graph[S]:
    if nxt == minimum[S]:
        costs[nxt] = 0
        heapq.heappush(queue, (0, nxt))
    else:
        costs[nxt] = 1
        heapq.heappush(queue, (1, nxt))

while queue:
    _, cur = heapq.heappop(queue)
    cost = costs[cur]
    if cur == T:
        print(cost)
        exit()

    for nxt in graph[cur]:
        if costs[nxt] == -1:
            if nxt == minimum[cur]:
                costs[nxt] = cost
                heapq.heappush(queue, (cost, nxt))
            else:
                costs[nxt] = cost+1
                heapq.heappush(queue, (cost+1, nxt))
        elif nxt == minimum[cur] and costs[nxt] > cost:
            costs[nxt] = cost
            heapq.heappush(queue, (cost, nxt))

print(-1)