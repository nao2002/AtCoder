#abc466d
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

N,M = map(int,input().split())

rc = [map(int,input().split()) for _ in range(M)]

r_checked = [False]*(N+1)
c_checked = [False]*(N+1)

ans = 0
for i in range(len(rc)-1,-1,-1):
    r,c = rc[i]
    if not r_checked[r] and not c_checked[c]:
        ans += 1
    r_checked[r] = True
    c_checked[c] = True

print(ans)