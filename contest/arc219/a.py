#arc219a
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

N,M = map(int,input().split())
S = [list(input()) for _ in range(N)]

current = []
checked = set()

for i in range(M):
    zero = set()
    one = set()
    for j in range(N):
        if not j in checked: 
            if S[j][i] == "0":
                zero.add(j)
            else:
                one.add(j)

    if len(one) >= len(zero):
        current.append("1")
        checked |= one
    else:
        current.append("0")
        checked |= zero

    if i == M-1 and len(zero) != 0 and len(one) != 0:
        print("No")
        exit()
print("Yes")
print("".join(current))
