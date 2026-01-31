#abc443c
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

N,T = map(int,input().split())

A = list(map(int,input().split()))
if N >= 1 and A[-1] != T:
    A.append(T)
elif N == 0:
    A.append(T)

ans = 0
last_close = 0

for i in range(len(A)):
    a = A[i]
    if (a - last_close) >= 100:
        if (last_close == 0):
            last_close = -100
        ans += (a - (last_close + 100))
        # print(a, (a - (last_close + 100)))
        last_close = a
    elif (last_close == 0):
        ans += a
        # print(a, a)
        last_close = a

print(ans)