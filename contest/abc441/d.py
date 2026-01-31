#abc441d
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

N,M,L,S,T = map(int,input().split())
graph = defaultdict(list)

for i in range(M):
    U,V,C = map(int,input().split())
    graph[U].append((V,C))


queue = []
ans = set()

for v,c in graph[1]:
    # 次の頂点, 累計コスト, 通った回数
    if L == 1:
        if S <= c <= T:
            ans.add(v)
    elif c <= T:
        queue.append((v,c,1))

if L == 1:
    print(*sorted(ans))
    exit()

idx = 0
while idx < len(queue):
    current, cost, count = queue[idx]
    for v,c in graph[current]:
        if count+1 == L:
            if S <= cost+c <= T:
                ans.add(v)
        elif cost+c <= T:
            queue.append((v,cost+c,count+1))
    idx += 1

print(*sorted(ans))