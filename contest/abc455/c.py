#abc455c
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

N,K = map(int,input().split())
A = list(map(int,input().split()))

dict = defaultdict(int)
s = 0

for i in range(N):
    s += A[i]
    dict[A[i]] += A[i]

values = sorted(dict.values(), reverse=True)

ans = s
for i in range(K):
    ans -= values[i]
    if ans == 0:
        break
print(ans)