#abc459b
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

N = int(input())
S = list(input().split())

chars = ["abc","def","ghi","jkl","mno","pqrs","tuv","wxyz"]

ans = ""
for i in range(N):
    s = S[i]
    for j in range(len(chars)):
        c = chars[j]
        if s[0] in c:
            ans += str(j+2)
            break

print(ans)