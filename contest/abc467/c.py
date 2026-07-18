#abc467c
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

A = list(map(int,input().split()))
B = list(map(int,input().split()))

dp = [[10**9 for _ in range(M)] for _ in range(N)]
# print(dp)
for i in range(M):
    if A[0] != i:
        dp[0][i] = 1
    else:
        dp[0][i] = 0

ans = 0
for i in range(1,N):
    # print(dp)
    for j in range(M):
        if A[i] != j:
            if B[i-1] == j:
                dp[i][j] = dp[i-1][0]+1
            else:
                dp[i][j] = dp[i-1][1]+1
        else:
            if B[i-1] == j:
                dp[i][j] = dp[i-1][0]
            else:
                dp[i][j] = dp[i-1][1]

# print(dp)
print(min(dp[N-1][0],dp[N-1][1]))

