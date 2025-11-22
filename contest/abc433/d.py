#abc433d
import sys
from collections import defaultdict
from collections import deque
import heapq
import pypyjit
# from sortedcontainers import SortedList, SortedDict, SortedSet
pypyjit.set_param('max_unroll_recursion=-1')
sys.setrecursionlimit(10**8)
sys.set_int_max_str_digits(0)

def input(): return (sys.stdin.readline()).rstrip()
