#abc478b
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

N,V = map(int,input().split())
W = list(map(int,input().split()))

ans = 0

for i in range(N):
    for j in range(i+1,N):
        for k in range(j+1,N):
            if i+j+k+3 <= V:
                ans = max(ans,W[i]+W[j]+W[k])
print(ans)