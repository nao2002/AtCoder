#abc452d
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

S = list(input())
T = list(input())

ans = 0
end = len(S)-1
t_ptr = len(T)-1
for i in range(len(S)-1,-1,-1):
    if t_ptr > 0 and S[i] == T[t_ptr]:
        t_ptr -= 1
    if t_ptr < 0:
        
    