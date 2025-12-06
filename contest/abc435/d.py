#abc435d
import sys
from collections import defaultdict
from collections import deque
import heapq
# from sortedcontainers import SortedList, SortedDict, SortedSet
try:
    import pypyjit
    pypyjit.set_param('max_unroll_recursion=-1')
except ImportError:
    pass
sys.setrecursionlimit(10**8)
sys.set_int_max_str_digits(0)
def input(): return (sys.stdin.readline()).rstrip()

N,M = map(int,input().split())

graph = defaultdict(set)
rev_graph = defaultdict(set)
ans = [False]*(N+1)

for i in range(M):
    X,Y = map(int,input().split())
    graph[X].add(Y)
    rev_graph[Y].add(X)

Q = int(input())

def dfs(pos):
    ans[pos] = True
    for next in rev_graph[pos]:
        if not ans[next]:
            dfs(next)

for i in range(Q):
    query = list(map(int,input().split()))
    if query[0] == 1:
        if not ans[query[1]]:
            dfs(query[1])
    else:
        if ans[query[1]]:
            print("Yes")
        else:
            print("No")