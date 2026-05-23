#abc455b
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

H,W = map(int,input().split())

grid = [list(input()) for _ in range(H)]
ans = 0
for left_up in range(H*W):
    for right_down in range(H*W):
        arr = []
        lu = [left_up//W,left_up%W]
        rd = [right_down//W,right_down%W]
        if rd[0] < lu[0] or rd[1] < lu[1]:
            continue
        for i in range(rd[0]-lu[0]+1):
            for j in range(rd[1]-lu[1]+1):
                arr.append(grid[lu[0]+i][lu[1]+j])
        isOK = True
        for i in range(len(arr)//2):
            if arr[i] != arr[len(arr)-(i+1)]:
                isOK = False
        if isOK:
            ans += 1

print(ans)