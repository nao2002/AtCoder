#abc482c
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

def binary_search(left, right, check):
    while left < right:
        mid = (left + right) // 2

        if check(mid):
            right = mid
        else:
            left = mid + 1

    return left

H,W,N,Q = map(int,input().split())

x_base = defaultdict(int)
y_base = defaultdict(int)

for i in range(N):
    R,C = map(int,input().split())

    x_base[R] += 1
    y_base[C] += 1

x_key = list(x_base.keys())
x_key.sort()

# print(x_key)


cnt = 0
for i in range(len(x_key)):
    cnt += x_base[x_key[i]]
    x_key[i] = [x_key[i],cnt]

y_key = list(y_base.keys())
y_key.sort()

# print(y_key)

cnt = 0
for i in range(len(y_key)):
    cnt += y_base[y_key[i]]
    y_key[i] = [y_key[i],cnt]

# print(x_key)
# print(y_key)

for q in range(Q):
    a,b,c = map(int,input().split())

    if a == 1:
        l = binary_search(0, len(x_key), lambda i: x_key[i][0] >= b)
        r = binary_search(0, len(x_key), lambda i: x_key[i][0] > c)
        cnt_l = 0
        if l != 0:
            l -= 1
            cnt_l = x_key[l][1]
        cnt_r = 0
        if r != 0:
            r -= 1
            cnt_r = x_key[r][1]
        print(cnt_r - cnt_l)
    else:
        l = binary_search(0, len(y_key), lambda i: y_key[i][0] >= b)
        r = binary_search(0, len(y_key), lambda i: y_key[i][0] > c)
        cnt_l = 0
        if l != 0:
            l -= 1
            cnt_l = y_key[l][1]
        cnt_r = 0
        if r != 0:
            r -= 1
            cnt_r = y_key[r][1]
        print(cnt_r - cnt_l)