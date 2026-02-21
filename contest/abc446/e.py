#abc446e
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

M, A, B = map(int,input().split())

ans = []
for i in range(M):
    for j in range(M):
        if (i % M == 0 or j % M == 0):
            continue
        mod_span = -1
        first = (i)*B + (j)*A
        second = (j)*B + (first)*A
        if first % M == 0 or second % M == 0:
            continue
        mod_first = first % M
        mod_second = second % M
        if mod_second <= mod_first:
            mod_second += M
        mod_span = mod_second - mod_first
        # print(i,j,first,second,mod_span)
        if M % mod_span == 0 and mod_first % mod_span != 0:
            ans.append((i,j,mod_span))

print(ans)
print(len(ans))