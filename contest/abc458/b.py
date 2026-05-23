#abc458b
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

H,W = map(int,input().split())

ans = []
for i in range(H):
    ans.append([])
    mx = 4
    for j in range(W):
        minus = 0
        if j == 0:
            minus += 1
        if j == W-1:
            minus += 1
        if i == 0:
            minus += 1
        if i == H-1:
            minus += 1
        ans[-1].append(mx-minus)
    
for line in ans:
    print(*line)
