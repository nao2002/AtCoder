#abc435c
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

max_len = A[0] - 1

for i in range(1,N+1):
    if max_len <= 0 or i == N:
        print(i)
        exit()
    max_len -= 1
    max_len = max(max_len, A[i]-1)