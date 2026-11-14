#abc482b
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

S = [list(input()) for i in range(5)]

ans = 5

for i in range(5):
    cnt = 5
    for j in range(5):
        if S[i][j] == "o":
            cnt -= 1
    if cnt < ans:
        ans = cnt

for j in range(5):
    cnt = 5
    for i in range(5):
        if S[i][j] == "o":
            cnt -= 1
    if cnt < ans:
        ans = cnt

cnt = 5
for i in range(5):
    if S[i][i] == "o":
        cnt -= 1
    if cnt < ans:
        ans = cnt

cnt = 5
for i in range(5):
    if S[i][4-i] == "o":
        cnt -= 1
    if cnt < ans:
        ans = cnt

print(ans)