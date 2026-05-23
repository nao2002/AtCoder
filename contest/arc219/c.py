#arc219c
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

def search_upper(ok:int,ng:int,f:bool)->int:
    while 1<abs(ok-ng):
        mid=(ng+ok)//2
        if f(mid):
            ok=mid
        else:
            ng=mid
    return ok

def search_lower(ok:int,ng:int,f:bool)->int:
    while 0<abs(ok-ng):
        mid=(ng+ok)//2
        if f(mid):
            ng=mid
        else:
            ok=mid+1
    return ok

H,W = map(int,input().split())

N = int(input())
floors = defaultdict(list)
HALF = W//2

max_cost = -1
same_exist = False
through = 0
for i in range(N):
    A,B = map(int,input().split())
    floors[A].append(B)

ans = 0
for values in floors.values():
    values.sort()
    l = search_upper(0, len(values), lambda i: values[i] < HALF)
    r = search_lower(0, len(values), lambda i: values[i] >= HALF)

    cost = (values[l]-1)*2 + (W - values[r])*2
    if cost == (W-1):
        same_exist = True
        through += 1
        ans += W-1
    elif cost > (W-1):
        through += 1
        ans += W-1
    else:
        if cost > max_cost:
            max_cost = cost
        ans += cost

if through % 2 == 0 or (through > 2 and same_exist):
    
    
