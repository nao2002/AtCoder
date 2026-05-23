#arc219b
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

T = int(input())
MOD = 998244353

for _ in range(T):
    N = int(input())
    P = list(map(int,input().split()))
    
    ans = 0
    any_reversed = False
    for i in range(N):
        if P[i] == (i+1):
            ans = (ans + (N-i-1)) % MOD
        else:
            any_reversed = True
            break

    # そのままが答えに含まれる
    if not any_reversed:
        ans = (ans + 1) % MOD

    print(ans)

# 3 1 2

# 1 4 2 3

# 4 1 2 3
# 2 4 1 3
# 3 4 2 1

# 2 1 4 3

# 