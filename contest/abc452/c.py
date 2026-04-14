#abc452c
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

chars = [set() for _ in range(N)]

AB = []
for i in range(N):
    A,B = map(int,input().split())
    AB.append((A,B))

M = int(input())
S_all = []

for i in range(M):
    S = input()
    S_all.append(S)
    for j in range(N):
        a,b = AB[j]
        if a == len(S):
            chars[j].add(S[b-1])

for s in S_all:
    if len(s) != N:
        print("No")
        continue
    for i in range(N):
        if not s[i] in chars[i]:
            print("No")
            break
    else:
        print("Yes")