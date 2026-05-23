#abc456a
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

X = int(input())

for i in range(1,7):
    for j in range(1,7):
        for k in range(1,7):
            if i+j+k == X:
                print("Yes")
                exit()

print("No")
