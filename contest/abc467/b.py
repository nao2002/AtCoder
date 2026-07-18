#abc467b
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
not_take = 0

for i in range(N):
    A,B,S = input().split()
    A = int(A)
    B = int(B)
    remain = B - A
    if S == "keep":
        not_take += remain

print(not_take)