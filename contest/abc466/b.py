#abc466b
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
balls = defaultdict(int)

for i in range(N):
    C,S = map(int,input().split())
    balls[C] = max(balls[C],S)

ans = []
for i in range(1,M+1):
    if i in balls:
        ans.append(balls[i])
    else:
        ans.append(-1)
    
print(*ans)