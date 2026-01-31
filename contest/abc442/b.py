#abc442b
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

Q = int(input())

vol = 0
playing = False

for _ in range(Q):
    A = int(input())
    if A == 1:
        vol += 1
    elif A == 2:
        if vol >= 1:
            vol -= 1
    else:
        playing = not playing
    
    if (playing and (vol >= 3)):
        print("Yes")
    else:
        print("No")