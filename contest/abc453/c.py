#abc453c
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

def sign(x):
    return (x > 0) - (x < 0)

N = int(input())
L = list(map(int,input().split()))

cur = 0.5

ans = 0

def dfs(cur, cnt, pos):
    global ans
    if cur < 0 and cur + L[pos] >= 0:
        cnt += 1
        if cnt > ans:
            ans = cnt
    cur += L[pos]
    if pos+1 < N:
        dfs(cur, cnt, pos+1)
    cur -= L[pos]
    if cur < 0 and cur + L[pos] >= 0:
        cnt -= 1

    if cur > 0 and cur - L[pos] <= 0:
        cnt += 1
        if cnt > ans:
            ans = cnt
    cur -= L[pos]
    if pos+1 < N:
        dfs(cur, cnt, pos+1)

dfs(cur, 0, 0)

print(ans)