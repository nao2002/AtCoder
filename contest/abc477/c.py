#abc477c
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
sys.setrecursionlimit(10**6)
sys.set_int_max_str_digits(0)

def input(): return (sys.stdin.readline()).rstrip()

def search(ok:int,ng:int,f:bool)->int:
    while 1<abs(ok-ng):
        mid=(ng+ok)//2
        if f(mid):
            ok=mid
        else:
            ng=mid
    return ok

Q = int(input())
S = input()
T = input()

pos = [-1]

for i in range(len(S)):
    for j in range(len(T)):
        if i+j >= len(S):
            break
        if S[i+j] != T[j]:
            break
    else:
        pos.append(i)

pos.append(len(S)+len(T)+1)
# print(pos)
for _ in range(Q):
    L,R = map(int,input().split())
    L -= 1
    R -= 1
    if L > pos[-1]:
        print("No")
        continue
    l = search(0, len(pos), lambda x: pos[x] < L)

    # print(pos[l+1])
    if (pos[l+1] >= L and pos[l+1]+(len(T)-1) <= R):
        print("Yes")
    else:
        print("No")