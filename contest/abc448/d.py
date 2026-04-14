#abc448d
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

N = int(input())
A = list(map(int,input().split()))

ans = ["No"]*N

tree = defaultdict(list)

for i in range(N-1):
    U,V = map(int,input().split())

    tree[U].append(V)
    tree[V].append(U)

visited = set()
seen = defaultdict(int)
seen_keys = set()

def dfs(pos):
    if len(seen_keys) > 0:
        ans[pos-1] = "Yes"
    else:
        ans[pos-1] = "No"
    
    for nxt in tree[pos]:
        if not nxt in visited:
            if seen[A[nxt-1]] == 1:
                seen_keys.add(A[nxt-1])
            seen[A[nxt-1]] += 1
            visited.add(nxt)
            dfs(nxt)
            seen[A[nxt-1]] -= 1
            if seen[A[nxt-1]] == 1:
                seen_keys.discard(A[nxt-1])

seen[A[0]] = 1
visited.add(1)
dfs(1)

for i in range(N):
    print(ans[i]) 