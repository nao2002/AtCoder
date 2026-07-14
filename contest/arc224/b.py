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
        if 0 <= target:
            return True
        return False
    if num == 1:
        if 4 < target:
            return True
        return False
    if num == 2:
        if 16 < target:
            return True
        return False
    test = 4+((12+4*(1+(num-1)*2))*(num-1))//2
    if test < target:
        return True
    return False
# search(0,10,lambda x: check(x,17))
for _ in range(T):
    N = int(input())

    num = search(0, 10**18, lambda x: check(x,N))
    # print(f"num:{num}")
    if num == 0:
        arr = [0,1,2,4]
        print(arr[N-1])
    else:
        if num == 1:
            N -= 4
            base = 4
        elif num == 2:
            N -= 16
            base = 4+(4*1)+(16*1)
        else:
            N -= 4+((12+4*(1+(num-1)*2))*(num-1))//2
            base = 4+(4*(num-1))+((2*8+(2*2*(num-1)*4))*(num-1))//2
        
        if N <= 2*num:
            if N == 1:
                print(base+1)
            else:
                mod = (N-1)%(2*num)
                print(base+1+(2*mod))
            continue
        N -= 2*num
        base += (1+2+2*(2*(num-1)))

        cycle = (1+2*(2*num))
        if N <= (num+2*(2*num)):
            rep = (N-1) // (1+(2*num))
            mod = (N-1) % (1+(2*num))

            if mod == 0:
                print(base+(cycle*rep)+1)
            else:
                print(base+(cycle*rep)+1+(2*mod))
            continue
        
        N -= 2+(2*(2*num))
        base += cycle*2

        mod = (N-1) % (2+(2*num))
        if mod == 0:
            print(base+1)
        else:
            print(base+1+(2*mod))
    


# b問題メモ
# 0 1 1 2
# 1 2 1 2 2 1 2 2 1 2 2 2
# 12221222212222122222

#4*(1+num*2)

#0 1 2 4
#5 7 8 10 12 13 15 17 18 20 22 24 -> 4+(4*1 + 8*2) 2が2
#1  2  2  2  1  2  2  2  2  1  2  2  2  2  1  2  2  2  2  2
#25 27 29 31 32 34 36 38 40 41 43 45 47 49 50 52 54 56 58 60 -> 4+(4*1 + 8*2)+(4*1+16*2) 2が4 次は2が6->6*4=24


#base += (1+2+2*(2*(num-1)))

#16+32+48

# ddd d d ddd
# dcc c c cc
# dcb b b b
# dcb a a b
# dcb a a b
# dcb b b b
# dc