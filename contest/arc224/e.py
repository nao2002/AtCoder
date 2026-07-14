#arc422b
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
    S = list(input())

    stack = []

    ans = len(S)
    for i in range(len(S)-1,-1,-1):
        c = S[i]
        stack.append(c)
        while len(stack) >= 1 and stack[-1] == "A":
            if len(stack) >= 2 and stack[-2] == "B":
                if len(stack) >= 3 and stack[-3] == "C":
                    for i in range(3):
                        stack.pop()
                    ans -= 3
                else:
                    for i in range(2):
                        stack.pop()
                    ans -= 2
            else:
                stack.pop()
                ans -= 1

    print(ans)