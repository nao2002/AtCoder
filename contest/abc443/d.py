#abc443d
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
    N = int(input())
    R = list(map(int,input().split()))

    ans = 0
    hq = []
    for i in range(N):
        heapq.heappush(hq, (R[i],i))
    
    checked = set()

    while hq:
        (num, pos) = heapq.heappop(hq)
        if pos in checked:
            continue
        checked.add(pos)
        if pos != 0:
            if R[pos] < (R[pos-1] -1):
                ans += (R[pos-1] - 1) - R[pos]
                R[pos-1] = R[pos] + 1
                heapq.heappush(hq,(R[pos-1],pos-1))
        if pos != (N-1):
            if R[pos] < (R[pos+1] -1):
                ans += (R[pos+1] - 1) - R[pos]
                R[pos+1] = R[pos] + 1
                heapq.heappush(hq,(R[pos+1],pos+1))
        
    print(ans)