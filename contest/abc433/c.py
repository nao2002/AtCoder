#abc433c
import sys
from collections import defaultdict
from collections import deque
import heapq
import pypyjit
# from sortedcontainers import SortedList, SortedDict, SortedSet
pypyjit.set_param('max_unroll_recursion=-1')
sys.setrecursionlimit(10**8)
sys.set_int_max_str_digits(0)

def input(): return (sys.stdin.readline()).rstrip()

S = list(input())

compress = []

last = ""
cnt = 0
for s in S:
    if last != s:
        if cnt != 0:
            compress.append((last, cnt))
        last = s
        cnt = 1
    else:
        cnt += 1
else:
    compress.append((last, cnt))

ans = 0
for i in range(len(compress)-1):
    c,l = compress[i]
    c_n, l_n = compress[i+1]
    if int(c) == int(c_n)-1:
        minimum = min(l,l_n)
        ans += minimum

print(ans)
