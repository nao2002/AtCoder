#abc485d
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

N = int(input())
S = list(input())
a_cnt = [0]*N
b_cnt = [0]*N
c_cnt = [0]*N

a_all = 0
b_all = 0
c_all = 0

for i in range(N):
    a_cnt[i] = max(a_cnt[i],a_cnt[i-1])
    b_cnt[i] = max(b_cnt[i],b_cnt[i-1])
    c_cnt[i] = max(c_cnt[i],c_cnt[i-1])
    if S[i] == "A":
        a_cnt[i] += 1
        a_all += 1
    elif S[i] == "B":
        b_cnt[i] += 1
        b_all += 1
    else:
        c_cnt[i] += 1
        c_all += 1

def check(i, a, b, c):
    if a_cnt[i] - a > 0 and b_cnt[i] - b > 0 and c_cnt[i] - c > 0:
        return True
    return False

def check_r(i):
    if a_all - a_cnt[i-1] > 0 and b_all - b_cnt[i-1] > 0 and c_all - c_cnt[i-1] > 0:
        return False
    return True

first = binary_search(0, N-2, lambda x: check(x, 0, 0, 0))

ans = 0
for i in range(first,N-2):
    l = i
    r = N
    mid_l = binary_search(l+1, N, lambda x: check(x, a_cnt[l], b_cnt[l], c_cnt[l]))
    mid_r = binary_search(mid_l+1, N, check_r)-1

    # print(l,mid_l,mid_r,r)
    if mid_l > N-4 or mid_r > N-3:
        continue
    ans += mid_r-mid_l
print(ans)