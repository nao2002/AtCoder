#abc446c
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

T = int(input())

for _ in range(T):
    N,D = map(int,input().split())
    A = list(map(int,input().split()))
    B = list(map(int,input().split()))

    queue = []
    idx = 0
    egg = 0
    
    for i in range(N):
        queue.append([i,A[i]])
        egg += A[i]
        
        used = 0
        while idx < len(queue) and used < B[i]:
            if queue[idx][1] + used <= B[i]:
                used += queue[idx][1]
                idx += 1
            else:
                queue[idx][1] -= (B[i] - used)
                used = B[i]
        
        egg -= B[i]

        while idx < len(queue) and queue[idx][0]+D <= i:
            egg -= queue[idx][1]
            idx += 1

    print(egg)