#abc436e
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

N = int(input())
P = list(map(int,input().split()))

incorrect = 0
for i in range(N):
    if P[i] != i+1:
        incorrect += 1

s = ((incorrect - 1) + 1) * ((incorrect - 1) // 2)
if (incorrect - 1) % 2 == 1:
    s += (incorrect - 1) // 2 + 1
print(s)