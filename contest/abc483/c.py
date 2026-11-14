#abc483c
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

N,M = map(int,input().split())
base = defaultdict(int)
add = {}

for i in range(M):
    A,B = map(int,input().split())
    base[A]+=1
    if not A in add:
        add[A] = {}
    if not B in add[A]:
        add[A][B] = 0
    add[A][B] += 1

mx = -1
second = -1
for i in range(1,N+1):
    if mx == -1:
        mx = i
        continue
    if base[i] > base[mx] or (base[i] == base[mx] and i < mx):
        second = mx
        mx = i
        continue

    if second == -1:
        second = i
        continue
    if base[i] > base[second] or (base[i] == base[second] and i < second):
        second = i
        continue

for i in range(1,N+1):
    cur_mx_idx = mx
    if i == cur_mx_idx:
        cur_mx_idx = second
    cur_mx = base[cur_mx_idx]
    if i in add:
        for key in add[i]:
            if base[key] + add[i][key] > cur_mx or (base[key] + add[i][key] == cur_mx and key < cur_mx_idx):
                cur_mx = base[key] + add[i][key]
                cur_mx_idx = key
    print(cur_mx_idx)