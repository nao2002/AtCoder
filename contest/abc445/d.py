#abc445d
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
sys.setrecursionlimit(10**8)
sys.set_int_max_str_digits(0)
def input(): return (sys.stdin.readline()).rstrip()

H,W,N = map(int,input().split())

h_heap = []
w_heap = []

used = set()

ans = [None]*N
for i in range(N):
    h,w = map(int,input().split())
    heapq.heappush(h_heap,(-h,i,w))
    heapq.heappush(w_heap,(-w,i,h))

h_offset = 0
w_offset = 0
for _ in range(N):
    while h_heap[0][1] in used:
        heapq.heappop(h_heap)
    while w_heap[0][1] in used:
        heapq.heappop(w_heap)

    cur_h = H - h_offset
    cur_w = W - w_offset

    if h_heap[0][0] == -cur_h:
        height, idx, width = heapq.heappop(h_heap)
        ans[idx] = (h_offset+1,w_offset+1)
        w_offset += width
        used.add(idx)
    else:
        width, idx, height  = heapq.heappop(w_heap)
        ans[idx] = (h_offset+1,w_offset+1)
        h_offset += height
        used.add(idx)

# print("======")
for a in ans:
    print(*a)

