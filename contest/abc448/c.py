#abc448c
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
sys.setrecursionlimit(10**8)
sys.set_int_max_str_digits(0)

def input(): return (sys.stdin.readline()).rstrip()

N,Q = map(int,input().split())

A = list(map(int,input().split()))

s = SortedList()
for i in range(N):
    s.add(A[i])

for i in range(Q):
    K = int(input())
    B = list(map(int,input().split()))
    for j in range(K):
        s.remove(A[B[j]-1])
    print(s[0])
    for j in range(K):
        s.add(A[B[j]-1])