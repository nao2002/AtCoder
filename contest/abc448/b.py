#abc448b
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

N,M = map(int,input().split())
C = list(map(int,input().split()))

ans = 0
for i in range(N):
    A,B = map(int,input().split())
    A -= 1
    if C[A] >= B:
        ans += B
        C[A] -= B
    elif C[A] > 0:
        ans += C[A]
        C[A] = 0
    
print(ans)