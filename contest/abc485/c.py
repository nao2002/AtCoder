#abc485c
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
C = list(map(int,input().split()))

arr = []

cur = C[0]
start = 0

for i in range(1,N):
    if C[i] != cur:
        arr.append((cur,start,i))
        cur = C[i]
        start = i
else:
    arr.append((cur,start,N))

cur_p = 0

ans = []
for i in range(N):
    if arr[cur_p][0] != C[i]:
        cur_p += 1

    if cur_p == 0:
        ans.append(arr[cur_p][2]-i)
    elif cur_p == len(arr)-1:
        ans.append(i-arr[cur_p][1]+1)
    else:
        ans.append(min(arr[cur_p][2]-i, i-arr[cur_p][1]+1))
print(*ans)