#abc482d
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
X = list(map(int,input().split()))


for k in range(N):
    if k == 0:
        ans = (X[-1] - X[0]) // 2
        print(ans)
    elif k == N-1:
        ans = (X[-1] - X[0]) // 2
        print(ans)
    else:
        to_l = (X[k] - X[k-1]) // 2
        l_l = (X[k-1] - X[0]) // 2
        l_r = (X[-1] - X[k]) // 2
        l = to_l + max(l_l,l_r)

        to_r = (X[k+1] - X[k]) // 2
        r_l = (X[k] - X[0]) // 2
        r_r = (X[-1] - X[k+1]) // 2
        r = to_r + max(r_l, r_r)

        ans = min(l,r)
        print(ans)


# 5 7 11 15

# 2 4 10 14 18
# 4 6 12 16

# 4 6 8 10 12 14 16
# 5 7 9 11 13 15 : 1
# 