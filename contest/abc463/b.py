#abc463b
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

N,X = input().split()
N = int(N)

S = [list(input()) for _ in range(N)]

c = ["A","B","C","D","E"]
idx = c.index(X)
for i in range(N):
    if S[i][idx] == "o":
        print("Yes")
        exit()
print("No")