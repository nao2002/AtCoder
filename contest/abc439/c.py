#abc439c
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

max_xy = int(math.sqrt(N))

one = set()
over_two = set()
for x in range(1, max_xy+1):
    for y in range(x+1, max_xy+1):
        res = x**2 + y**2
        if res in over_two or res > N:
            continue
        elif res in one:
            one.remove(res)
            over_two.add(res)
        else:
            one.add(res)

sort = sorted(one)
print(len(sort))
print(*sort)
