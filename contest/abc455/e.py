#abc455e
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

N = int(input())
S = list(input())

max_counts = [0,0,0]

for c in S:
    if c == "A":
        max_counts[0] += 1
    elif c == "B":
        max_counts[1] += 1
    else:
        max_counts[2] += 1

checked = [[[False for _ in range(max_counts[2])] for _ in range(max_counts[1])] for _ in range(max_counts[0])]

A_cnt = 0
B_cnt = 0
C_cnt = 0
