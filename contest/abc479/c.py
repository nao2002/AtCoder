#abc479c
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

N, W = map(int,input().split())

bit = 0

menu = []

for i in range(N):
    A,B = map(int,input().split())
    menu.append((A,B))

ans = 0
for i in range(2**N):
    yum = 0
    wei = 0
    for j in range(N):
        if i & 1 == 1:
            a,b = menu[j]
            yum += a
            wei += b
            if wei > W:
                break
        i = i >> 1
    if wei <= W:
        ans = max(ans,yum)
print(ans)