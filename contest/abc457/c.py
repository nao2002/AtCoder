#abc457c
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

N,K = map(int,input().split())

arrs = []

for i in range(N):
    L, *A = map(int,input().split())
    A = list(A)
    arrs.append(A)
C = list(map(int,input().split()))

passed = 0

for i in range(N):
    add = len(arrs[i])*C[i]
    if passed + add >= K:
        remain = K - passed
        idx = (remain-1) % len(arrs[i])
        print(arrs[i][idx])
        exit()
    passed += add