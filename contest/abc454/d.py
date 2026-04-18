#abc454d
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

checker = ["(","x","x",")"]

for _ in range(T):
    A = list(input())
    B = list(input())

    stack_a = []
    stack_b = []

    N = max(len(A),len(B))

    for i in range(N):
        if i < len(A):
            stack_a.append((A[i]))
            flag_a = True
        else:
            flag_a = False
        if i < len(B):
            stack_b.append((B[i]))
            flag_b = True
        else:
            flag_b = False
        
        for j in range(4):
            if len(stack_a) >= 4:
                if flag_a and stack_a[-1*(j+1)] != checker[-1*(j+1)]:
                    flag_a = False
            else:
                flag_a = False
            if len(stack_b) >= 4:
                if flag_b and stack_b[-1*(j+1)] != checker[-1*(j+1)]:
                    flag_b = False
            else:
                flag_b = False
            if not flag_a and not flag_b:
                break
        
        if flag_a:
            stack_a.pop()
            stack_a.pop()
            stack_a[-2] = "x"
        if flag_b:
            stack_b.pop()
            stack_b.pop()
            stack_b[-2] = "x"

    
    if len(stack_a) == len(stack_b):
        for i in range(len(stack_a)):
            if stack_a[i] != stack_b[i]:
                print("No")
                break
        else:
            print("Yes")
    else:
        print("No")
