#abc441c
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

N,K,X = map(int,input().split())
A = list(map(int,input().split()))

A.sort()

current = 0
for i in range(K):
    pos = (N-(N-K)-1)-i
    current += A[pos]
    if current >= X:
        print((i+1) + (N-K))
        exit()

print(-1)