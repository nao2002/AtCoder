#abc483d
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

N = int(input())
A = list(map(int,input().split()))

ans = 0
sums = [A[0]]
for i in range(1,N):
    sums.append(sums[-1]+A[i])

mx_plus = [0]*(N)
min_minus = [0]*(N)
mx_plus[-1] = sums[-1]
min_minus[-1] = sums[-1]
for i in range(N-2,-1,-1):
    mx_plus[i] = max(mx_plus[i+1],sums[i])
    min_minus[i] = min(min_minus[i+1],sums[i])

ans = max(0,mx_plus[0])+min(0,min_minus[0])*-1

passed_mx = 0
passed_mn = 0
for i in range(N):
    target = A[i]
    mx = mx_plus[i]
    bef_mx = mx_plus[i]
    mn = min_minus[i]
    bef_mn = min_minus[i]
    if target >= 0:
        mx = max(0,mx - target*2)
        mn = min(0,mn + target*-2)
    else:
        mx = max(0, mx + target*-2)
        mn = min(0, mn - target*2)
    if mx <= passed_mx:
        mx = passed_mx
    if mn >= passed_mn:
        mn = passed_mn
    ans = max(
        ans,
        max(0, mx) + min(0,mn)*-1
    )
    passed_mx = max(passed_mx, sums[i])
    passed_mn = min(passed_mn, sums[i])
print(ans+1)