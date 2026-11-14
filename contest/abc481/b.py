#abc481b
# Source: https://github.com/nao2002/AtCoder
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

N,K = map(int,input().split())
A = list(map(int,input().split()))

ans = 0

if A[0] == K:
    ans += 1

for i in range(N-1):
    if A[i] < K <= A[i+1]:
        ans += 1
        continue
    if A[i] > K >= A[i+1]:
        ans += 1
        continue
print(ans)