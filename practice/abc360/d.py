#abc360d
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

N,T = map(int,input().split())
S = list(map(int,list(input())))
X = list(map(int,input().split()))

X,S = zip(*sorted(zip(X,S)))

ans = 0
target_right_pos = []
idx = 0
current_ants = 0
for i in range(N):
    ant_pos = X[i]
    ant_dir = S[i]

    moved_ant_pos = -1
    if ant_dir == 0:
        moved_ant_pos = ant_pos - T
        while idx < len(target_right_pos) and target_right_pos[idx] < moved_ant_pos:
            idx += 1
            current_ants -= 1
        ans += current_ants
    else:
        moved_ant_pos = ant_pos + T
        target_right_pos.append(moved_ant_pos)
        current_ants += 1

print(ans)