#abc448e
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

K,M = map(int,input().split())
N = 0

DIV = 10007


for i in range(K):
    c,l = map(int,input().split())
    
    if c == 0:
        N *= 10**l
    else:
        N *= 10**l
        # N += int("1"*l)*c

print((N//M)%DIV)
    

# seen = set()
# for i in range(100):
#     num = int(str("1")*(i+1))%DIV
#     print(num, num in seen)
#     seen.add(num)