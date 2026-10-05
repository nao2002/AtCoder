#abc478c
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

N,K = map(int,input().split())

A = list(map(int,input().split()))

answer = sorted(A)

diff_start = -1
cnt = 0

for i in range(N):
    if diff_start == -1:
        if A[i] != answer[i]:
            diff_start = i
            cnt = 1
    else:
        if A[i] != answer[i]:
            cnt = i - diff_start+1

if cnt > K:
    print("No")
else:
    print("Yes")