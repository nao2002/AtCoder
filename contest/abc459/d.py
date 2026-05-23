#abc459d
import sys
from collections import defaultdict
from collections import deque
import heapq
import math
from sortedcontainers import SortedList, SortedDict, SortedSet
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

    max_cnt = 0

    counts = SortedList()

    d = defaultdict(int)

    for c in S:
        d[c] += 1
    
    sort = sorted(d.items(), key=lambda x: x[1])
    
    for c,v in sort:
        counts.add((v,c))

    # print(list_kvp)
    half_ceil = len(S) // 2 + len(S) % 2

    if counts[-1][0] > half_ceil:
        print("No")
        continue
    
    ans = [""]*len(S)
    ptr = 0
    last_c = ""
    used_idx = -1
    for i in range(len(S)):
        # print(counts)
        remain, char = counts[-1]
        used_idx = -1
        if char == last_c:
            remain, char = counts[-2]
            used_idx = -2
        counts.pop(len(counts)+used_idx)
        if remain > 1:
            counts.add((remain-1, char))
        last_c = char
        ans[ptr] = char

        ptr += 1

    
    print("Yes")
    print("".join(ans))