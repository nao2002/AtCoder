#abc437d
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

N,M = map(int,input().split())

A = list(map(int,input().split()))
B = list(map(int,input().split()))
B.sort()

b = [B[0]]
for i in range(1,M):
    b.append(b[i-1]+B[i])

ans = 0

def search(ok,ng,num):
    # okは条件を満たす領域の外側
    # ngは条件を満たさない領域の外側
    while 1<abs(ok-ng):
        mid=(ng+ok)//2
        if B[mid] <= num:
            ok=mid
        else:
            ng=mid
    return ok

for i in range(N):
    if A[i] >= B[-1]:
        ans += (A[i]*M) - b[-1]
    elif A[i] < B[0]:
        ans += b[-1] - (A[i]*M)
    else:
        spl_idx = search(0, M-1, A[i])
        ans += ((A[i]*(spl_idx+1)) - b[spl_idx]) + ((b[-1] - b[spl_idx]) - (A[i]*(M-(spl_idx+1))))

print(ans % 998244353)
