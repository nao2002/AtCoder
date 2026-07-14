#arc422b
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

def check(num,target):
    if num == 0:
        if 0 < target:
            return True
        return False
    test = 1+(8+(4+(1*4+(num-1)*2*4)))*(num)//2
    if test < target:
        return True
    return False

for _ in range(T):
    N = int(input())
    
    if N == 1:
        print(0)
        continue

    num = search(0, 10**18, lambda x: check(x,N))
    # print(f"num:{num}")
    if num == 0:
        N -= 1
        base = 0
        num += 1
    else:
        N -= 1+(8+(4+(1*4+(num-1)*2*4)))*(num)//2
        base = (12+(4+(2*(1*4)+2*((num-1)*2)*4)))*(num)//2
        num += 1
    
    first_step_count = 1+(num-1)*2
    if N <= first_step_count:
        if N == 1:
            print(base+1)
        else:
            print(base+1+2*(N-1))
        continue
    
    N -= first_step_count
    base += 1+(first_step_count-1)*2

    one_cycle_count = (num)*2
    one_cycle_score = 1+(one_cycle_count-1)*2

    whole_cycle_count = 2*one_cycle_count
    whole_cycle_score = 2*one_cycle_score

    if N <= whole_cycle_count:
        rep = (N-1) // one_cycle_count
        mod = (N-1) % one_cycle_count

        print(base+one_cycle_score*rep+1+2*mod)
        continue

    N -= whole_cycle_count
    base += whole_cycle_score

    if N == 1:
        print(base+1)
    else:
        print(base+1+(N-1)*2)
