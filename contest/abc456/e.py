#abc456e
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
    queue = []
    idx = 0

    N,M = map(int,input().split())
    uv = []
    graph = defaultdict(list)
    for i in range(M):
        U,V = map(int,input().split())
        U -= 1
        V -= 1
        uv.append((U,V))
    
    W = int(input())
    S = [list(input()) for _ in range(N)]

    for i in range(M):
        for j in range(W):
            U,V = uv[i]
            if S[U][j] == "o" and S[V][(j+1)%W] == "o":
                graph[(N*j)+U].append((N*((j+1)%W))+V)
            if S[V][j] == "o" and S[U][(j+1)%W] == "o":
                graph[(N*j)+V].append((N*((j+1)%W))+U)
    
    for i in range(N):
        for j in range(W):
            if S[i][j] == "o" and S[i][(j+1)%W] == "o":
                graph[(N*j)+i].append((N*((j+1)%W))+i)

    visited = [[0 for _ in range(N)] for _ in range(W)]
    uf = UnionFind(N)

    for i in range(N):
        if S[i][0] == "o":
            queue.append((i,0))
            visited[0][i] = 1
    
    done = False
    while idx < len(queue):
        city, day = queue[idx]
        pos = (N*day)+city
        visited[day][city] = 2
        nextday = (day+1) % W
        for nxt in graph[pos]:
            nextcity = nxt % N
            if visited[nextday][nextcity] == 2 and uf.same():
                done = True
                break
            if visited[nextday][nextcity] == 0:
                queue.append((nextcity, nextday))
                visited[nextday][nextcity] = 1
        if done:
            break
        idx += 1
    if not done:
        print("No")
    else:
        print("Yes")