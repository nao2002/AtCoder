#abc484c
# Source: https://github.com/nao2002/AtCoder
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

N = int(input())
A = list(map(int,input().split()))
B = list(map(int,input().split()))

diff_l = 0
diff_r = N

def same(A,B):
    if len(A) == len(B):
        for i in range(len(A)):
            if A[i] != B[i]:
                return False
        else:
            return True
    return False

for i in range(N):
    if A[i] != B[i]:
        diff_l = i
        break

for i in range(N-1,-1,-1):
    if A[i] != B[i]:
        diff_r = i
        break

if same(A,B):
    print("Yes")
    exit()

a_l = A[diff_l]
a_r = A[diff_r]

ans = False

A.pop(diff_l)
A.insert(diff_r,a_l)
ans = ans | same(A,B)
A.pop(diff_r)
A.insert(diff_l, a_l)

A.pop(diff_r)
A.insert(diff_l,a_r)
ans = ans | same(A,B)
A.pop(diff_l)
A.insert(diff_r,a_r)

if ans:
    print("Yes")
else:
    print("No")