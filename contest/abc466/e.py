#abc466e
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

N,K = map(int,input().split())


def range_max(arr):
    n = len(arr)

    dp_internal_sum = [[0,0,0]]*(n+1)
    dp_suffix_sum = [0]*(n+1)

    left = 0

    for i in range(n):
        if arr[i] > dp_suffix_sum[i]+arr[i]:
            dp_suffix_sum[i + 1] = arr[i]
            left = i
        else:
            dp_suffix_sum[i + 1] = dp_suffix_sum[i]+arr[i]

        if dp_suffix_sum[i]+arr[i] > dp_internal_sum[i][0]:
            dp_internal_sum[i+1] = [dp_suffix_sum[i]+arr[i],left,i]
        else:
            dp_internal_sum[i+1] = [dp_internal_sum[i][0],dp_internal_sum[i][1],dp_internal_sum[i][2]]

        if arr[i] > dp_internal_sum[i+1][0]:
            left = i
            dp_internal_sum[i+1] = [arr[i],left,i]

    # print(dp_internal_sum)
    # print(dp_suffix_sum)
    return dp_internal_sum[n]

cards = [list(map(int,input().split())) for _ in range(N)]

for _ in range(K):
    diff = []
    for i in range(N):
        diff.append(cards[i][1]-cards[i][0])
    # print(*diff)
    num,left,right = range_max(diff)
    # print(num,left,right)
    if num > 0:
        for i in range(left,right+1):
            cards[i][0],cards[i][1] = cards[i][1],cards[i][0]

ans = 0
for i in range(N):
    ans += cards[i][0]
print(ans)