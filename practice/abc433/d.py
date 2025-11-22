#abc433d
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

DIG = 11

N,M = map(int,input().split())
A = list(map(int,input().split()))

mod_dict = [defaultdict(int) for _ in range(DIG)]

for i in range(N):
    for j in range(DIG):
        mod = (A[i] * 10**j) % M
        mod_dict[j][mod] += 1

# print(mod_dict)

ans = 0
for i in range(N):
    num = A[i]
    mod = num % M
    l = len(str(num))

    target = (M - mod) % M
    ans += mod_dict[l][target]

print(ans)