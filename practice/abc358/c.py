#abc358c
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

N,M = map(int,input().split())

popcorns = []
for i in range(N):
    S = list(input())
    tmp = 0
    for j in range(M):
        if S[j] == "o":
            tmp = (tmp << 1) | 1
        else:
            tmp = tmp << 1
    popcorns.append(tmp)
    print(bin(tmp))

# print(popcorns)

viewed = set()
NEEDED = (1 << M) - 1

queue = []
idx = 0
for i in range(N):
    if popcorns[i] == NEEDED:
        print(1)
        exit()
    queue.append((i,1,popcorns[i],(1 << i)))
    viewed.add(1 << i)

while len(queue) > idx:
    (cur_shop, cnt, founds, checked) = queue[idx]
    # print(f"Current: {cur_shop, cnt, founds, checked}")
    for i in range(N):
        tmp_checked = checked | (1 << i)
        if not tmp_checked in viewed:
            new_founds = founds | popcorns[i]
            if new_founds == NEEDED:
                print(cnt+1)
                exit()
            queue.append((i, cnt+1, new_founds, tmp_checked))
            viewed.add(tmp_checked)

    idx += 1