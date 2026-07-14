#abc466c
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

ans = 0

left = 1
right = 2

while right <= N:
    print("?",left,right,flush=True)
    res = input()
    if res == "Yes":
        ans += right - left
        right += 1
    else:
        left += 1
        if right <= left:
            right = left + 1

print("!",ans,flush=True)