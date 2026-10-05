#abc478d
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

N,Q = map(int,input().split())

s = SortedList()

cur_set = set()
cur_cnt = defaultdict(int)

inc_s = SortedList()
dec_s = SortedList()

for i in range(Q):
    L,R,X = map(int,input().split())
    L -= 1
    R -= 1
    inc_s.add((L,X))
    dec_s.add((R,X))

l = 0
r = 0
ans = []
for i in range(N):
    while r < len(inc_s) and inc_s[r][0] <= i:
        if cur_cnt[inc_s[r][1]] == 0:
            cur_set.add(inc_s[r][1])
        cur_cnt[inc_s[r][1]] += 1
        r += 1

    while l < len(dec_s) and dec_s[l][0] < i:
        cur_cnt[dec_s[l][1]] -= 1
        if cur_cnt[dec_s[l][1]] == 0:
            cur_set.remove(dec_s[l][1])
        l += 1

    ans.append(len(cur_set))

print(*ans)