#abc456d
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

S = list(input())

aend = 0
bend = 0
cend = 0

cnt = 0

ans = 0

MOD = 998244353
for i in range(len(S)):
    if S[i] == "a":
        ans += cnt - aend + 1
        aend += bend + cend + 1
        cnt += bend + cend + 1
    elif S[i] == "b":
        ans += cnt - bend + 1
        bend += aend + cend + 1
        cnt += aend + cend + 1
    elif S[i] == "c":
        ans += cnt - cend + 1
        cend += aend + bend + 1
        cnt += aend + bend + 1
    ans %= MOD
    cnt %= MOD
    aend %= MOD
    bend %= MOD
    cend %= MOD

print(ans%MOD)