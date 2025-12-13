#abc436c
import sys
from collections import defaultdict
from collections import deque
import heapq
# from sortedcontainers import SortedList, SortedDict, SortedSet
try:
    import pypyjit
    pypyjit.set_param('max_unroll_recursion=-1')
except ImportError:
    pass
sys.setrecursionlimit(10**8)
sys.set_int_max_str_digits(0)
def input(): return (sys.stdin.readline()).rstrip()

N,M = map(int,input().split())

placed = set()
ans = 0

for i in range(M):
    R,C = map(int,input().split())

    pos = [(R,C),(R+1,C),(R,C+1),(R+1,C+1)]
    for p in pos:
        if p in placed:
            break
    else:
        for p in pos:
            placed.add(p)
        ans += 1

print(ans)