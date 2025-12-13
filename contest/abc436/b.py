#abc436b
import sys
from collections import defaultdict
from collections import deque
import heapq
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

m = [[-1 for _ in range(N)] for _ in range(N)]

m[0][(N-1)//2] = 1
last = (0,(N-1)//2)

for i in range(2,N*N+1):
    upright = ((last[0] - 1) % N, (last[1] + 1) % N)
    down = ((last[0] + 1) % N, last[1])
    if m[upright[0]][upright[1]] == -1:
        m[upright[0]][upright[1]] = i
        last = upright
    else:
        m[down[0]][down[1]] = i
        last = down

for i in range(N):
    print(*m[i])