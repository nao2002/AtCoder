#abc485b
# Source: https://github.com/nao2002/AtCoder
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

N,M = map(int,input().split())

for i in range(N):
    S = list(input())
    x = 0
    last = -1
    for j in range(M):
        if S[j] == "x":
            x += 1
            last = j
    if x >= 1:
        if x == M:
            print(f"Faucet {i+1}")
        else:
            print(f"Drain {last+1}")
        exit()