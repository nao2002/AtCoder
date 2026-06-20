#abc463c
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
sys.setrecursionlimit(10**8)
sys.set_int_max_str_digits(0)

def input(): return (sys.stdin.readline()).rstrip()


def search(ok:int,ng:int,f:bool)->int:
    while ok<ng:
        mid=(ng+ok)//2
        if f(mid):
            ok=mid+1
        else:
            ng=mid
    return ok
    
N = int(input())

times = [0]*(N+1)
times[N] = 10**9+1

height = [0]*(N+1)

HL = []

for i in range(N):
    H,L = map(int,input().split())
    HL.append((L,H))
HL.sort()

for i in range(N-1,-1,-1):
    height[i] = max(height[i+1], HL[i][1])
    times[i] = HL[i][0]

print(times)
print(height)
Q = int(input())
T = list(map(int,input().split()))
for i in range(Q):
    t = T[i]
    idx = search(0, len(T)-1, lambda v:times[v]<=t)
    print(height[idx])