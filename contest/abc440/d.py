#abc440d

#ps. AC解だが1分足らずで提出できず
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
# sys.set_int_max_str_digits(0)

def input(): return (sys.stdin.readline()).rstrip()

N,Q = map(int,input().split())

A = list(map(int,input().split()))

A.sort()

a = []
for i in range(N):
    a.append((A[i],i))
# print(a)

def find_x(left, right, search_target):
    while left <= right:
        center = (left+right)//2
        if a[center][0] == search_target:
            left = center + 1
        elif a[center][0] <= search_target:
            left = center+1
        else:
            right = center-1
    else:
        return right
    
def find_y(left, right, search_target, l_s):
    while left <= right:
        center = (left+right)//2
        if a[center][0] < search_target + (a[center][1] - l_s):
            left = center+1
        else:
            right = center-1
    else:
        return right

for i in range(Q):
    X,Y = map(int,input().split())

    if (X+(Y-1)) < (a[0][0]):
        ans = (X+(Y-1))
        print(ans)
        continue

    if X > a[-1][0]:
        ans = (X+(Y-1))
        print(ans)
        continue

    if X < a[0][0]:
        l = -1
        l_s = -1
    else:
        l = find_x(0, N-1, X)
        l_s = a[l][1]

        if a[l][0] == X:
            l_s -= 1

    r = find_y(0, N-1, X+(Y-1), l_s)

    print(X+(Y-1)+(a[r][1]-l_s))
