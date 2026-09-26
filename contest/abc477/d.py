#abc477d
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
sys.setrecursionlimit(10**6)
sys.set_int_max_str_digits(0)

def input(): return (sys.stdin.readline()).rstrip()

N,Q = map(int,input().split())

query = [list(input().split()) for _ in range(Q)]
# print(query)

query_two_pos = [0]
non_covered = set(range(1,N+1))

for i in range(len(query)):
    if query[i][0] == "1":
        if int(query[i][1]) in non_covered:
            non_covered.remove(int(query[i][1]))
        else:
            non_covered.add(int(query[i][1]))
    elif query[i][0] == "2":
        query_two_pos.append(i)

done = set()
ans = ["a"]*N

tail = query_two_pos[-1]
if tail != Q-1:
    for i in range(tail+1, Q):
        _, x = query[i]
        x = int(x)
        if x in non_covered:
            non_covered.remove(x)
        else:
            non_covered.add(x)
            
while len(query_two_pos) != 1:
    cand = query[query_two_pos[-1]][1]
    # print(non_covered)
    for pos in non_covered:
        done.add(int(pos))
        ans[int(pos)-1] = cand
    non_covered.clear()
    top = query_two_pos[-2]

    for i in range(tail-1, top, -1):
        _, x = query[i]
        x = int(x)
        if x in done:
            continue
        if x in non_covered:
            non_covered.remove(x)
        else:
            non_covered.add(x)

    query_two_pos.pop()
    tail = query_two_pos[-1]

print("".join(ans))