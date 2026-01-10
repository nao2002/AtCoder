#abc440b
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
# sys.set_int_max_str_digits(0)

def input(): return (sys.stdin.readline()).rstrip()

N = int(input())

T = list(map(int,input().split()))

t = []
for i in range(N):
    t.append((T[i],i+1))

t.sort()

ans = [t[0][1],t[1][1],t[2][1]]
print(*ans)