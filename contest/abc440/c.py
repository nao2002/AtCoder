#abc440c
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
# sys.set_int_max_str_digits(0)

def input(): return (sys.stdin.readline()).rstrip()

T = int(input())

for _ in range(T):
    N,W = map(int,input().split())
    C = list(map(int,input().split()))

    a = [0,C[0]]
    for i in range(1,N):
        a.append(a[i-1]+C[i])
    print(a)

    s = sum(C)

    ans = math.inf

    for i in range(W*2):
        tmp = 0
        

