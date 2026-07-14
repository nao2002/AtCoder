#arc422b
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

T = int(input())

for _ in range(T):
    N,M = map(int,input().split())
    graph = defaultdict(set)
    for i in range(M):
        U,V = map(int,input().split())
        graph[U].add(V)
        graph[V].add(U)

    checked = [False]*(N+1)
    ans = [None]*N
    def dfs(pos,num):
        checked[pos] = True
        ans[pos-1] = num
        for nxt in graph[pos]:
            if not checked[nxt]:
                dfs(nxt,num+1)
    
    dfs(1,0)
    print(*ans)