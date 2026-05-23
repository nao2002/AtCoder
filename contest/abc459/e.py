#abc459e
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
P = list(map(int,input().split()))
C = list(map(int,input().split()))
D = list(map(int,input().split()))

MOD = 998244353

tree = defaultdict(list)

for i in range(N-1):
    p = P[i]
    tree[p-1].append(i+1)

fact = [0]



def dfs(pos):
