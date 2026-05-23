#abc459c
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

N,Q = map(int,input().split())

minus = 0
countMap = defaultdict(int)
counts = [0]*N

reached = [0]*((3*10**5)+1)

countMap[0] = N
reached[0] = N

for _ in range(Q):
    opt, value = map(int,input().split())

    if opt == 1:
        value -= 1
        current = counts[value]
        countMap[current] -= 1
        counts[value] += 1
        new = counts[value]
        countMap[new] += 1
        reached[new] += 1

        if countMap[minus] == 0:
            minus += 1
    
    elif opt == 2:
        if (value+minus) >= len(reached):
            print(0)
        else:
            print(reached[value+minus])
