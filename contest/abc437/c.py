#abc437c
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
# sys.set_int_max_str_digits(0)

def input(): return (sys.stdin.readline()).rstrip()

T = int(input())

for _ in range(T):
    N = int(input())
    wp = []
    weight = 0
    power = 0
    score = []
    for i in range(N):
        W,P = map(int,input().split())
        wp.append((W,P,i))
        score.append((W+P,i))
        weight += W

    score.sort(reverse=True)
    
    for i in range(N):
        idx = score[i][1]
        weight -= wp[idx][0]
        power += wp[idx][1]
        if power >= weight:
            print(N-(i+1))
            break
    else:
        print(0)