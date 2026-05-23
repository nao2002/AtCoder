#abc458d
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

X = int(input())

Q = int(input())

cur_nums = SortedList()
cur_nums.add(X)

for _ in range(Q):
    A,B = map(int,input().split())
    cur_nums.add(A)
    cur_nums.add(B)

    print(cur_nums[len(cur_nums)//2])