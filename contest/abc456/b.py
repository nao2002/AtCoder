#abc456b
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

A = [list(map(int,input().split())) for _ in range(3)]

fours = [0,0,0]
fives = [0,0,0]
sixes = [0,0,0]

for i in range(3):
    for j in range(6):
        if A[i][j] == 4:
            fours[i] += 1
        elif A[i][j] == 5:
            fives[i] += 1
        elif A[i][j] == 6:
            sixes[i] += 1

ans = 0

for i in range(3):
    for j in range(3):
        if i == j:
            continue
        for k in range(3):
            if j == k or i == k:
                continue
            ans += fours[i]*fives[j]*sixes[k]

print(ans/(6**3))