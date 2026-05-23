#arc218b
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

user = ["Alice","Bob"]
for _ in range(T):
    N = int(input())
    A = list(map(int,input().split()))
    A.sort()

    runlength = []

    last = A[0]
    cont = 1
    for i in range(1,len(A)):
        if A[i] == last:
            cont += 1
        else:
            runlength.append((last, cont))
            last = A[i]
            cont = 1
    else:
        runlength.append((last, cont))

    # print(runlength)
    if runlength[0][0] >= 2:
        print(user[0])
        continue

    current = runlength[0][0] % 2

    if runlength[0][1] >= 2:
        print(user[current])
        continue

    last = runlength[0][0]
    winner = current
    for i in range(1,len(runlength)):
        if runlength[i][0] - last >= 2:
            current ^= 1
            winner = current
            break
        elif runlength[i][1] >= 2:
            winner = current
            break
        last = runlength[i][0]
    
    print(user[winner])