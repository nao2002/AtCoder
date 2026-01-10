#abc437b
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

H,W,N = map(int,input().split())

d = dict()
for i in range(H):
    A = list(map(int,input().split()))
    for a in A:
        d[a] = i

ans = [0]*H
for i in range(N):
    B = int(input())
    if B in d:
        ans[d[B]] += 1

print(max(ans))