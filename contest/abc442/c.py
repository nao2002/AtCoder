#abc442c
import sys
from collections import defaultdict
from collections import deque
import heapq
import math
import itertools
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

s = [[N-1,i] for i in range(N)]

for m in range(M):
    A,B = map(int,input().split())
    s[A-1][0] -= 1
    s[B-1][0] -= 1

s.sort()

ans = [None]*N

for cnt,idx in s:
    if cnt < 3:
        ans[idx] = 0
    else:
        ans[idx] = (cnt*(cnt-1)*(cnt-2)) // 6
print(*ans)