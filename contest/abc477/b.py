#abc477b
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

N,D = map(int,input().split())

X = list(map(int,input().split()))

pos = [(X[i],i+1) for i in range(N)]

pos.sort()

ans = []

for i in range(N):
    near = D+1
    if i != 0:
        near = min(near,abs(pos[i][0] - pos[i-1][0]))

    if i < N-1:
        near = min(near,abs(pos[i][0] - pos[i+1][0]))

    if near >= D:
        ans.append(pos[i][1])

ans.sort()
print(len(ans))
print(*ans)