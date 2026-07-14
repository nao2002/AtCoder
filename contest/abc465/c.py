#abc465c
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
S = list(input())

cur = "r"
last = "x"

ans = [0]*N
l_ptr = 0
r_ptr = N-1

for i in range(N-1,-1,-1):
    if cur == "r":
        if S[i] == "o":
            ans[l_ptr] = i+1
            l_ptr += 1
            cur = "l"
        else:
            ans[r_ptr] = i+1
            r_ptr -= 1
    else:
        if S[i] == "o":
            ans[r_ptr] = i+1
            r_ptr -= 1
            cur = "r"
        else:
            ans[l_ptr] = i+1
            l_ptr += 1

print(*ans)