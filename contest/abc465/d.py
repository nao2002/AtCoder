#abc465d
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

T = int(input())

for _ in range(T):
    X,Y,K = map(int,input().split())
    base = []
    target = []

    while X > 0 or Y > 0:
        if X > 0:
            base.append(X%K)
            X //= K
        if Y > 0:
            target.append(Y%K)
            Y //= K
    
    ans = len(base)+len(target)
    for i in range(min(len(base),len(target))):
        if base[len(base)-1-i] != target[len(target)-1-i]:
            break
        ans -= 2
    
    print(ans)