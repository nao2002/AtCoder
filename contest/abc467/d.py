#abc467d
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
    P1, P2, Q1, Q2, R1, R2, S1, S2 = map(int,input().split())

    a1 = ((Q2-P2)*(S1-R1))
    a2 = ((S2-R2)*(Q1-P1))

    # c3 = ((P2-R2)*(S1-R1)*(Q1-P1))
    
    # print(c1,c2)

    if a1 != a2:
        print("Yes")
    else:
        if c1 == c3:
            print("Yes")
        else:
            print("No")

