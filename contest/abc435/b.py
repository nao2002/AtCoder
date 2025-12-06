#abc435b
import sys
from collections import defaultdict
from collections import deque
import heapq
# from sortedcontainers import SortedList, SortedDict, SortedSet
try:
    import pypyjit
    pypyjit.set_param('max_unroll_recursion=-1')
except ImportError:
    pass
sys.setrecursionlimit(10**8)
sys.set_int_max_str_digits(0)
def input(): return (sys.stdin.readline()).rstrip()

N = int(input())
A = list(map(int,input().split()))

l = 0
r = 0

cur = 0
ans = 0
for i in range(N):
    for j in range(l,N):
        cur += A[r]
        for k in range(l,r+1):
            if cur % A[k] == 0:
                break
        else:
            ans += 1
        # if cur % A[l] != 0 and cur % A[r] != 0:
        #     ans += 1
            # print(l,r, cur, A[l], A[r])
        r += 1
    cur = 0
    
    l += 1
    r = l
print(ans)