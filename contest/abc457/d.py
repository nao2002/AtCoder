#abc457d
import sys
from collections import defaultdict
from collections import deque
import heapq
import math
from sortedcontainers import SortedList, SortedDict, SortedSet
try:
    import pypyjit
    pypyjit.set_param('max_unroll_recursion=-1')
except ImportError:
    pass
sys.setrecursionlimit(10**8)
sys.set_int_max_str_digits(0)

def input(): return (sys.stdin.readline()).rstrip()

N,K = map(int,input().split())

A = list(map(int,input().split()))

MAX = 10**18 * (2*10**5)

left = 1
right = MAX

def search(ok:int,ng:int,f:bool)->int:
    # okは条件を満たす領域の外側
    # ngは条件を満たさない領域の外側
    # fは条件を満たすかどうかの評価関数
        # lambda i:a[i]<x xを含まない最大のiを返す
        # lambda i:a[i]<=x xを含む最大のiを返す
    while 1<abs(ok-ng):
        mid=(ng+ok)//2
        if f(mid):
            ok=mid
        else:
            ng=mid
    return ok

def solve(target):
    usable = K
    for i in range(N):
        if A[i] >= target:
            continue
        remain = (target - A[i]) // (i+1)
        if (target - A[i]) % (i+1) != 0:
            remain += 1
        if remain > usable:
            return False
        usable -= remain
    return True

print(search(1, MAX+1, solve))