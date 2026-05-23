#arc218a
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

A = [list(map(int,input().split())) for _ in range(N)]

seen = {}
same = 0
MOD = 998244353
mul = [1]

alls = set()
for i in range(N):
    alls |= set(A[i])

for i in range(1,N+1):
    mul.append(mul[-1]*M%MOD)

others = {}
for i in range(N):
    included = defaultdict(int)
    for j in range(M):
        included[A[i][j]] += 1

    for num in alls:
        if not num in others:
            others[num] = [N-included[num]]
        else:
            others[num].append((others[num]-1)*(N-included[num]))

for i in range(N):
    included = defaultdict(int)
    for j in range(M):
        included[A[i][j]] += 1
    
    for key, value in included.items():
        if not key in seen:
            seen[key] = [value,1,0,i]
        else:
            lastCount, oneshotValue, others, lastN = seen[key]
            same += ((value * lastCount * oneshotValue * mul[i-(lastN+1)]) % MOD + (others * mul[i-(lastN+1)]) % MOD) % MOD
            seen[key][0] = value
            seen[key][1] = (lastCount * oneshotValue) % MOD
            seen[key][2] = (max(1,seen[key][2])*(M-value))%MOD
            seen[key][3] = i

print((mul[N]*N-same)%MOD)
